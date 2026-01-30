#include <cmd/SolveBoltzmann/InitialConditions/InitialField/ThermalMatter/Command.h>

using namespace std;

// This command class calculates the initial conditions (at T_RH) for thermally produced matter
// This requires the cross section to have been previously calculated for the particle at T_RH (using the appropriate command)
ThermalMatterCommand::ThermalMatterCommand(
    Connection& connection, 
    DbManager& db,
    const Models::Particle& particle, 
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint
) :
    connection_(connection),
    db_(db)
{
    this -> initialPoint_ = initialPoint;
    this -> particle_ = particle;
    this -> receiver_ = std::make_shared< ThermalMatterReceiver >( 
        connection_, 
        db_,
        initialPoint_, 
        particle_ 
    );
}
ThermalMatterCommand::~ThermalMatterCommand(){
}

void ThermalMatterCommand::postBoltzmannParticleEvolution( const Models::ParticleEvolution& particleEvolution){
    auto statement = Statements::BoltzmannParticleEvolution( particleEvolution, Statements::StatementType::Create );
    db_.Execute(statement);
}

void ThermalMatterCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto a = receiver_ -> getBoltzmannParticleEvolution();
    postBoltzmannParticleEvolution(a);

    ostringstream logEntry;
    logEntry << "Log(n/s0) for " << particle_.Key << ": " << a.Y1;
    connection_.Log.Info( logEntry.str() );
    logEntry.str("");
    logEntry.clear();
    logEntry << "Log(rho/n*T_R) for " << particle_.Key << ": " << a.Y2;
    connection_.Log.Info( logEntry.str() );
}
