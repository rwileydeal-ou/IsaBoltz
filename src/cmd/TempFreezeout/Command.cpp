#include <cmd/TempFreezeout/Command.h>

using namespace std;

TempFreezeoutCommand::TempFreezeoutCommand(Connection& connection, Models::Particle& particle) :
    connection_(connection)
{
    particle_ = particle;
    this -> receiver_ = std::make_shared< TempFreezeoutReceiver >( connection_, particle_ );
}
TempFreezeoutCommand::TempFreezeoutCommand(Connection& connection, Models::Particle& particle, std::shared_ptr< double > tempFreezeout) :
    connection_(connection)
{
    particle_ = particle;
    this -> receiver_ = std::make_shared< TempFreezeoutReceiver >( connection_, particle_ );
    this -> tempFreezeout_ = tempFreezeout;
}
TempFreezeoutCommand::~TempFreezeoutCommand(){
}

void TempFreezeoutCommand::Execute(){
    this -> receiver_ -> Calculate();
    *tempFreezeout_ = receiver_ -> getTempFreezeout();

    ostringstream logEntry;
    logEntry << "Freeze-out temperature for " << particle_.Key << ": " << *tempFreezeout_ << " GeV";
    connection_.Log.Info( logEntry.str() );
}
