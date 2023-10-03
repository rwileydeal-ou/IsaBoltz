#include <io/Input/Run/RunCardParser.h>

using namespace std;

RunCardParser::RunCardParser(Logger& logger) : 
    FileParser(),
    logger_(logger)
{

}
RunCardParser::~RunCardParser(){}

string RunCardParser::parseString(string line){
    string inString = FileIO::Split(line,"=#!")[1];
    inString.erase(remove(inString.begin(), inString.end(), '\t'), inString.end());
    inString.erase(remove(inString.begin(), inString.end(), ' '), inString.end());
    return inString;
}

LoggingLevel RunCardParser::parseLogLevel(string line){
    string inString = FileIO::Split(line,"=#!")[1];
    inString.erase(remove(inString.begin(), inString.end(), '\t'), inString.end());
    inString.erase(remove(inString.begin(), inString.end(), ' '), inString.end());
    boost::to_lower(inString);
    if (inString == "debug"){
        return LoggingLevel::DEBUG;
    } else if (inString == "info"){
        return LoggingLevel::INFO;
    } else if (inString == "notice"){
        return LoggingLevel::NOTICE;
    } else if (inString == "warn"){
        return LoggingLevel::WARN;
    } else if (inString == "error"){
        return LoggingLevel::ERROR;
    } else if (inString == "critical"){
        return LoggingLevel::CRITICAL;
    } else if (inString == "fatal"){
        return LoggingLevel::FATAL;
    } else {
        throw logic_error("Could not understand Logging Level!");
    }
}

RunParams RunCardParser::Convert(string infile){
    FileIO::ValidateFilePath(infile, logger_);
    RunParams runParams;
    map<string, bool&> runOptsBool;
    runOptsBool.insert(pair<string, bool&> ("Cluster", runParams.ClusterRun.Cluster));
    runOptsBool.insert(pair<string, bool&> ("CombineFiles", runParams.CombineRunOutputs));

    map<string, string&> runOptsStr;
    runOptsStr.insert(pair<string, string&> ("CpusPerTask", runParams.ClusterRun.CpusPerTask));
    runOptsStr.insert(pair<string, string&> ("MemoryPerCpu", runParams.ClusterRun.MemoryPerCpu));
    runOptsStr.insert(pair<string, string&> ("TasksPerNode", runParams.ClusterRun.TaskPerNode));
    runOptsStr.insert(pair<string, string&> ("Nodes", runParams.ClusterRun.Nodes));
    runOptsStr.insert(pair<string, string&> ("Partition", runParams.ClusterRun.Partition));
    runOptsStr.insert(pair<string, string&> ("TimeLimit", runParams.ClusterRun.TimeLimit));
    runOptsStr.insert(pair<string, string&> ("Email", runParams.ClusterRun.Email));
    
    ifstream file;
    file.open(infile);
    string line;
    while(getline(file, line)){
        string key = FileIO::Split(line, " ")[0];
        if (key[0] != '#' && key[0] != ' ' && key[0] != '\0'){
            auto posStr = runOptsStr.find(key);
            auto posBool = runOptsBool.find(key);
            
            if (posStr != runOptsStr.end()){
                posStr->second = parseString(line);
            } else if (posBool != runOptsBool.end()){
                posBool->second = FileParser::ParseBool(line);;
            } else if (key == "LogLevel"){
                logger_.SetPriority( parseLogLevel(line) );
            } else{
                logger_.Warn("Could not find run key=" + key);
            }
        }
    }
    return runParams;
}

void RunCardParser::Parse(string infile){
    try{
        logger_.Info("Parsing run_card: " + infile);
        Run = Convert(infile);
    }
    catch(FileNotFoundException& e){
        logger_.Error("Could not find run_card: " + infile);
    }
}
