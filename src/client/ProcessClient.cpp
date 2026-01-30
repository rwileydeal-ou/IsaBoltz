#include <client/ProcessClient.h>

using namespace std;
using namespace boost::filesystem;

ProcessClient::ProcessClient(
    bool interactiveMode, 
    Logger& logger, 
    std::string sqlConnectionString, 
    boost::uuids::uuid inputId
) :
    logger_(logger)
{
    sqlConnectionString_ = sqlConnectionString;
    inputId_ = inputId;
    interactiveMode_ = interactiveMode;
    initializedParams_ = false;

    connection_.SqlConnectionString = sqlConnectionString_;
    connection_.InputId = inputId_;
    connection_.Log = logger_;

}

ProcessClient::~ProcessClient(){
}

CommandWithPayload ProcessClient::getCmdFromString(string cmd, string payload){
    CommandWithPayload c;
    c.Payload = payload;
    if (cmd == "exit"){
        c.Command = SupportedCommands::EXIT; 
    } else if (cmd == "branching_ratios"){
        c.Command =  SupportedCommands::BRANCHING_RATIO;
    } else if (cmd == "cross_sections"){
        c.Command =  SupportedCommands::CROSS_SECTION;
    } else if (cmd == "solve_boltzmann"){
        c.Command =  SupportedCommands::SOLVE_BOLTZMANN;
    } else if (cmd == "spectrum"){
        c.Command =  SupportedCommands::SPECTRUM;
    } else if (cmd == "temp_decay"){
        c.Command =  SupportedCommands::TEMP_DECAY;
    } else if (cmd == "temp_oscillation"){
        c.Command =  SupportedCommands::TEMP_OSC;
    } else if (cmd == "temp_equality"){
        c.Command =  SupportedCommands::TEMP_EQUALITY;
    } else if (cmd == "critical_abundance"){
        c.Command = SupportedCommands::CRITICAL_ABUNDANCE;
    } else if (cmd == "freezeout_abundance"){
        c.Command = SupportedCommands::FREEZEOUT_ABUNDANCE;
    } else if (cmd == "delta_neff"){
        c.Command = SupportedCommands::DELTA_NEFF;
    } else{
        throw_with_trace( NotImplementedException() );
    }
    return c;
}

vector<CommandWithPayload> ProcessClient::getCmdsFromString(vector<string> cmdsString){
    vector<CommandWithPayload> cmdsEnum;
    for(auto& cmd : cmdsString){
        cmdsEnum.push_back( getCmdFromString(cmd, "") );
    }
    return cmdsEnum;
}

ModelBase ProcessClient::getParams(){
    ModelBase inputs;

    string configCard = (current_path() /= "cards/config/particle.config").string();
    ConfigParser configFile(logger_);
    configFile.Parse(configCard, inputs);

    // Parse param file to get raw data and create base model
    ParamCardParser paramFile(logger_);
    string paramCard = (current_path() /= "cards/param_card.dat").string();
    paramFile.Parse(paramCard, inputs);
    return inputs;
}

void ProcessClient::handleCmd(
    CommandWithPayload cmd, 
    std::shared_ptr< Sender > invoker,
    DbManager& db
){
    if ( !initializedParams_ ){
        model_ = getParams();

        // TODO: need to create PQ handler for minimizing potential
        model_.PQSector.Potential.vPQ = model_.PQSector.Fa / sqrt(2.0);

        // store default state to revert
        defaultModel_ = model_;
        connection_.Model = model_;
        initializedParams_ = true;
    }

    std::shared_ptr< Macro > macro;

    switch (cmd.Command) {
        case SupportedCommands::LAUNCH: {
            // launch the process
            invoker->InvokeProcess();
        } break;
        case SupportedCommands::PARAMETER_OVERRIDE: {
            ParamCardParser interpreter(logger_);
            interpreter.ReadLine(cmd.Payload, connection_.Model);  
        } break;
        case SupportedCommands::SPECTRUM: {
            if (! this -> spectraCmd_){
                spectraCmd_ = std::make_shared< MssmSpectrumCommand >(
                    connection_,
                    db
                );
                invoker->AddCommand( spectraCmd_ );
            }
        } break;
        case SupportedCommands::BRANCHING_RATIO: {
            macro = std::make_shared< BranchingRatioMacro >(
                cmd,
                invoker,
                this -> spectraCmd_,
                connection_,
                db,
                interactiveMode_
            );
        } break;
        case SupportedCommands::CROSS_SECTION: {
            macro = std::make_shared< CrossSectionMacro >(
                cmd,
                invoker,
                this -> spectraCmd_,
                connection_,
                db,
                interactiveMode_
            );
        } break;
        case SupportedCommands::TEMP_DECAY: {
            macro = std::make_shared< TempDecayMacro >(
                cmd,
                invoker,
                this -> spectraCmd_,
                connection_,
                db,
                interactiveMode_
            );
        } break;
        case SupportedCommands::TEMP_EQUALITY: {
            macro = std::make_shared< TempEqualityMacro >(
                cmd,
                invoker,
                this -> spectraCmd_,
                connection_,
                db,
                interactiveMode_
            );
        } break;
        case SupportedCommands::CRITICAL_ABUNDANCE: {
            macro = std::make_shared< CriticalAbundanceMacro >(
                cmd,
                invoker,
                this -> spectraCmd_,
                connection_,
                db,
                interactiveMode_
            );
        } break;
        case SupportedCommands::FREEZEOUT_ABUNDANCE: {
            macro = std::make_shared< FreezeoutAbundanceMacro >(
                cmd,
                invoker,
                this -> spectraCmd_,
                connection_,
                db,
                interactiveMode_
            );
        } break;
        case SupportedCommands::SOLVE_BOLTZMANN: {
            macro = std::make_shared< SolveBoltzmannMacro >( 
                cmd,
                invoker,
                this -> spectraCmd_,
                interactiveMode_, 
                connection_,
                db
            );
        } break;
        case SupportedCommands::DELTA_NEFF: {
            macro = std::make_shared< DeltaNeffMacro >(
                cmd,
                invoker,
                this -> spectraCmd_,
                connection_,
                db,
                interactiveMode_ 
            );
        } break;
        default:
            throw_with_trace( NotImplementedException() );
    }

    if (macro != nullptr){
        macro -> Execute();
    }
}

// This method processes the cmds and writes the results to the boost ptree 
void ProcessClient::Handle(vector<CommandWithPayload> cmds){
    try{
        DbManager db(connection_);
        std::shared_ptr< Sender > invoker = std::make_shared< Sender >();

        for (auto& cmd : cmds){
            handleCmd(cmd, invoker, db);
        }
        connection_.Log.Info("Finished processing");
    } catch( boost::numeric::ublas::internal_logic& e ){
        connection_.Log.Error(e.what());
    } catch( std::logic_error& e ){
        connection_.Log.Error(e.what());
    }
}
