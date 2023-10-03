#include <cmd/TempOscillation/Command.h>

using namespace std;

TempOscillationCommand::TempOscillationCommand(Connection& connection, Models::Particle& particle) :
    connection_(connection)
{
    particle_ = particle;
    this -> receiver_ = std::make_shared< TempOscillationReceiver >( connection_, particle_, connection_.Model.Cosmology.Temperatures.Reheat );
}
TempOscillationCommand::~TempOscillationCommand(){
}

void TempOscillationCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto result = receiver_ -> getTempOscillation();

    DbManager db(connection_.SqlConnectionString, connection_.Log);
    db.Open();
    auto statement = Statements::TempOsc( result, Statements::Create );
    db.Execute( statement );
    db.Close();

    ostringstream logEntry;
    logEntry << "Oscillation temperature for " << particle_.Key << ": " << result.Temperature << " GeV";
    connection_.Log.Info( logEntry.str() );
}
