//main.cpp	

#include <iostream>
#include <fstream>
#include <string.h>
#include "sched_sim.h"

int snapshotInterval;
std::FILE *outputFile;

// BufferSize, NumProducers, and NumConsumers
int main(int argc, char **argv)
{
    char *inputTxt = argv[1];
    char *outputTxt = argv[2];
    snapshotInterval = std::atoi(argv[3]);

    ProcessInfo *processInfo;

    std::ifstream inputFileStream;// Open File 
    inputFileStream.open(inputTxt);

    
    // Determine line count
    std::string tempString;
    int tempInt;
    int lineCount = 0;

    while (getline(inputFileStream, tempString, '\n'))
    {
        lineCount++;
    }
    // Return to top of file
    inputFileStream.clear();
    inputFileStream.seekg(0, inputFileStream.beg);

    // Read in data
    processInfo = new ProcessInfo[lineCount];
    for (int j = 0; j < lineCount; j++)
    {
        inputFileStream >> processInfo[j].burstTime;
        inputFileStream >> processInfo[j].priority;
        inputFileStream >> processInfo[j].arrivalTime;

        processInfo[j].burstRemaining = processInfo[j].burstTime;
    }
   inputFileStream.close();

    //memcpy the processInfo pointer so each algorythm starts with its own copy
    ProcessSet FCFSprocessSet(processInfo, lineCount);
    ProcessSet SJFprocessSet(processInfo, lineCount);
    ProcessSet STCprocessSet(processInfo, lineCount);
    ProcessSet RRprocessSet(processInfo, lineCount);
    ProcessSet NPPprocessSet(processInfo, lineCount);

    // Open file to write
    outputFile = fopen(outputTxt, "w");

    FCFS(&FCFSprocessSet);
    SJF(&SJFprocessSet);
    STCF(&STCprocessSet);
    RoundRobin(&RRprocessSet);
    Priority(&NPPprocessSet);

    //Overall Summary
    std::vector<std::pair<double, char *>> TurnaroundTimes;
    std::vector<std::pair<double, char *>> WaitTimes;
    std::vector<std::pair<int, char *>> ContextSwitches;

    TurnaroundTimes.push_back(std::make_pair(FCFSprocessSet.getAverageTotalTime(), (char *)"FCFS"));
    WaitTimes.push_back(std::make_pair(FCFSprocessSet.getAverageWaitTime(), (char *)"FCFS"));
    ContextSwitches.push_back(std::make_pair(FCFSprocessSet.numContextSwitches, (char *)"FCFS"));

    TurnaroundTimes.push_back(std::make_pair(SJFprocessSet.getAverageTotalTime(), (char *)"SJF"));
    WaitTimes.push_back(std::make_pair(SJFprocessSet.getAverageWaitTime(), (char *)"SJF"));
    ContextSwitches.push_back(std::make_pair(SJFprocessSet.numContextSwitches, (char *)"SJF"));

    TurnaroundTimes.push_back(std::make_pair(STCprocessSet.getAverageTotalTime(), (char *)"STCF"));
    WaitTimes.push_back(std::make_pair(STCprocessSet.getAverageWaitTime(), (char *)"STCF"));
    ContextSwitches.push_back(std::make_pair(STCprocessSet.numContextSwitches, (char *)"STCF"));

    TurnaroundTimes.push_back(std::make_pair(RRprocessSet.getAverageTotalTime(), (char *)"Round Robin"));
    WaitTimes.push_back(std::make_pair(RRprocessSet.getAverageWaitTime(), (char *)"Round Robin"));
    ContextSwitches.push_back(std::make_pair(RRprocessSet.numContextSwitches, (char *)"Round Robin"));

    TurnaroundTimes.push_back(std::make_pair(NPPprocessSet.getAverageTotalTime(), (char *)"Priority"));
    WaitTimes.push_back(std::make_pair(NPPprocessSet.getAverageWaitTime(), (char *)"Priority"));
    ContextSwitches.push_back(std::make_pair(NPPprocessSet.numContextSwitches, (char *)"Priority"));
    
    std::sort(TurnaroundTimes.begin(), TurnaroundTimes.end());
    std::sort(WaitTimes.begin(), WaitTimes.end());
    std::sort(ContextSwitches.begin(), ContextSwitches.end());

    fprintf(outputFile, "***** OVERALL SUMMARY *****\n\n");
    fprintf(outputFile, "Wait Time Comparison\n");
    for (int j = 0; j < WaitTimes.size(); j++)
    {
        fprintf(outputFile, "%-2d%-14s%.2lf\n", j + 1, WaitTimes[j].second, WaitTimes[j].first);
    }
    fprintf(outputFile, "\nTurnaround Time Comparison\n");
    for (int j = 0; j < TurnaroundTimes.size(); j++)
    {
        fprintf(outputFile, "%-2d%-14s%.2lf\n", j + 1, TurnaroundTimes[j].second, TurnaroundTimes[j].first);
    }
    fprintf(outputFile, "\nContext Switch Comparison\n");
    for (int j = 0; j < ContextSwitches.size(); j++)
    {
        fprintf(outputFile, "%-2d%-14s%d\n", j + 1, ContextSwitches[j].second, ContextSwitches[j].first);
    }
    fclose(outputFile);
}