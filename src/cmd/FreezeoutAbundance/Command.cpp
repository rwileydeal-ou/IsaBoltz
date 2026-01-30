#include <cmd/FreezeoutAbundance/Command.h>

using namespace std;

FreezeoutAbundanceCommand::FreezeoutAbundanceCommand(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle, 
    boost::uuids::uuid scaleFactorId
) :
    connection_(connection),
    db_(db)
{
    particle_ = particle;
    scaleFactorId_ = scaleFactorId;
    this -> receiver_ = std::make_shared< FreezeoutAbundanceReceiver >( 
        connection_, 
        db_,
        particle_, 
        scaleFactorId_ 
    );
}
FreezeoutAbundanceCommand::~FreezeoutAbundanceCommand(){
}

void FreezeoutAbundanceCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto a = receiver_ -> getFreezeoutAbundance();
    auto b = receiver_ -> getFreezeoutNumberDensity();

    ostringstream logEntry;
    logEntry << "Freeze-out abundance for " << particle_.Key << ": " << a;
    connection_.Log.Info( logEntry.str() );
    logEntry.str("");
    logEntry.clear();
    logEntry << "Freeze-out number density for " << particle_.Key << ": " << b << " GeV^3";
    connection_.Log.Info( logEntry.str() );
}
