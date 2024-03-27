////sched_sim.cpp
#include "sched_sim.h"

extern int snapshotInterval;
extern std::FILE *outputFile;

#pragma region ProcessSet functions

ProcessSet::ProcessSet(ProcessInfo *processInfo, int processCount)// Process constructor
{
    this->processCount = processCount;
    this->processInfo = new ProcessInfo[processCount];
    memcpy(this->processInfo, processInfo, processCount * sizeof(struct ProcessInfo));
}

// Return Statments
int ProcessSet::gettotalCpuTime() 
{
    int returnVal = 0;
    for (int j = 0; j < processCount; j++)
        returnVal += processInfo[j].burstTime;
    return returnVal;
}


double ProcessSet::getAverageWaitTime()
{
    double returnVal = 0;
    for (int j = 0; j < processCount; j++)
    {
        returnVal += processInfo[j].waitTime;
    }
    return returnVal / processCount;
}


double ProcessSet::getAverageTotalTime()
{
    double returnVal = 0;
    for (int j = 0; j < processCount; j++)
    {
        returnVal += processInfo[j].totalTime;
    }
    return returnVal / processCount;
}

double ProcessSet::getAverageTurnaroundTime()
{
    double returnVal = 0;
    for (int j = 0; j < processCount; j++)
    {
        returnVal += processInfo[j].totalTime;
    }
    return returnVal / processCount;
}

#pragma endregion
#pragma region Processor functions

Processor::Processor(int totalCpuTime)// Processor class constructor
{
    this->totalCpuTime = totalCpuTime;
}

// Runs the current process and returns true if the process has completed
bool Processor::RunProcess()
{
    runningProcess->burstRemaining--;
    runningProcess->totalTime++;
    runningProcess->hasRan = true;
    runningProcess->Done = (runningProcess->burstRemaining == 0);
    runningProcess->LastSwitch++;
    
    //Prevent duplicates
    if (processorder.empty() || processorder.back()->PID != runningProcess->PID)
        processorder.push_back(runningProcess);
    return runningProcess->Done;
}

// Prints the ready queue
void Processor::PrintReadyQueue()
{
    fprintf(outputFile, "\nReady Queue: ");
    for (int j = 0; j < readyQueue.size(); j++)
    {
        fprintf(outputFile, "%d", readyQueue.at(j)->PID);
        if (j != readyQueue.size() - 1)
            fprintf(outputFile, "-");
    }
    if (readyQueue.empty())
        fprintf(outputFile, "empty");
    fprintf(outputFile, "\n\n");
}

// Print summary
void Processor::PrintSummary(ProcessSet *Process_Set)
{
    fprintf(outputFile, "*********************************************************\nFCFS Summary (WT = wait time, TT = turnaround time):\n\n");
    fprintf(outputFile, "%-6s%-6s%-6s\n", "PID", "WT", "TT");
    for (int j = 0; j < Process_Set->processCount; j++)
    {
        fprintf(outputFile, "%-6d%-6d%-6d\n", Process_Set->processInfo[j].PID, Process_Set->processInfo[j].waitTime, Process_Set->processInfo[j].totalTime);
    }
    fprintf(outputFile, "%-6s%-6.2lf%-6.2lf\n\n", "AVG", Process_Set->getAverageWaitTime(), Process_Set->getAverageTotalTime());
    fprintf(outputFile, "Process sequence: ");
    for (int j = 0; j < processorder.size(); j++)
    {
        fprintf(outputFile, "%d", processorder.at(j)->PID);
        if (j != processorder.size() - 1)
            fprintf(outputFile, "-");
    }
    fprintf(outputFile, "\nContext switches: %d\n\n\n", (int)processorder.size());
}

#pragma endregion

#pragma region Comparison functions

// Compare by priority
bool compareByPriority(const ProcessInfo *x1, const ProcessInfo *x2)
{
    return x1->priority < x2->priority;
}

bool compareByBurstThenPriority(const ProcessInfo *x1, const ProcessInfo *x2) // Compare burst and priority
{
    if (x1->burstTime != x2->burstTime)
        return x1->burstTime < x2->burstTime;
    else
        return compareByPriority(x1, x2);
}


