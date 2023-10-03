#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <proc/RunHandler.h>
#include <io/Input/Run/RunCardParser.h>
#include <client/ProcessClient.h>
#include <client/ScriptClient.h>

using namespace std;
using namespace boost::filesystem;

RunCardParser ParseRunCard(Logger& logger){
    // Parse param file to get raw data and create base model
    RunCardParser runFile(logger);
    string runCard = (current_path() /= "cards/run_card.dat").string();
    runFile.Parse(runCard);
    return runFile;
}

string startUpVersion(){
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

    return version;
}

// these are the currently supported commands
vector<string> supportedOptions_ = {
    "exit", 
    "branching_ratios", 
    "cross_sections", 
    "solve_boltzmann", 
    "spectrum",
    "temp_decay",
    "temp_equality",
    "temp_oscillation"
};

int main(int argc, char *argv[])
{
    Logger logger("Isaboltz");
    try{
        string version = startUpVersion();
        if (argc > 2){
            string script = string(argv[1]);
            string sqlConnectionString = string(argv[2]);

            // may have specified output directory, if not, default should be created
            string outputDir = "";
            if (argv[3] != nullptr){
                outputDir = string(argv[3]);
            }
            
            auto runParams = ParseRunCard(logger).Run;

            ScriptClient client(outputDir, logger);
            vector< CommandWithPayload > cmds = client.scriptInput( script, supportedOptions_ );
            auto macros = client.separateMacros( cmds );
            client.HandleScript(sqlConnectionString, macros, runParams.CombineRunOutputs);
        }
    } catch(exception& e){
        logger.Fatal(e.what());

        const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);
        if (st) {
            std::cout << *st << '\n';
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
