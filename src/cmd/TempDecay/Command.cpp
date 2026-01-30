#include <cmd/TempDecay/Command.h>

using namespace std;

TempDecayCommand::TempDecayCommand(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle
) :
    connection_(connection),
    db_(db)
{
    particle_ = particle;
    this -> receiver_ = std::make_shared< TempDecayReceiver >( connection_, db_, particle_ );
}
TempDecayCommand::TempDecayCommand(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle, 
    std::shared_ptr< double > tempDecay
) :
    connection_(connection),
    db_(db)
{
    particle_ = particle;
    this -> receiver_ = std::make_shared< TempDecayReceiver >( connection_, db_, particle_ );
    this -> tempDecay_ = tempDecay;
}
TempDecayCommand::~TempDecayCommand(){
}

void TempDecayCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto a = receiver_ -> getTempDecay();
    if (tempDecay_ != nullptr){
        *tempDecay_ = a.Temperature;
    } else{
        tempDecay_ = std::make_shared<double>(a.Temperature);
    }

    auto statement = Statements::TempDecay( a, Statements::Create );
    db_.Execute( statement );

    ostringstream logEntry;
    logEntry << "Decay temperature for " << particle_.Key << ": " << *tempDecay_ << " GeV";
    connection_.Log.Info( logEntry.str() );
}
