#ifndef DeltaNeffMacro_h
#define DeltaNeffMacro_h

#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <client/Sender.h>
#include <cmd/Commands.h>
#include <cmd/MssmSpectrum/Command.h>
#include <cmd/BranchingRatios/Command.h>
#include <cmd/TempEquality/Command.h>
#include <cmd/TempOscillation/Command.h>
#include <cmd/TempDecay/Command.h>
#include <cmd/DeltaNeff/Command.h>

#include <macros/Macro.h>
#include <io/FileIO.h>
#include <Models/Connection.h>

class DeltaNeffMacro : public Macro {
private:
    Connection& connection_;
    CommandWithPayload cmd_;
    std::shared_ptr< Sender > invoker_;
    bool interactiveMode_;
public:
    DeltaNeffMacro(CommandWithPayload cmd, std::shared_ptr< Sender > invoker, std::shared_ptr< MssmSpectrumCommand >& spectraCmd, Connection& connection, bool interactiveMode);
    ~DeltaNeffMacro();
    void Execute() override;
};

#endif
