#!/bin/bash

# Variables
word="Santanu"          # Word to search for
file="file.txt"       # File to search in

# Check if file exists
if [ ! -f "$file" ]; then
    echo "File '$file' not found!"
    exit 1
fi

# Find the word using grep and sort the results
grep "$word" "$file" | sort

