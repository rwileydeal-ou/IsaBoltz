#ifndef BranchingRatioMacro_h
#define BranchingRatioMacro_h

#include <memory>
#include <client/Sender.h>
#include <cmd/Commands.h>
#include <cmd/MssmSpectrum/Command.h>
#include <cmd/BranchingRatios/Command.h>
#include <macros/Macro.h>
#include <io/FileIO.h>
#include <Models/Connection.h>

class BranchingRatioMacro : public Macro {
private:
    Connection& connection_;
    CommandWithPayload cmd_;
    std::shared_ptr< MssmSpectrumCommand > spectraCmd_;
    std::shared_ptr< Sender > invoker_;
    bool interactiveMode_;
public:
    BranchingRatioMacro(CommandWithPayload cmd, std::shared_ptr< Sender > invoker, std::shared_ptr< MssmSpectrumCommand >& spectraCmd, Connection& connection, bool interactiveMode);
    ~BranchingRatioMacro();
    void Execute() override;
};

#endif
