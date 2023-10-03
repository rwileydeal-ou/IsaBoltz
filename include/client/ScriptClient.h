#ifndef ScriptClient_h
#define ScriptClient_h

#include <string>
#include <vector>
#include <cmd/Commands.h>
#include <client/ProcessClient.h>
#include <io/Input/Params/ScriptRunnerParser.h>

class ScriptClient
{
private:
    Logger& logger_;
    std::string outputPath_;
    std::vector<CommandWithPayload> handleScan(std::string line, std::vector<CommandWithPayload> baseCmds);
public:
    std::vector<std::vector<CommandWithPayload>> separateMacros( std::vector<CommandWithPayload> allMacros );
    std::vector<CommandWithPayload> scriptInput(std::string script, std::vector<std::string> supportedOptions);
    void HandleScript(std::string sqlConnectionString, std::vector<std::vector<CommandWithPayload>> setOfCmds, bool combineFiles);
    ScriptClient(std::string outputPath, Logger& logger);
    ~ScriptClient();
};

#endif