bool compareByBurstRemainingThenPriority(const ProcessInfo *x1, const ProcessInfo *x2)
{
    if (x1->burstRemaining != x2->burstRemaining)
        return x1->burstTime < x2->burstTime;
    else
        return compareByPriority(x1, x2);
}

#pragma endregion


void FCFS(ProcessSet *Process_Set)// FCFS algorithm (Logic repeates)
{
    fprintf(outputFile, "***** FCFS Scheduling *****\n");

    Processor cpu(Process_Set->gettotalCpuTime());


    for (int cpuTime = 0; cpuTime < cpu.totalCpuTime + 1; cpuTime++)    // Cpu loop
    {
       
        bool Snapshot = (cpuTime == 0 || cpuTime % snapshotInterval == 0); // promts to print snapshot

        if (Snapshot)
            fprintf(outputFile, "t = %d\nCPU: ", cpuTime);

       
        if (cpu.loadProcess != nullptr) // while in the last cycle set as running process and remove from ready queue
        {
            cpu.runningProcess = cpu.loadProcess;
            cpu.readyQueue.pop_front();
            cpu.loadProcess = nullptr;
        }

        // Loop totalTime and waitTime for all processes in the queue
        for (int j = 0; j < cpu.readyQueue.size(); j++)
        {
            cpu.readyQueue.at(j)->totalTime++;
            cpu.readyQueue.at(j)->waitTime++;
        }

        // Checks which processes arrive this clock tick
        for (int j = 0; j < Process_Set->processCount; j++)
        {
            if (cpuTime == Process_Set->processInfo[j].arrivalTime)
            {
                Process_Set->processInfo[j].hasArrived = true;
                cpu.processesArrived.push_back(&Process_Set->processInfo[j]);
            }
        }

        // Sort the processes by priority
        std::sort(cpu.processesArrived.begin(), cpu.processesArrived.end(), &compareByPriority);

        // Put the processes in the queue
        for (int j = 0; j < cpu.processesArrived.size(); j++)
        {
           
            cpu.processesArrived.at(j)->PID = cpu.highestPID; // Assigns PID to the process
            cpu.highestPID++;

           
            cpu.readyQueue.push_back(cpu.processesArrived.at(j)); // Put process in the ready queue
        }
        
        cpu.processesArrived.clear();// Clears this cycle

        // If somethings in the ready to process, process it
        if (cpu.runningProcess != nullptr)
        {
            bool complete = cpu.RunProcess();
            if (Snapshot)
            {
                if (complete)
                    fprintf(outputFile, "Finishing process %d ", cpu.runningProcess->PID);
                else
                    fprintf(outputFile, "Running process %d (remaining CPU burst = %d) ", cpu.runningProcess->PID, cpu.runningProcess->burstRemaining);
            }
        }
        
// If running process is over or if there is no running process check queue
               if ((cpu.runningProcess == nullptr || cpu.runningProcess->Done) && !cpu.readyQueue.empty())
        {
            cpu.loadProcess = cpu.readyQueue.front();
            if (Snapshot)
                fprintf(outputFile, "Loading process %d (CPU burst = %d) ", cpu.loadProcess->PID, cpu.loadProcess->burstRemaining);
        }
        else if (cpu.runningProcess->Done)
        {
            cpu.runningProcess = nullptr;
        }

        // Print Queue
        if (Snapshot)
        {
            cpu.PrintReadyQueue();
        }
    }

    Process_Set->numContextSwitches = (int)cpu.processorder.size();
    cpu.PrintSummary(Process_Set);
}


