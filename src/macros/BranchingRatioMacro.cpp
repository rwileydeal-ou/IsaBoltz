#include <macros/BranchingRatioMacro.h>

using namespace std;

BranchingRatioMacro::BranchingRatioMacro(
    CommandWithPayload cmd, 
    std::shared_ptr< Sender > invoker, 
    std::shared_ptr< MssmSpectrumCommand >& spectraCmd, 
    Connection& connection, 
    DbManager& db,
    bool interactiveMode
) : 
    Macro(interactiveMode),
    connection_(connection),
    db_(db)
{
    if (!spectraCmd){
        spectraCmd = std::make_shared< MssmSpectrumCommand >( 
            connection_,
            db_
        );
        invoker -> AddCommand( spectraCmd );
    }

    cmd_ = cmd;
    invoker_ = invoker;
    interactiveMode_ = interactiveMode;
}

BranchingRatioMacro::~BranchingRatioMacro(){
}

void BranchingRatioMacro::Execute(){
    vector<string> supportedKeys = { "neutralino1", "gravitino", "axino", "saxion", "modulus" };
    vector<string> enabledKeys = enabledParticleKeys(supportedKeys, cmd_, "branching ratios");

    for (auto& key : enabledKeys){
        auto particle = &ModelBaseOps::Find(connection_.Model.Particles, key);
        invoker_ -> AddCommand( 
            std::make_shared< BranchingRatioCommand >( 
                connection_, 
                db_, 
                *particle, 
                connection_.Model.Particles 
            ) 
        );
    }
}
