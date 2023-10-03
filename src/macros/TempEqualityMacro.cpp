#include <macros/TempEqualityMacro.h>

using namespace std;

TempEqualityMacro::TempEqualityMacro(CommandWithPayload cmd, std::shared_ptr< Sender > invoker, std::shared_ptr< MssmSpectrumCommand >& spectraCmd, Connection& connection, bool interactiveMode) : 
    Macro(interactiveMode),
    connection_(connection)
{
    if (! spectraCmd){
        spectraCmd = std::make_shared< MssmSpectrumCommand >( connection_ );
        invoker -> AddCommand( spectraCmd );
    }

    cmd_ = cmd;
    invoker_ = invoker;
    interactiveMode_ = interactiveMode;
}

TempEqualityMacro::~TempEqualityMacro(){
}

void TempEqualityMacro::Execute(){
    vector<string> supportedKeys = { "neutralino1", "gravitino", "axino", "cohosc_saxion", "thermal_saxion", "cohosc_modulus" };
    vector<string> enabledKeys = enabledParticleKeys(supportedKeys, cmd_, "radiation-matter equality temperature");

    for (auto& key : enabledKeys){
        auto splitKey = FileIO::Split(key, "_");
        if (splitKey.size() > 1){
            auto particle = &ModelBaseOps::Find(connection_.Model.Particles, splitKey[1]);
            if (splitKey[0] == "thermal"){
                invoker_ -> AddCommand( 
                    std::make_shared< TempEqualityCommand >( connection_, *particle, ParticleProductionMechanism::THERMAL ) 
                );
            } else if (splitKey[0] == "cohosc"){
                invoker_ -> AddCommand( 
                    std::make_shared< TempOscillationCommand >( connection_, *particle ) 
                );
                invoker_ -> AddCommand( 
                    std::make_shared< TempEqualityCommand >( connection_, *particle, ParticleProductionMechanism::COHERENT_OSCILLATION ) 
                );
            } else{
                throw_with_trace( NotImplementedException() );
            }
        } else{
            // if no specification whether coh osc or thermal, assume thermal and use default
            throw_with_trace( NotImplementedException() );
        }
    }
}
