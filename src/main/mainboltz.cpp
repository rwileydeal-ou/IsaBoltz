#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>
#include <proc/RunHandler.h>
#include <io/Input/Run/RunCardParser.h>
#include <io/Logging/Logger.h>
#include <client/ProcessClient.h>
#include <client/ScriptClient.h>
#include <sql/DbManager.h>

typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace> traced;

using namespace std;
using namespace boost::filesystem;

RunCardParser ParseRunCard(Logger& logger){
    // Parse param file to get raw data and create base model
    RunCardParser runFile(logger);
    logger.Info(current_path().string());
    string runCard = (current_path() /= "cards/run_card.dat").string();
    runFile.Parse(runCard);
    return runFile;
}

void startUpVersion(){
    string version = "0.0.1";

    cout << "**********************************************" << endl;
    cout << "**********************************************" << endl;
    cout << "**                                          **" << endl;
    cout << "**                 ISABOLTZ                 **" << endl;
    cout << "**                  v" << version << "                  **" << endl;
    cout << "**                                          **" << endl;
    cout << "**                                          **" << endl;
    cout << "**                                          **" << endl;
    cout << "**                                          **" << endl;
    cout << "**        Authors: Robert Wiley Deal        **" << endl;
    cout << "**        Contact: rwileydeal@ou.edu        **" << endl;
    cout << "**                                          **" << endl;
    cout << "**********************************************" << endl;
    cout << "**********************************************" << endl;
}

// first screen commands
static std::vector<std::string> firstOptions_ = {
    "exit",
    "load_model"
};

static std::vector<std::string> supportedModels_ = {
    "nuhm2",
    "nuhm3",
    "nuhm2_dfsz",
    "nuhm3_dfsz",
    "nuhm2_dfsz_modulus",
    "nuhm3_dfsz_modulus"
};

// these are the currently supported commands
static std::vector<std::string> supportedOptions_ = {
    "exit", 
    "branching_ratios", 
    "cross_sections", 
    "solve_boltzmann", 
    "spectrum",
    "temp_decay",
    "temp_equality",
    "temp_oscillation",
    "critical_abundance",
    "freezeout_abundance",
    "delta_neff"
};

std::string createDatabase( std::string outputPath, Logger& logger ){
    string connectionString = outputPath + "results.db";
    DbManager dbManager(connectionString, logger);
    dbManager.Open();
    dbManager.Configure();
    dbManager.Close();
    return connectionString;
}

void processInteractiveMode(Logger& logger){
    // launch the interactive mode and get commands from user
    std::vector<std::string> cmdsIn = Macro::cmdInput( supportedOptions_ );
    vector<CommandWithPayload> cmds = ProcessClient::getCmdsFromString(cmdsIn);
    if ( cmds.size() > 0 && cmds[0].Command == SupportedCommands::EXIT ){ 
        return; 
    }
    cmds.push_back( CommandWithPayload(SupportedCommands::LAUNCH, "") );

    // create output directory
    string outputPath = FileIO::RunDirectory("output", "run", "_", logger);
    FileIO::CreateDirectory(outputPath, logger);

    // now create database
    auto connectionString = createDatabase( outputPath, logger );
    
    boost::uuids::uuid runId = boost::uuids::random_generator()();

    std::shared_ptr< ProcessClient > client = std::make_shared< ProcessClient> (
    true, 
    logger, 
    connectionString, 
    runId);
    client->Handle(cmds);
}

int main(int argc, char *argv[])
{
    Logger logger("Isaboltz");
    try{
        startUpVersion();
        auto runParams = ParseRunCard(logger).Run;

        // argc > 1 if run in scripting mode (non-interactive)
        // argc == 1 should be interactive (non-scripted)
        if (argc == 1){
            processInteractiveMode(logger);
        } else{
            string script = string(argv[1]);

            // may have specified output directory, if not, default should be created
            string outputDir = "";
            if (argv[2] != nullptr){
                outputDir = string(argv[2]);
            } else {
                outputDir = FileIO::RunDirectory("output", "run", "_", logger);
            }
            FileIO::CreateDirectory(outputDir, logger);

            // now create database
            string connectionString = createDatabase( outputDir, logger );

            if (runParams.ClusterRun.Cluster){
                RunHandler scan(logger);
                scan.HandleClusterRun(script, outputDir, connectionString, runParams.ClusterRun);
            } else{
                ScriptClient client(outputDir, logger);
                vector< CommandWithPayload > cmds = client.scriptInput( script, supportedOptions_ );
                auto macros = client.separateMacros( cmds );
                client.HandleScript(connectionString, macros, runParams.CombineRunOutputs);
            }
        }
    } catch (exception& e){
        logger.Fatal(e.what());

        const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);
        if (st) {
            std::cout << *st << '\n';
        }
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
