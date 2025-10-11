#include <client/ScriptClient.h>

using namespace std;

ScriptClient::ScriptClient(string outputPath, Logger& logger) :
    logger_(logger)
{
    outputPath_ = outputPath;
}

ScriptClient::~ScriptClient(){
}

vector< vector< CommandWithPayload > > ScriptClient::separateMacros( vector< CommandWithPayload > allMacros ){
    vector< vector< CommandWithPayload > > dividedMacros;
    vector< CommandWithPayload > singleMacro;

    for (auto& macro : allMacros){
        singleMacro.push_back( macro );
        if (macro.Command == SupportedCommands::LAUNCH){
            dividedMacros.push_back(singleMacro);
            singleMacro.clear();
        }
    }

    return dividedMacros;
}

vector< CommandWithPayload > ScriptClient::handleScan(string line, vector< CommandWithPayload > baseCmds){
    vector< CommandWithPayload > scanCommands;

    try{
        ScriptRunnerParser s(logger_);
        ScanDetails endpoints = s.setScanDetails(line);

        for (int i=0; i<endpoints.Steps; ++i){
            // base cmds are already attached to first macro object
            if (i != 0){
                scanCommands.insert( scanCommands.end(), baseCmds.begin(), baseCmds.end() );
            }

            for (auto& endpoint : endpoints.ScanParams){
               std::stringstream ss;
               ss << endpoint.ScanValues[i];
               string scanLine = endpoint.ScanKey + "=" + ss.str();
                scanCommands.push_back( 
                    CommandWithPayload(SupportedCommands::PARAMETER_OVERRIDE, scanLine) 
                );
            }
            // now insert a "launch" to separate each scan point, except for the last one (since script should have "launch" after scan, to avoid confusion)
            // this is not ideal behavior (TODO: revise and allow for cmds after the "scan" keyword), but implementation will take a while, so pushing to later...
            if (i < endpoints.Steps - 1 ){
                scanCommands.push_back( 
                    CommandWithPayload(SupportedCommands::LAUNCH, "") 
                );
            }
        }
    }
    catch (exception& e){
        logger_.Error("Could not understand scan syntax: " + line);
        logger_.Info("See manual for syntax...");
        throw_with_trace(e);
    }

    return scanCommands;
}

vector< CommandWithPayload > ScriptClient::scriptInput(std::string script, std::vector<std::string> supportedOptions){
    vector< CommandWithPayload > allCmds;
    vector< CommandWithPayload > macroContents;
    try{
        logger_.Info("Parsing script: " + script);

        FileIO::ValidateFilePath(script, logger_);
        ifstream fileStream;
        fileStream.open(script);
        string line;

        while(getline(fileStream, line)){
            vector<string> splitLine = FileIO::Split(line, " ");
            string cmd = splitLine[0];
            splitLine.erase(splitLine.begin());
            string payload = boost::algorithm::join(splitLine, "");

            // don't process line if commented or blank
            if (cmd.front() != '#' && cmd.front() != '\0'){
                if (cmd == "set"){
                    macroContents.push_back( 
                        CommandWithPayload(SupportedCommands::PARAMETER_OVERRIDE, payload) 
                    );
                } else if (cmd == "scan"){
                    auto scanCmds = handleScan(payload, macroContents);
                    macroContents.insert(macroContents.end(), scanCmds.begin(), scanCmds.end());
                } else if (cmd == "launch"){
                    macroContents.push_back( 
                        CommandWithPayload(SupportedCommands::LAUNCH, "") 
                    );

                    // now push back the full set of commands
                    allCmds.insert( allCmds.end(), macroContents.begin(), macroContents.end() );
                    // and start over
                    macroContents = vector< CommandWithPayload >();
                } else{
                    macroContents.push_back(
                        ProcessClient::getCmdFromString(cmd, payload)
                    );
                }
            }
        }

        return allCmds;
    }
    catch (FileNotFoundException& e){
        logger_.Error( "Could not find " + script );
        logger_.Warn("Continuing using the default params...");
    }
    return allCmds;
}

bool compareByEnum(const CommandWithPayload& cmd1, const CommandWithPayload& cmd2){
    return cmd1.Command > cmd2.Command;
}

void ScriptClient::HandleScript( string sqlConnectionString, vector< vector< CommandWithPayload > > setOfCmds, bool combineFiles ){
    boost::property_tree::ptree combinedResults;

    int i = 0;
    for(auto cmds = setOfCmds.begin(); cmds != setOfCmds.end(); ++cmds){
        // since we may have a ton of different scenarios, for now easier to just create a new processclient and dispose afterwards to sanitize the extensive number of parameters
        auto inputId = boost::uuids::random_generator()();
        std::shared_ptr< ProcessClient > client = std::make_shared< ProcessClient >(false, logger_, sqlConnectionString, inputId);
        sort( (*cmds).begin(), (*cmds).end(), compareByEnum);

        client->Handle(*cmds);
        ++i;
    }
}
