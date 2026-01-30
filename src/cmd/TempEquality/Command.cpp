#include <cmd/TempEquality/Command.h>

using namespace std;

TempEqualityCommand::TempEqualityCommand(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle, 
    ParticleProductionMechanism productionMechanism
) :
    connection_(connection),
    db_(db)
{
    particle_ = particle;
    production_ = productionMechanism;
    switch (productionMechanism)
    {
    case ParticleProductionMechanism::COHERENT_OSCILLATION:
        this -> receiver_ = std::make_shared< CohOscTempEqualityReceiver >( 
            connection_, 
            db_,
            particle_, 
            connection_.Model.Cosmology.Temperatures.Reheat 
        );
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

    auto statement = Statements::TempEquality( a, Statements::Create );
    db_.Execute( statement );

    ostringstream logEntry;
    logEntry << "Matter-Radiation equality temperature for " << particle_.Key << ": " << a.Temperature << " GeV";
    connection_.Log.Info( logEntry.str() );
}
