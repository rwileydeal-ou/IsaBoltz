#ifndef ProcessClient_h
#define ProcessClient_h

#include <string>
#include <memory>
#include <boost/filesystem.hpp>

#include <io/Exceptions/NotImplemented.h>
#include <io/Input/Params/ConfigParser.h>
#include <io/Input/Params/ParamCardParser.h>
#include <Models/ModelBase.h>
#include <Models/Connection.h>
#include <client/Sender.h>
#include <cmd/Commands.h>
#include <cmd/MssmSpectrum/Command.h>
#include <macros/BranchingRatioMacro.h>
#include <macros/CrossSectionMacro.h>
#include <macros/TempDecayMacro.h>
#include <macros/TempEqualityMacro.h>
#include <macros/CriticalAbundanceMacro.h>
#include <macros/FreezeoutAbundanceMacro.h>
#include <macros/SolveBoltzmannMacro.h>
#include <macros/DeltaNeffMacro.h>

class ProcessClient
{
private:
    Connection connection_;
    std::string sqlConnectionString_;
    boost::uuids::uuid inputId_;
    Logger& logger_;
    bool initializedParams_;
    bool interactiveMode_;
    std::shared_ptr< MssmSpectrumCommand > spectraCmd_;
    ModelBase model_;
    ModelBase defaultModel_;
    ModelBase getParams();
    void handleCmd(CommandWithPayload cmd, std::shared_ptr< Sender > invoker);
public:
    ProcessClient(bool interactiveMode, Logger& logger, std::string sqlConnectionString, boost::uuids::uuid inputId);
    ~ProcessClient();
    void Handle(std::vector<CommandWithPayload> cmds);
    static CommandWithPayload getCmdFromString(std::string cmd, std::string payload);
    static std::vector<CommandWithPayload> getCmdsFromString(std::vector<std::string> cmds);
};

#endif
