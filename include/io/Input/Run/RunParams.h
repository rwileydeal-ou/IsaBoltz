#ifndef RunParams_h
#define RunParams_h

#include <iostream>
#include <io/Logging/LoggingLevel.h>

enum JobScheduler{
    Slurm
};

struct ClusterRunParams{
    bool Cluster;
    JobScheduler Scheduler;
    std::string Nodes;
    std::string TaskPerNode;
    std::string CpusPerTask;
    std::string Email;
    std::string MemoryPerCpu;
    std::string Partition;
    std::string TimeLimit;
};

struct RunParams{
    ClusterRunParams ClusterRun;
    bool CombineRunOutputs;
};

#endif
