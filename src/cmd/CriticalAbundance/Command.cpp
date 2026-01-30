#include <cmd/CriticalAbundance/Command.h>

using namespace std;

CriticalAbundanceCommand::CriticalAbundanceCommand(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle
) :
    connection_(connection),
    db_(db)
{
    particle_ = particle;
    this -> receiver_ = std::make_shared< CriticalAbundanceReceiver >( 
        connection_,
        db_
    );
}
CriticalAbundanceCommand::~CriticalAbundanceCommand(){
}

void CriticalAbundanceCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto a = receiver_ -> getCriticalAbundance();
    auto b = receiver_ -> getCriticalNumberDensity();

    ostringstream logEntry;
    logEntry << "Critical abundance for " << particle_.Key << ": " << a;
    connection_.Log.Info( logEntry.str() );
    logEntry.str("");
    logEntry.clear();
    logEntry << "Critical number density for " << particle_.Key << ": " << b << " GeV^3";
    connection_.Log.Info( logEntry.str() );
}
