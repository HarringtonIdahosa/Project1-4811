# Project4-4811

## Overview

This C program manages the execution of multiple test cases using forked processes. Each child process is responsible for running a specific test case, while the parent process waits for their completion.

## Instructions

1. Compiling
    - Place each file in one folder, you could either run the Makefile in the temrinal or compile each file individually.

2. Number of Child Processes:
   - The user specifies the number of child processes to create with a max of 25, using the command line ./(Compiled Executable's name)(# of procecess). 
   - Example: ./output 9

3. Execution:
   - The program generates child processes, each executing a test case based on its index.
   - The parent process then waits for all child processes to finish.
