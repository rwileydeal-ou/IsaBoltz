#include <cmd/TempDecay/Command.h>

using namespace std;

TempDecayCommand::TempDecayCommand(Connection& connection, Models::Particle& particle) :
    connection_(connection)
{
    particle_ = particle;
    this -> receiver_ = std::make_shared< TempDecayReceiver >( connection_, particle_ );
}
TempDecayCommand::TempDecayCommand(Connection& connection, Models::Particle& particle, std::shared_ptr< double > tempDecay) :
    connection_(connection)
{
    particle_ = particle;
    this -> receiver_ = std::make_shared< TempDecayReceiver >( connection_, particle_ );
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

    DbManager db(connection_.SqlConnectionString, connection_.Log);
    db.Open();
    auto statement = Statements::TempDecay( a, Statements::Create );
    db.Execute( statement );
    db.Close();

    ostringstream logEntry;
    logEntry << "Decay temperature for " << particle_.Key << ": " << *tempDecay_ << " GeV";
    connection_.Log.Info( logEntry.str() );
}
