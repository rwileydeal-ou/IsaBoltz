#include <macros/CrossSectionMacro.h>

CrossSectionMacro::CrossSectionMacro(CommandWithPayload cmd, std::shared_ptr< Sender > invoker, std::shared_ptr< MssmSpectrumCommand >& spectraCmd, Connection& connection, bool interactiveMode) : 
    Macro(interactiveMode),
    connection_(connection)
{
    if (!spectraCmd){
        spectraCmd = std::make_shared< MssmSpectrumCommand >( connection_ );
        invoker -> AddCommand( spectraCmd );
    }

    cmd_ = cmd;
    invoker_ = invoker;
    interactiveMode_ = interactiveMode;
    spectraCmd_ = spectraCmd;
    temp_ = std::make_shared<double>();
}
CrossSectionMacro::~CrossSectionMacro(){
}


void CrossSectionMacro::Execute(){
    vector<string> supportedKeys = { "neutralino1", "gravitino", "axion", "axino", "saxion", "modulus" };
    vector<string> enabledKeys = enabledParticleKeys(supportedKeys, cmd_, "thermal cross sections");
    
    *temp_ = connection_.Model.Cosmology.Temperatures.Reheat;
    auto fortranStuff = spectraCmd_ -> getFortranInterface();

    for (auto& key : enabledKeys){
        auto particle = ModelBaseOps::Find(connection_.Model.Particles, key);

        // only need to run IsaRed for neutralino sig.v
        if (key == "neutralino1"){
            invoker_ -> AddCommand( 
                std::make_shared< IsaToolsCommand >( IsaTool::IsaRed, connection_, fortranStuff ) 
            );
        }
        invoker_ -> AddCommand( 
            std::make_shared< CrossSectionCommand >( connection_, particle, temp_, fortranStuff ) 
        );
    }
}
