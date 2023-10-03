#include <macros/BoltzmannInitialConditionsMacro.h>

using namespace std;

BoltzmannInitialConditionsMacro::BoltzmannInitialConditionsMacro(CommandWithPayload cmd, std::shared_ptr< Sender > invoker, vector<string> enabledKeys, std::shared_ptr< Models::ScaleFactorPoint > initialPoint, bool interactiveMode, std::shared_ptr< DataRelay > fortranInterface, Connection& connection) : 
    Macro(interactiveMode),
    connection_(connection)
{
    cmd_ = cmd;
    invoker_ = invoker;
    enabledKeys_ = enabledKeys;
    fortranInterface_ = fortranInterface;
    initialPoint_ = initialPoint;
    interactiveMode_ = interactiveMode;
}

BoltzmannInitialConditionsMacro::~BoltzmannInitialConditionsMacro(){
}

void BoltzmannInitialConditionsMacro::Execute(){
    InitialPointCommand initialCmd( 
        connection_, 
        initialPoint_ 
    );

    initialCmd.Execute();

    // should keep a running list of the "unique" keys so we don't calculate BRs for both thermal & cohosc components of same field
    vector<string> uniqueKeys;
    for (auto& key : enabledKeys_){
        auto keyComponents = FileIO::Split(key, "_");
        string particleKey = ( keyComponents.size() > 1 ) ? keyComponents[1] : keyComponents[0];

        auto particle = ModelBaseOps::FindConst(connection_.Model.Particles, particleKey);

        // If we haven't already added the BR command for the field, add one now
        if ( std::find( uniqueKeys.begin(), uniqueKeys.end(), particleKey ) == uniqueKeys.end() ){
            uniqueKeys.push_back( particleKey );
            BranchingRatioCommand brCmd( 
                connection_, 
                particle, 
                connection_.Model.Particles 
            );
            brCmd.Execute();
        }

        // since we need cross sections, if the neutralino is one of the particles in the set of Boltzmann equations, need to run IsaRED first to get sig.v
        if (particleKey == "neutralino1"){
            IsaToolsCommand isaToolsCmd( 
                IsaTool::IsaRed, 
                connection_, 
                fortranInterface_ 
            ); 
            isaToolsCmd.Execute();
        }

        // here we calculate the initial state for both thermal and coh.osc. fields
        if (keyComponents[0] == "cohosc"){
            CohOscMatterCommand cohoscCmd( 
                connection_, 
                particle, 
                initialPoint_ 
            );
            cohoscCmd.Execute();
        } else{
            // and here we add the cross section command
            CrossSectionCommand crossSecCmd( 
                connection_, 
                particle, 
                std::make_shared<double>(initialPoint_ -> Temperature), 
                fortranInterface_, 
                initialPoint_ -> Id 
            );
            crossSecCmd.Execute();

            ThermalMatterCommand thermalCmd( 
                connection_, 
                particle, 
                initialPoint_ 
            );
            thermalCmd.Execute();
        }
    }

    // and here we calculate the initial state for radiation
    RadiationCommand radCmd( 
        connection_, 
        initialPoint_
    );
    radCmd.Execute();
}

