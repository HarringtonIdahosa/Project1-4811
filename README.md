# Project1-4811

## Overview

This C program handles the concurrent execution of multiple test cases using forked processes. Each child process runs a specific test case, and the parent process waits for their completion.

## Instructions
1. Test Cases
    -First place each test case into one file and complile them.
2. Directory Input:
   - Upon running parent-child.c the program prompts the user to copy and paste the directory containing test cases.
   - Example: "Copy and paste the directory of the test cases: /path/to/test_cases"

3. Number of Child Processes:
   - The user specifies the number of child processes to create, capped at a maximum of 25.

4. Execution:
   - The program generates child processes, each executing a test case based on its index.
   - The parent process then waits for all child processes to finish.

