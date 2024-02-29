# Project2-4811

## Overview

This C program demonstrates a multithreaded producer-consumer scenario in C, using POSIX threads. The program manages the interaction between producer and consumer threads to showcase thread synchronization and communication in a concurrent environment.

## Instructions

### 1. Compiling

Place each file in one folder and compile the program using the provided makefile:


### 2. Command-Line Inputs

The executable takes three command-line inputs:


./prog2 <buffer_size> <num_producers> <num_consumers>


- `<buffer_size>`: Size of the buffer.
- `<num_producers>`: Number of producer threads.
- `<num_consumers>`: Number of consumer threads.

Example:
./prog2 10 5 3

### 3. Execution

The program creates and manages multiple producer and consumer threads, simulating a producer-consumer scenario. The threads collaborate to produce and consume values from a shared buffer. The main function prints messages as specified.

### 4. Message Types

#### Producer Threads:

1. Immediately after entering the thread, print the number of values to be produced.
2. As a value is added to the buffer, print the value and position in which it is added.
3. If the buffer is full, print one message when the thread blocks and another when it's opened up and allowed to continue executing.
4. Just before exiting the thread, print a message indicating the thread is finished.

#### Consumer Threads:

1. Immediately after entering the thread, print the number of values to be consumed.
2. As a value is removed, print the value and position from which it is removed.
3. If the buffer is empty, print one message when the thread blocks and another when it's opened up and allowed to continue executing.
4. Just before exiting the thread, print a message indicating the thread is finished.