void SJF(ProcessSet *Process_Set) // SJF algorithm
{
    fprintf(outputFile, "***** SJF Scheduling *****\n");

    Processor cpu(Process_Set->gettotalCpuTime());

    
    for (int cpuTime = 0; cpuTime < cpu.totalCpuTime + 1; cpuTime++)
    {
        
        bool Snapshot = (cpuTime == 0 || cpuTime % snapshotInterval == 0);

        if (Snapshot)
            fprintf(outputFile, "t = %d\nCPU: ", cpuTime);

        
        if (cpu.loadProcess != nullptr)
        {
            cpu.runningProcess = cpu.loadProcess;
            cpu.readyQueue.pop_front();
            cpu.loadProcess = nullptr;
        }

        
        for (int j = 0; j < cpu.readyQueue.size(); j++)
        {
            cpu.readyQueue.at(j)->totalTime++;
            cpu.readyQueue.at(j)->waitTime++;
        }

         for (int j = 0; j < Process_Set->processCount; j++)// Check which processes arrives 
        {
            if (cpuTime == Process_Set->processInfo[j].arrivalTime)
            {
                Process_Set->processInfo[j].hasArrived = true;
                cpu.processesArrived.push_back(&Process_Set->processInfo[j]);
            }
        }

       
        std::sort(cpu.processesArrived.begin(), cpu.processesArrived.end(), &compareByPriority); 
        
        for (int j = 0; j < cpu.processesArrived.size(); j++)
        {
           
            cpu.processesArrived.at(j)->PID = cpu.highestPID;
            cpu.highestPID++;

            
            cpu.readyQueue.push_back(cpu.processesArrived.at(j));
        }

        if (cpu.processesArrived.size() != 0) // If anything arrived sort the ready queue
        {
            std::sort(cpu.readyQueue.begin(), cpu.readyQueue.end(), &compareByBurstThenPriority);
        }

        
        cpu.processesArrived.clear();
        
        if (cpu.runningProcess != nullptr)
        {
            bool complete = cpu.RunProcess();
            if (Snapshot)
            {
                if (complete)
                    fprintf(outputFile, "Finishing process %d ", cpu.runningProcess->PID);
                else
                    fprintf(outputFile, "Running process %d (remaining CPU burst = %d) ", cpu.runningProcess->PID, cpu.runningProcess->burstRemaining);
            }
        }

  
        if ((cpu.runningProcess == nullptr || cpu.runningProcess->Done) && !cpu.readyQueue.empty())
        {
            cpu.loadProcess = cpu.readyQueue.front();
            if (Snapshot)
                fprintf(outputFile, "Loading process %d (CPU burst = %d) ", cpu.loadProcess->PID, cpu.loadProcess->burstRemaining);
        }
        else if (cpu.runningProcess->Done)
        {
            cpu.runningProcess = nullptr;
        }

        
        if (Snapshot)
        {
            cpu.PrintReadyQueue();// Print Queue
        }
    }

    Process_Set->numContextSwitches = (int)cpu.processorder.size();
    cpu.PrintSummary(Process_Set);
}


