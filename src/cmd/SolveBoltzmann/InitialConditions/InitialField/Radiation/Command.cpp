#include <cmd/SolveBoltzmann/InitialConditions/InitialField/Radiation/Command.h>

using namespace std;

// This command class calculates the initial conditions (at T_RH) for thermally produced matter
// This requires the cross section to have been previously calculated for the particle at T_RH (using the appropriate command)
RadiationCommand::RadiationCommand(Connection& connection, std::shared_ptr< Models::ScaleFactorPoint > initialPoint) :
    connection_(connection)
{
    this -> initialPoint_ = initialPoint;
    pullParticle();
    this -> receiver_ = std::make_shared< RadiationReceiver >( connection_, initialPoint_, particle_ );
}
RadiationCommand::~RadiationCommand(){
}

void RadiationCommand::pullParticle(){
    DbManager db(connection_);
    db.Open();
    Models::Particle p;
    auto statement = Statements::Particle(p, Statements::StatementType::Read);
    auto filter = Filters::Particle("photon", connection_.InputId);
    statement.AddFilter(filter);
    auto cb = Callbacks::Particle();
    db.Execute(statement, cb.Callback, cb.CallbackReturn);
    if (cb.CallbackReturn.Particles.size() == 1){
        particle_ = cb.CallbackReturn.Particles.front();
    } else{
        throw_with_trace( logic_error("Could not pull radiation") );
    }
    db.Close();
}

void RadiationCommand::postBoltzmannParticleEvolution( const Models::ParticleEvolution& particleEvolution){
    DbManager db(connection_);
    db.Open();
    auto statement = Statements::BoltzmannParticleEvolution( particleEvolution, Statements::StatementType::Create );
    db.Execute(statement);
    db.Close();
}

void RadiationCommand::Execute(){
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
