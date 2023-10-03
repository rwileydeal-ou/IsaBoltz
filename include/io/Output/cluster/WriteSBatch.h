#include <iostream>
#include <fstream>
#include <io/Input/Run/RunParams.h>

class WriteSBatch
{
private:
    static void handleClusterParams(std::ofstream& fileStream, ClusterRunParams params);
    static std::string sbatchLine(std::string cmd, std::string value);
public:
    WriteSBatch(/* args */);
    ~WriteSBatch();
    static void WriteTmp(std::string submissionScript, std::string baseDir, std::string logDir, std::string scanScriptPrefix, std::string sqlConnectionString, ClusterRunParams clusterParams);
};
