#ifndef BoltzmannInitialConditionsMacro_h
#define BoltzmannInitialConditionsMacro_h

#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <client/Sender.h>
#include <cmd/Commands.h>
#include <cmd/IsaTools/Command.h>
#include <cmd/CrossSections/Command.h>
#include <cmd/BranchingRatios/Command.h>
#include <cmd/SolveBoltzmann/InitialConditions/InitialPoint/Command.h>
#include <cmd/SolveBoltzmann/InitialConditions/InitialField/CohOscMatter/Command.h>
#include <cmd/SolveBoltzmann/InitialConditions/InitialField/ThermalMatter/Command.h>
#include <cmd/SolveBoltzmann/InitialConditions/InitialField/Radiation/Command.h>
#include <macros/Macro.h>
#include <DataRelay.h>
#include <io/FileIO.h>
#include <Models/ScaleFactor.h>
#include <Models/Connection.h>

class BoltzmannInitialConditionsMacro : public Macro {
private:
    Connection& connection_;
    CommandWithPayload cmd_;
    std::shared_ptr< Sender > invoker_;
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint_;
    std::shared_ptr< DataRelay > fortranInterface_;
    bool interactiveMode_;
    std::vector< std::string > enabledKeys_;
public:
    BoltzmannInitialConditionsMacro(CommandWithPayload cmd, std::shared_ptr< Sender > invoker, std::vector<std::string> enabledKeys, std::shared_ptr< Models::ScaleFactorPoint > initialPoint, bool interactiveMode, std::shared_ptr< DataRelay > fortranInterface, Connection& connection);
    ~BoltzmannInitialConditionsMacro();
    void Execute() override;
};

#endif
