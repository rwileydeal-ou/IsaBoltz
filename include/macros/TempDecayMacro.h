#ifndef TempDecayMacro_h
#define TempDecayMacro_h

#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <client/Sender.h>
#include <cmd/Commands.h>
#include <cmd/MssmSpectrum/Command.h>
#include <cmd/BranchingRatios/Command.h>
#include <cmd/TempDecay/Command.h>
#include <macros/Macro.h>
#include <io/FileIO.h>
#include <Models/Connection.h>

class TempDecayMacro : public Macro {
private:
    Connection& connection_;
    CommandWithPayload cmd_;
    std::shared_ptr< Sender > invoker_;
    bool interactiveMode_;
public:
    TempDecayMacro(CommandWithPayload cmd, std::shared_ptr< Sender > invoker, std::shared_ptr< MssmSpectrumCommand >& spectraCmd, Connection& connection, bool interactiveMode);
    ~TempDecayMacro();
    void Execute() override;
};

#endif
