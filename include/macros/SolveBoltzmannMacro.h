#ifndef SolveBoltzmannMacro_h
#define SolveBoltzmannMacro_h

#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <client/Sender.h>

#include <cmd/Commands.h>
#include <cmd/MssmSpectrum/Command.h>
#include <cmd/BranchingRatios/Command.h>
#include <cmd/TempDecay/Command.h>
#include <cmd/SolveBoltzmann/Solver/Command.h>
#include <cmd/DeltaNeff/Command.h>

#include <macros/Macro.h>
#include <macros/BoltzmannInitialConditionsMacro.h>

#include <io/FileIO.h>
#include <Models/ScaleFactor.h>
#include <Models/Connection.h>

class SolveBoltzmannMacro : public Macro {
private:
    Connection& connection_;
    CommandWithPayload cmd_;
    std::shared_ptr< Sender > invoker_;
    bool interactiveMode_;
    std::shared_ptr< Models::ScaleFactorPoint > reheatPoint_;
    std::shared_ptr< MssmSpectrumCommand > spectraCmd_;

    void setInitialConditions(const std::vector<std::string>& enabledKeys, std::shared_ptr< DataRelay >& fortranInterface);
    void solveBoltzmannEqns(const std::vector<std::string>& enabledKeys, std::shared_ptr< DataRelay >& fortranInterface);
public:
    SolveBoltzmannMacro(CommandWithPayload cmd, std::shared_ptr< Sender > invoker, std::shared_ptr< MssmSpectrumCommand >& spectraCmd, bool interactiveMode, Connection& connection);
    ~SolveBoltzmannMacro();
    void Execute() override;
};

#endif
