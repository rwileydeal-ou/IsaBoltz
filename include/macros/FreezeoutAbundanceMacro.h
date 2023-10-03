#ifndef FreezeoutAbundanceMacro_h
#define FreezeoutAbundanceMacro_h

#include <memory>
#include <client/Sender.h>
#include <cmd/Commands.h>
#include <cmd/MssmSpectrum/Command.h>
#include <cmd/IsaTools/Command.h>
#include <cmd/BranchingRatios/Command.h>
#include <cmd/CrossSections/Command.h>
#include <cmd/TempFreezeout/Command.h>
#include <cmd/FreezeoutAbundance/Command.h>
#include <macros/Macro.h>
#include <io/FileIO.h>
#include <Models/Connection.h>

class FreezeoutAbundanceMacro : public Macro {
private:
    Connection& connection_;
    CommandWithPayload cmd_;
    std::shared_ptr< MssmSpectrumCommand > spectraCmd_;
    std::shared_ptr< Sender > invoker_;
    bool interactiveMode_;
    std::vector< std::shared_ptr< double > > tempFreezeouts_;
public:
    FreezeoutAbundanceMacro(CommandWithPayload cmd, std::shared_ptr< Sender > invoker, std::shared_ptr< MssmSpectrumCommand >& spectraCmd, Connection& connection, bool interactiveMode);
    ~FreezeoutAbundanceMacro();
    void Execute() override;
};

#endif