void STCF(ProcessSet *Process_Set)// STCF algorithm
{
    fprintf(outputFile, "***** STCF Scheduling *****\n");

    Processor cpu(Process_Set->gettotalCpuTime());

    
    for (int cpuTime = 0; cpuTime < cpu.totalCpuTime + 1; cpuTime++)
    {
       
        bool Snapshot = (cpuTime == 0 || cpuTime % snapshotInterval == 0);

        if (Snapshot)
            fprintf(outputFile, "t = %d\nCPU: ", cpuTime);

       
        if (cpu.loadProcess != nullptr)
        {
            cpu.runningProcess = cpu.loadProcess;
            cpu.readyQueue.pop_front();
            cpu.loadProcess = nullptr;
        }

        
        for (int j = 0; j < cpu.readyQueue.size(); j++)
        {
            cpu.readyQueue.at(j)->totalTime++;
            cpu.readyQueue.at(j)->waitTime++;
        }

       
        for (int j = 0; j < Process_Set->processCount; j++)
        {
            if (cpuTime == Process_Set->processInfo[j].arrivalTime)
            {
                Process_Set->processInfo[j].hasArrived = true;
                cpu.processesArrived.push_back(&Process_Set->processInfo[j]);
            }
        }

        std::sort(cpu.processesArrived.begin(), cpu.processesArrived.end(), &compareByPriority);

        for (int j = 0; j < cpu.processesArrived.size(); j++)
        {
           
            cpu.processesArrived.at(j)->PID = cpu.highestPID;
            cpu.highestPID++;

            
            cpu.readyQueue.push_back(cpu.processesArrived.at(j));
        }

        
        std::sort(cpu.readyQueue.begin(), cpu.readyQueue.end(), &compareByBurstThenPriority);

      
        cpu.processesArrived.clear();

        
        if (cpu.runningProcess != nullptr)
        {
            bool complete = cpu.RunProcess();

            // Calculates if the process should be preempted
            bool shouldPreempt = false;
            if (!cpu.readyQueue.empty())
            {
                if (cpu.readyQueue.front()->burstRemaining < cpu.runningProcess->burstRemaining)
                {
                    shouldPreempt = true;
                }
                else if (cpu.readyQueue.front()->burstRemaining == cpu.runningProcess->burstRemaining && cpu.readyQueue.front()->priority < cpu.runningProcess->priority)
                {
                    shouldPreempt = true;
                }
            }

            if (shouldPreempt)
            {
                if (Snapshot)
                    fprintf(outputFile, "Preempting process %d (remaining CPU burst = %d) ", cpu.runningProcess->PID, cpu.runningProcess->burstRemaining);
                cpu.readyQueue.push_back(cpu.runningProcess);
                cpu.runningProcess = cpu.readyQueue.front();
                cpu.readyQueue.pop_front();
                if (Snapshot)
                    fprintf(outputFile, "Loading process %d (CPU burst = %d) ", cpu.runningProcess->PID, cpu.runningProcess->burstRemaining);
            }
            else if (Snapshot)
            {
                if (complete)
                    fprintf(outputFile, "Finishing process %d ", cpu.runningProcess->PID);
                else
                    fprintf(outputFile, "Running process %d (remaining CPU burst = %d) ", cpu.runningProcess->PID, cpu.runningProcess->burstRemaining);
            }
        }

      
        if ((cpu.runningProcess == nullptr || cpu.runningProcess->Done) && !cpu.readyQueue.empty())
        {
            cpu.loadProcess = cpu.readyQueue.front();
        }
        else if (cpu.runningProcess->Done)
        {
            cpu.runningProcess = nullptr;
        }

        if (cpu.loadProcess && Snapshot)
        {
            fprintf(outputFile, "Loading process %d (CPU burst = %d) ", cpu.loadProcess->PID, cpu.loadProcess->burstRemaining);
        }

        
        if (Snapshot)
        {
            cpu.PrintReadyQueue();// Print Queue
        }
    }

    Process_Set->numContextSwitches = (int)cpu.processorder.size();
    cpu.PrintSummary(Process_Set);
}


void RoundRobin(ProcessSet *Process_Set)// Round robin algorithm
{
    fprintf(outputFile, "***** Round robin Scheduling *****\n");

    Processor cpu(Process_Set->gettotalCpuTime());

    // Cpu loop
    for (int cpuTime = 0; cpuTime < cpu.totalCpuTime + 1; cpuTime++)
    {
       
        bool Snapshot = (cpuTime == 0 || cpuTime % snapshotInterval == 0);

        if (Snapshot)
            fprintf(outputFile, "t = %d\nCPU: ", cpuTime);


        if (cpu.loadProcess != nullptr)
        {
            cpu.runningProcess = cpu.loadProcess;
            cpu.loadProcess = nullptr;
            cpu.readyQueue.pop_front();
        }

       
        for (int j = 0; j < cpu.readyQueue.size(); j++)
        {
            cpu.readyQueue.at(j)->totalTime++;
            cpu.readyQueue.at(j)->waitTime++;
        }

        
        for (int j = 0; j < Process_Set->processCount; j++)
        {
            if (cpuTime == Process_Set->processInfo[j].arrivalTime)
            {
                Process_Set->processInfo[j].hasArrived = true;
                cpu.processesArrived.push_back(&Process_Set->processInfo[j]);
            }
        }

        
        std::sort(cpu.processesArrived.begin(), cpu.processesArrived.end(), &compareByPriority);

        
        for (int j = 0; j < cpu.processesArrived.size(); j++)
        {
      
            cpu.processesArrived.at(j)->PID = cpu.highestPID;
            cpu.highestPID++;

            
            cpu.readyQueue.push_back(cpu.processesArrived.at(j));
        }

        
        cpu.processesArrived.clear();

        if (cpu.runningProcess != nullptr)
        {
            bool complete = cpu.RunProcess();

            if (cpu.runningProcess->LastSwitch == 2 && cpu.runningProcess->burstRemaining != 0)
            {
                cpu.runningProcess->LastSwitch = 0;
                if (Snapshot)
                    fprintf(outputFile, "Preempting process %d (remaining CPU burst = %d) ", cpu.runningProcess->PID, cpu.runningProcess->burstRemaining);
                cpu.readyQueue.push_back(cpu.runningProcess);
                cpu.runningProcess = nullptr;
            }
            else if (Snapshot)
            {
                if (complete)
                    fprintf(outputFile, "Finishing process %d ", cpu.runningProcess->PID);
                else
                    fprintf(outputFile, "Running process %d (remaining CPU burst = %d) ", cpu.runningProcess->PID, cpu.runningProcess->burstRemaining);
            }
        }

      
        if ((cpu.runningProcess == nullptr || cpu.runningProcess->Done) && !cpu.readyQueue.empty())
        {
            cpu.loadProcess = cpu.readyQueue.front();
        }
        else if (cpu.runningProcess != nullptr && cpu.runningProcess->Done)
        {
            cpu.runningProcess = nullptr;
        }

        if (cpu.loadProcess != nullptr && Snapshot)
        {
            fprintf(outputFile, "Loading process %d (CPU burst = %d) ", cpu.loadProcess->PID, cpu.loadProcess->burstRemaining);
        }

        // Print Queue
        if (Snapshot)
        {
            cpu.PrintReadyQueue();
        }
    }

    Process_Set->numContextSwitches = (int)cpu.processorder.size();
    cpu.PrintSummary(Process_Set);
}


