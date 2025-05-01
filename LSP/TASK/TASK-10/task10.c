
 //Write a C program that demonstrates process creation and management:
 //Use fork() to create a child process.
 //In the child process, execute an external program (e.g., ls command) using execvp().
 //In the parent process, wait for the child process to complete and print a message indicating the child’s termination.
 
 
 #include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork(); // Create a child process

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child process
        printf("Child process: Executing the 'ls' command\n");

        // Arguments for execvp (command and its arguments)
        char *args[] = {"ls", "-l", NULL};  // List files in long format
        execvp(args[0], args);  // Execute the command

        // execvp() only returns if there's an error
        perror("Execvp failed");
        return 1;
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);  // Wait for the child process to terminate

        if (WIFEXITED(status)) {
            printf("Parent process: Child terminated successfully with exit status %d\n", WEXITSTATUS(status));
        } else {
            printf("Parent process: Child terminated abnormally\n");
        }
    }

    return 0;
}

