#include <io/Output/cluster/WriteSBatch.h>

using namespace std;

WriteSBatch::WriteSBatch(/* args */){}
WriteSBatch::~WriteSBatch(){}

void WriteSBatch::WriteTmp(string submissionScript, string baseDir, string logDir, string scanScriptPrefix, string sqlConnectionString, ClusterRunParams clusterParams){
    ofstream submitScript;
    submitScript.open("tmp/" + submissionScript);
    submitScript << "#!/bin/bash\n#\n";

    handleClusterParams(submitScript, clusterParams);

    submitScript << "#\n###########################################\n\n";
    submitScript << "module load parallel\n";
    submitScript << "module load Boost\n";
//    submitScript << "module load SQLite\n";
    submitScript << "srun=\"srun --exclusive -N1 -n1 & \"\n";
    submitScript << "parallel=\"parallel --delay 0.2 -j $SLURM_NTASKS --joblog " + baseDir + "runtask.log --resume\"\n";
    submitScript << "$parallel \"$srun ./bin/clusterboltz ";
    submitScript << "tmp/" + scanScriptPrefix + "{1}.dat ";
    submitScript << sqlConnectionString + " ";
    submitScript << baseDir + " > " + baseDir + logDir + "clusterstream.{1}\" ::: {0.." + to_string(stoi(clusterParams.Nodes)*stoi(clusterParams.TaskPerNode)-1) + "}\n";
    submitScript.close();
}

void WriteSBatch::handleClusterParams(ofstream& fileStream, ClusterRunParams params){
    if (!params.Email.empty()){
        fileStream << sbatchLine("mail-user", params.Email);
        fileStream << sbatchLine("mail-type", "ALL");
    }
    fileStream << sbatchLine("time", params.TimeLimit);
    fileStream << sbatchLine("partition", params.Partition);
    fileStream << sbatchLine("ntasks", to_string(stoi(params.Nodes) * stoi(params.TaskPerNode)));
    fileStream << sbatchLine("nodes", params.Nodes);
    fileStream << sbatchLine("ntasks-per-node", params.TaskPerNode);
    fileStream << sbatchLine("cpus-per-task", params.CpusPerTask);
    fileStream << sbatchLine("mem-per-cpu", params.MemoryPerCpu);
    fileStream << sbatchLine("job-name", "IsaBoltz");
}

string WriteSBatch::sbatchLine(string cmd, string value){
    string line;
    if (!value.empty()){
        line = "#SBATCH --" + cmd + "=" + value + "\n";
    } 
    return line;
}
