#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sstream>
#include <algorithm>
#include <csignal>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <unistd.h>    // For sysconf
#include <climits>     // For _SC_CLK_TCK

struct ProcessInfo {
    int pid;
    std::string name;
    float cpu_usage;
    float memory_usage;
};

// Function to get CPU info
void get_cpu_info() {
    std::ifstream cpuinfo("/proc/stat");
    std::string line;
    if (cpuinfo.is_open()) {
        std::getline(cpuinfo, line);  // Read the first line for CPU stats
        std::cout << "CPU Info: " << line << std::endl;
    }
}

// Function to get memory info
void get_memory_info() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal") != std::string::npos) {
            std::cout << "Total Memory: " << line << std::endl;
        }
        if (line.find("MemFree") != std::string::npos) {
            std::cout << "Free Memory: " << line << std::endl;
        }
    }
}

// Function to get a list of processes
std::vector<ProcessInfo> get_processes() {
    std::vector<ProcessInfo> processes;
    DIR* dir = opendir("/proc");
    struct dirent* entry;
    if (dir == nullptr) return processes;

    while ((entry = readdir(dir)) != nullptr) {
        if (isdigit(entry->d_name[0])) { // Skip non-process directories
            ProcessInfo pinfo;
            pinfo.pid = atoi(entry->d_name);

            // Read process name
            std::ifstream cmdline_file(std::string("/proc/") + entry->d_name + "/comm");
            if (cmdline_file.is_open()) {
                std::getline(cmdline_file, pinfo.name);
            }

            // Read memory usage
            std::ifstream status_file(std::string("/proc/") + entry->d_name + "/status");
            if (status_file.is_open()) {
                std::string line;
                while (std::getline(status_file, line)) {
                    if (line.find("VmRSS") != std::string::npos) { // Resident Set Size
                        std::istringstream ss(line);
                        std::string name;
                        int memory;
                        ss >> name >> memory;
                        pinfo.memory_usage = memory / 1024.0f; // Convert to MB
                        break;
                    }
                }
            }

            // Calculate CPU usage
            std::ifstream stat_file(std::string("/proc/") + entry->d_name + "/stat");
            if (stat_file.is_open()) {
                std::string line;
                std::getline(stat_file, line);
                std::istringstream ss(line);
                std::string field;
                for (int i = 0; i < 13; ++i) ss >> field;  // Skip first 13 fields
                long utime, stime;
                ss >> utime >> stime;
                long total_time = utime + stime;
                pinfo.cpu_usage = static_cast<float>(total_time) / sysconf(_SC_CLK_TCK);  // CPU time in seconds
            }

            processes.push_back(pinfo);
        }
    }

    closedir(dir);
    return processes;
}

// Comparator to sort processes by CPU usage
bool compare_by_cpu(const ProcessInfo& a, const ProcessInfo& b) {
    return a.cpu_usage > b.cpu_usage;
}

// Comparator to sort processes by memory usage
bool compare_by_memory(const ProcessInfo& a, const ProcessInfo& b) {
    return a.memory_usage > b.memory_usage;
}

// Function to kill a process by PID
void kill_process(int pid) {
    if (kill(pid, SIGTERM) == 0) {
        std::cout << "Process " << pid << " terminated successfully." << std::endl;
    } else {
        std::cerr << "Failed to terminate process " << pid << "." << std::endl;
    }
}

// Function to display processes with the option to sort
void display_processes(std::vector<ProcessInfo>& processes, bool sort_by_cpu = true) {
    // Sort the processes
    if (sort_by_cpu) {
        std::sort(processes.begin(), processes.end(), compare_by_cpu);
    } else {
        std::sort(processes.begin(), processes.end(), compare_by_memory);
    }

    // Display process info
    std::cout << "PID    Name                   CPU Usage    Memory Usage (MB)" << std::endl;
    for (const auto& p : processes) {
        std::cout << p.pid << "    " << p.name << "       " << p.cpu_usage << "        " << p.memory_usage << std::endl;
    }
}

int main() {
    while (true) {
        // Display system info
        std::cout << "System Monitor Tool" << std::endl;
        std::cout << "-------------------" << std::endl;
        get_cpu_info();
        get_memory_info();

        // Get process list and display it
        std::vector<ProcessInfo> processes = get_processes();

        // Sort and display processes
        display_processes(processes, true); // Sort by CPU usage

        // Allow user to kill a process
        std::cout << "\nEnter PID to kill or 0 to skip: ";
        int pid_to_kill;
        std::cin >> pid_to_kill;
        if (pid_to_kill != 0) {
            kill_process(pid_to_kill);
        }

        // Wait before refreshing
        std::cout << "\nRefreshing in 2 seconds...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Refresh every 2 seconds
        system("clear"); // Clear the console screen (Linux/Unix)
    }

    return 0;
}

