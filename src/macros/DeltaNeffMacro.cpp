#include <macros/DeltaNeffMacro.h>

DeltaNeffMacro::DeltaNeffMacro(
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

DeltaNeffMacro::~DeltaNeffMacro(){
}

void DeltaNeffMacro::Execute(){
    vector<string> supportedParentKeys = { "cohosc_modulus" };
    vector<string> supportedDaughterKeys = { "axion", "alp" };
    CommandWithPayload parentCMD = cmd_;
    CommandWithPayload daughterCMD = cmd_;
    auto splitPayload = FileIO::Split( cmd_.Payload, ">" );
    if (splitPayload.size() > 1){
        parentCMD.Payload = splitPayload[0];
        daughterCMD.Payload = splitPayload[1];
    }
    vector<string> enabledParentKeys = enabledParticleKeys(supportedParentKeys, parentCMD, "Delta N_eff parent");
    vector<string> enabledDaughterKeys = enabledParticleKeys(supportedDaughterKeys, daughterCMD, "Delta N_eff daughter");

    for (auto& key : enabledParentKeys){
        auto splitKey = FileIO::Split(key, "_");
        if (splitKey.size() > 1){
            auto particle = &ModelBaseOps::Find(connection_.Model.Particles, splitKey[1]);
            if (splitKey[0] == "thermal"){
                throw_with_trace( NotImplementedException() );
            } else if (splitKey[0] == "cohosc"){
                // decay temp depends on total width, so need BRs
                invoker_ -> AddCommand( 
                    std::make_shared< BranchingRatioCommand >( 
                        connection_, 
                        db_, 
                        *particle, 
                        connection_.Model.Particles 
                    ) 
                );
                invoker_ -> AddCommand( 
                    std::make_shared< TempDecayCommand >( 
                        connection_, 
                        db_,
                        *particle 
                    )
                );
                invoker_ -> AddCommand(
                    std::make_shared< TempOscillationCommand >( 
                        connection_,
                        db_,
                        *particle 
                    )
                );
                invoker_ -> AddCommand(
                    std::make_shared< TempEqualityCommand >( 
                        connection_, 
                        db_,
                        *particle, 
                        ParticleProductionMechanism::COHERENT_OSCILLATION
                    )
                );

                vector< vector< boost::uuids::uuid > > childIdPairs;
                for (auto& daughterKey : enabledDaughterKeys){
                    auto p = ModelBaseOps::FindConst(connection_.Model.Particles, daughterKey);
                    vector< boost::uuids::uuid > childIdPair{ p.Id, p.Id };
                    childIdPairs.push_back( childIdPair );
                } 

                invoker_ -> AddCommand(
                    std::make_shared< DeltaNeffCommand >( 
                        connection_, 
                        db_, 
                        *particle, 
                        childIdPairs
                    )
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