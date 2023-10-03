#include <cmd/IsaTools/Command.h>

IsaToolsCommand::IsaToolsCommand(IsaTool tool, Connection& connection, std::shared_ptr< DataRelay > fortranInterface, bool squashLogs) :
    connection_(connection)
{
    this -> fortLib_ = fortranInterface;
    this -> tool_ = tool;
    this -> receiver_ = std::make_shared< IsaToolsReceiver >( tool_, connection_, fortLib_, squashLogs);
}

IsaToolsCommand::~IsaToolsCommand(){
}

void IsaToolsCommand::Execute(){
    this -> receiver_ -> Calculate();
}
