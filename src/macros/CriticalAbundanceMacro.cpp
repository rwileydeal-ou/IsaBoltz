#include <macros/CriticalAbundanceMacro.h>

using namespace std;

CriticalAbundanceMacro::CriticalAbundanceMacro(
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
    spectraCmd_ = spectraCmd;

}

CriticalAbundanceMacro::~CriticalAbundanceMacro(){
    tempDecays_.clear();
}

void CriticalAbundanceMacro::Execute(){
    vector<string> supportedKeys = { "neutralino1", "gravitino", "axion", "axino", "saxion", "modulus" };
    CommandWithPayload parentCMD = cmd_;
    CommandWithPayload daughterCMD = cmd_;
    auto splitPayload = FileIO::Split( cmd_.Payload, ">" );
    if (splitPayload.size() > 1){
        parentCMD.Payload = splitPayload[0];
        daughterCMD.Payload = splitPayload[1];
    }
    vector<string> enabledParentKeys = enabledParticleKeys(supportedKeys, parentCMD, "critical abundance parent");
    vector<string> enabledDaughterKeys = enabledParticleKeys(supportedKeys, daughterCMD, "critical abundance daughter");

    auto fortranStuff = spectraCmd_ -> getFortranInterface();

    for (auto& key : enabledParentKeys){
        auto particle = &ModelBaseOps::Find( connection_.Model.Particles, key);        

        invoker_ -> AddCommand(
            std::make_shared< BranchingRatioCommand >( 
                connection_, 
                db_, 
                *particle, 
                connection_.Model.Particles 
            )
        );

        std::shared_ptr< double > tempDecay = std::make_shared<double>();
        tempDecays_.push_back( tempDecay );
        invoker_ -> AddCommand(
            std::make_shared< TempDecayCommand >( 
                connection_, 
                db_,
                *particle, 
                tempDecay
            )
        );

        for (auto& lsp : enabledDaughterKeys){
            auto particleLSP = ModelBaseOps::Find( connection_.Model.Particles, lsp );

            if (lsp == "neutralino1"){
                // need to run IsaRed for neutralino sig.v
                invoker_ -> AddCommand( 
                    std::make_shared< IsaToolsCommand >(IsaTool::IsaRed, connection_, fortranStuff ) 
                );
            }

            invoker_ -> AddCommand(
                std::make_shared< CrossSectionCommand >( 
                    connection_, 
                    db_,
                    particleLSP, 
                    tempDecay, 
                    fortranStuff 
                )
            );

            invoker_ -> AddCommand( 
                std::make_shared< CriticalAbundanceCommand >( 
                    connection_, 
                    db_,
                    *particle 
                ) 
            );
        }
    }
}
