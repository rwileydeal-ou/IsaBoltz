#include <cmd/TempEquality/Command.h>

using namespace std;

TempEqualityCommand::TempEqualityCommand(Connection& connection, Models::Particle& particle, ParticleProductionMechanism productionMechanism) :
    connection_(connection)
{
    particle_ = particle;
    production_ = productionMechanism;
    switch (productionMechanism)
    {
    case ParticleProductionMechanism::COHERENT_OSCILLATION:
        this -> receiver_ = std::make_shared< CohOscTempEqualityReceiver >( connection_, particle_, connection_.Model.Cosmology.Temperatures.Reheat );
        break;
    case ParticleProductionMechanism::THERMAL:
        throw_with_trace( NotImplementedException() );
    default:
        throw_with_trace( NotImplementedException() );
    }
}
TempEqualityCommand::~TempEqualityCommand(){
}

void TempEqualityCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto a = receiver_ -> getTempEquality();

    DbManager db(connection_.SqlConnectionString, connection_.Log);
    db.Open();
    auto statement = Statements::TempEquality( a, Statements::Create );
    db.Execute( statement );
    db.Close();

    ostringstream logEntry;
    logEntry << "Matter-Radiation equality temperature for " << particle_.Key << ": " << a.Temperature << " GeV";
    connection_.Log.Info( logEntry.str() );
}
