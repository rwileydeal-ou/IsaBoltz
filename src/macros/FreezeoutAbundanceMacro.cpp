#include <macros/FreezeoutAbundanceMacro.h>

using namespace std;

FreezeoutAbundanceMacro::FreezeoutAbundanceMacro(
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

FreezeoutAbundanceMacro::~FreezeoutAbundanceMacro(){
    tempFreezeouts_.clear();
}

void FreezeoutAbundanceMacro::Execute(){
    vector<string> supportedKeys = { "neutralino1", "gravitino", "axion", "axino", "saxion", "modulus" };
    vector<string> enabledKeys = enabledParticleKeys(supportedKeys, cmd_, "freeze-out abundance");

    auto fortranStuff = spectraCmd_ -> getFortranInterface();

    for (auto& key : enabledKeys){
        auto particle = ModelBaseOps::Find(connection_.Model.Particles, key);

        std::shared_ptr< double > tempFreezeout = std::make_shared< double >();
        tempFreezeouts_.push_back( tempFreezeout );
        invoker_ -> AddCommand(
            std::make_shared< TempFreezeoutCommand >( connection_, particle, tempFreezeout )
        );

        // need to run IsaRed for neutralino sig.v
        if (key == "neutralino1"){
            invoker_ -> AddCommand( 
                std::make_shared< IsaToolsCommand >( IsaTool::IsaRed, connection_, fortranStuff ) 
            );
        }

        auto scaleId = boost::uuids::random_generator()();
        invoker_ -> AddCommand(
            std::make_shared< CrossSectionCommand >( 
                connection_, 
                db_,
                particle, 
                tempFreezeout, 
                fortranStuff, 
                scaleId 
            )
        );

        invoker_ -> AddCommand( 
            std::make_shared< FreezeoutAbundanceCommand >( 
                connection_, 
                db_,
                particle, 
                scaleId 
            )
        );
    }
}

