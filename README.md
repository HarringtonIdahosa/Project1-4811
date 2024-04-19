# Project4-4811

## Overview
The provided code implements a paged memory manager simulation that utilizes pthreads for simulating processes accessing memory simultaneously. Each process has its own virtual memory space, and memory management is handled using the clock algorithm. The program reads input from files, performs memory operations, and outputs the results to another file. 

## Instructions
1. Compiling
    - Place each file in one folder, you could either run the Makefile in the temrinal or compile each file individually.

2. Run Command:
  `./main` `inputfile` `outputfile` `RNGseed`

3. Execution:
The program prints all output to the specified output file. Output includes process numbers, details of memory operations, address translations, and page eviction information. Addresses and values are printed in hexadecimal format.
