#include <proc/RunHandler.h>

using namespace std;
using namespace boost::filesystem;

RunHandler::RunHandler(Logger& logger) : logger_(logger){}
RunHandler::~RunHandler(){}

ModelBase RunHandler::getParams(){
    ModelBase inputs;

    const char* configCard = (current_path() /= "cards/config/particle.config").c_str();
    ConfigParser configFile(logger_);
    configFile.Parse(string(configCard), inputs);

    // Parse param file to get raw data and create base model
    ParamCardParser paramFile(logger_);
    const char* paramCard = (current_path() /= "cards/param_card.dat").c_str();
    paramFile.Parse(string(paramCard), inputs);
    return inputs;
}

void RunHandler::HandleClusterRun(string script, string outputDir, string sqlConnectionString, ClusterRunParams clusterRunParams){
    logger_.Info("Running in cluster mode...");
    string logDir = "logs/";
    FileIO::CreateDirectory(outputDir + logDir, logger_);

    // build commands, using a uniquifier to guarantee can run multiple simultaneous scans
    srand(time(0));
    string uniquifier = to_string(rand());
    string submissionScript = uniquifier + "_submit.sbatch";
    string scanScript = uniquifier + "_scan_";
    string tmpDir = "tmp/";
    string launchCmd = "sbatch " + tmpDir + submissionScript;

    // handle dividing up scans
    ScriptRunnerParser s(logger_);
    s.DivideScriptRuns(script, clusterRunParams, tmpDir, scanScript);
    // write the submission script
    WriteSBatch::WriteTmp(submissionScript, outputDir, logDir, scanScript, sqlConnectionString, clusterRunParams);
    // now submit sbatch script
    system(launchCmd.c_str());
}
