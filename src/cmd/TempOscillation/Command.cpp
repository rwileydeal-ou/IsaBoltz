#include <cmd/TempOscillation/Command.h>

using namespace std;

TempOscillationCommand::TempOscillationCommand(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle
) :
    connection_(connection),
    db_(db)
{
    particle_ = particle;
    this -> receiver_ = std::make_shared< TempOscillationReceiver >( 
        connection_, 
        db_,
        particle_, 
        connection_.Model.Cosmology.Temperatures.Reheat 
    );
}
TempOscillationCommand::~TempOscillationCommand(){
}

void TempOscillationCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto result = receiver_ -> getTempOscillation();

    auto statement = Statements::TempOsc( result, Statements::Create );
    db_.Execute( statement );

    ostringstream logEntry;
    logEntry << "Oscillation temperature for " << particle_.Key << ": " << result.Temperature << " GeV";
    connection_.Log.Info( logEntry.str() );
}
