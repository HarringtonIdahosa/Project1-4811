//sched_sim.h
#ifndef SCHED_SIM
#define SCHED_SIM
#include <stdio.h>
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>

struct ProcessInfo
{
    
    bool hasArrived = 0;// algorithm
    bool Done = 0;
    bool Running = 0;
    bool hasRan = 0; ;
    
    int burstTime;// file
    int priority;
    int arrivalTime;
    int PID;
    int waitTime = 0;
    int totalTime = 0;

   
    int burstRemaining; // file then algorithm
    int LastSwitch = 0;

    
};

class ProcessSet
{
public:
    int processCount;
    int numContextSwitches = 0;
    ProcessInfo *processInfo;

    ProcessSet(ProcessInfo *processInfo, int processCount);
    
    int gettotalCpuTime();
    
    double getAverageTotalTime();
    double getAverageWaitTime();
    double getAverageTurnaroundTime();
};

class Processor
{
public:
   
    int totalCpuTime; // Stores the total CPUtime

   std::deque<ProcessInfo *> readyQueue; // track cpu's ready queue

     std::vector<ProcessInfo *> processesArrived;// track of the process which arrive during the cpu cycle

    
    int highestPID = 0;// track of which PIDs are used

   
    ProcessInfo *runningProcess = nullptr; // pointer to the currently running process

     ProcessInfo *loadProcess = nullptr;// pointer to the currently loading process

   // Stores the order of processes
    std::vector<ProcessInfo *> processorder; 

    Processor(int totalCpuTime);
    bool RunProcess();
    void PrintReadyQueue();
    void PrintSummary(ProcessSet *process_Set);
};

void FCFS(ProcessSet *Process_Set);
void SJF(ProcessSet *Process_Set);
void STCF(ProcessSet *Process_Set);
void RoundRobin(ProcessSet *Process_Set);
void Priority(ProcessSet *Process_Set);

// comparison function
bool compareByBurstThenPriority(const ProcessInfo *x1, const ProcessInfo *x2);

bool compareByPriority(const ProcessInfo *x1, const ProcessInfo *x2);

#endif