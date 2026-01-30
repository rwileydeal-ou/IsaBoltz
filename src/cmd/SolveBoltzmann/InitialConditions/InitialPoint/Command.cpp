#include <cmd/SolveBoltzmann/InitialConditions/InitialPoint/Command.h>

using namespace std;

InitialPointCommand::InitialPointCommand(
    Connection& connection,
    DbManager& db
) :
    connection_(connection),
    db_(db)
{
    this -> receiver_ = std::make_shared< InitialPointReceiver >( 
        connection_,
        db_,
        connection_.OutputTree 
    );
}
InitialPointCommand::InitialPointCommand(
    Connection& connection, 
    DbManager& db,
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint
) :
    connection_(connection),
    db_(db)
{
    this -> receiver_ = std::make_shared< InitialPointReceiver >( 
        connection_, 
        db_,
        connection_.OutputTree 
    );
    this -> initialPoint_ = initialPoint;
} 

InitialPointCommand::~InitialPointCommand(){
}

void InitialPointCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto a = receiver_ -> getInitialPoint();
    (*initialPoint_).InputId = connection_.InputId;
    (*initialPoint_).GStarEntropic = a.GStarEntropic;
    (*initialPoint_).GStar = a.GStar;
    (*initialPoint_).Entropy = a.Entropy;
    (*initialPoint_).Hubble = a.Hubble;
    (*initialPoint_).ScaleFactor = a.ScaleFactor;
    (*initialPoint_).Temperature = a.Temperature;

    auto statement = Statements::ScaleFactor( *initialPoint_, Statements::Create );
    db_.Execute( statement );

    ostringstream logEntry;
    logEntry << "Initial temperature: " << initialPoint_ -> Temperature << " GeV";
    connection_.Log.Info( logEntry.str() );
    logEntry.str("");
    logEntry.clear();
    logEntry << "Initial entropy: " << initialPoint_ -> Entropy << " GeV^3";
    connection_.Log.Info( logEntry.str() );
    logEntry.str("");
    logEntry.clear();
    logEntry << "Initial Hubble rate: " << initialPoint_ -> Hubble << " GeV";
    connection_.Log.Info( logEntry.str() );
}
