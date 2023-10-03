#include <iostream>
#include <exception>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <io/Input/Params/ConfigParser.h>
#include <io/Input/Params/ScriptRunnerParser.h>
#include <io/Output/cluster/WriteSBatch.h>

class RunHandler
{
    private:
        Logger& logger_;
        ModelBase getParams();
    public:
        RunHandler(Logger& logger);
        ~RunHandler();
        void HandleClusterRun(std::string script, std::string outputDir, std::string sqlConnectionString, ClusterRunParams clusterRunParams);
};
