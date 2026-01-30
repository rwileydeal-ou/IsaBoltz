#ifndef CriticalAbundanceMacro_h
#define CriticalAbundanceMacro_h

#include <memory>
#include <client/Sender.h>
#include <cmd/Commands.h>
#include <cmd/MssmSpectrum/Command.h>
#include <cmd/IsaTools/Command.h>
#include <cmd/BranchingRatios/Command.h>
#include <cmd/CrossSections/Command.h>
#include <cmd/TempDecay/Command.h>
#include <cmd/CriticalAbundance/Command.h>
#include <macros/Macro.h>
#include <io/FileIO.h>
#include <Models/Connection.h>

class CriticalAbundanceMacro : public Macro {
private:
    Connection& connection_;
    DbManager& db_;
    CommandWithPayload cmd_;
    std::shared_ptr< MssmSpectrumCommand > spectraCmd_;
    std::shared_ptr< Sender > invoker_;
    bool interactiveMode_;
    std::vector< std::shared_ptr< double > > tempDecays_;
public:
    CriticalAbundanceMacro(
        CommandWithPayload cmd, 
        std::shared_ptr< Sender > invoker, 
        std::shared_ptr< MssmSpectrumCommand >& spectraCmd, 
        Connection& connection, 
        DbManager& db,
        bool interactiveMode
    );
    ~CriticalAbundanceMacro();
    void Execute() override;
};

#endif