void Priority(ProcessSet *Process_Set) // Non preemptive priority  algorithm
{
    fprintf(outputFile, "***** Priority Scheduling *****\n");

    Processor cpu(Process_Set->gettotalCpuTime());

    
    for (int cpuTime = 0; cpuTime < cpu.totalCpuTime + 1; cpuTime++)
    {
        
        bool Snapshot = (cpuTime == 0 || cpuTime % snapshotInterval == 0);

        if (Snapshot)
            fprintf(outputFile, "t = %d\nCPU: ", cpuTime);

        
        if (cpu.loadProcess != nullptr)
        {
            cpu.runningProcess = cpu.loadProcess;
            cpu.readyQueue.pop_front();
            cpu.loadProcess = nullptr;
        }

       
        for (int j = 0; j < cpu.readyQueue.size(); j++)
        {
            cpu.readyQueue.at(j)->totalTime++;
            cpu.readyQueue.at(j)->waitTime++;
        }

        
        for (int j = 0; j < Process_Set->processCount; j++)
        {
            if (cpuTime == Process_Set->processInfo[j].arrivalTime)
            {
                Process_Set->processInfo[j].hasArrived = true;
                cpu.processesArrived.push_back(&Process_Set->processInfo[j]);
            }
        }

        std::sort(cpu.processesArrived.begin(), cpu.processesArrived.end(), &compareByPriority);

        for (int j = 0; j < cpu.processesArrived.size(); j++)
        {
            
            cpu.processesArrived.at(j)->PID = cpu.highestPID;
            cpu.highestPID++;

            cpu.readyQueue.push_back(cpu.processesArrived.at(j));
        }

        
        if (cpu.processesArrived.size() != 0)
        {
            std::sort(cpu.readyQueue.begin(), cpu.readyQueue.end(), &compareByPriority);
        }

        
        cpu.processesArrived.clear();

       
        if (cpu.runningProcess != nullptr)
        {
            bool complete = cpu.RunProcess();
            if (Snapshot)
            {
                if (complete)
                    fprintf(outputFile, "Finishing process %d ", cpu.runningProcess->PID);
                else
                    fprintf(outputFile, "Running process %d (remaining CPU burst = %d) ", cpu.runningProcess->PID, cpu.runningProcess->burstRemaining);
            }
        }

       
        if ((cpu.runningProcess == nullptr || cpu.runningProcess->Done) && !cpu.readyQueue.empty())
        {
            cpu.loadProcess = cpu.readyQueue.front();
            if (Snapshot)
                fprintf(outputFile, "Loading process %d (CPU burst = %d) ", cpu.loadProcess->PID, cpu.loadProcess->burstRemaining);
        }
        else if (cpu.runningProcess->Done)
        {
            cpu.runningProcess = nullptr;
        }

         if (Snapshot)
        {
            cpu.PrintReadyQueue(); // Print Queue
        }
    }

    Process_Set->numContextSwitches = (int)cpu.processorder.size();
    cpu.PrintSummary(Process_Set);
}