#include <cmd/MssmSpectrum/Receiver.h>

using namespace std;

MssmSpectrumReceiver::MssmSpectrumReceiver(
    Connection& connection, 
    DbManager& db,
    std::shared_ptr< DataRelay > fortranInterface
) :
    connection_(connection),
    db_(db)
{
    fortranInterface_ = fortranInterface;
}

MssmSpectrumReceiver::~MssmSpectrumReceiver(){
}

void MssmSpectrumReceiver::Calculate(){
    try{
        connection_.Log.Info( "Current Input ID: " + boost::lexical_cast<std::string>(connection_.InputId) );
        // run sugra to solve the rges down to weak scale
        fortranInterface_ -> RunSugra( connection_.Model );
        // initialize SM and SUSY parameters
        int iallow;
        fortranInterface_ -> InitializeMssmSpectra( connection_.Model, iallow );
        // find the lightest susy particle
        fortranInterface_ -> FindLsp( connection_.Model );
        // Test params here
        fortranInterface_ -> TestParameters( connection_.Model, iallow );
        // now get the parameters from fortran and assign them to the model
        fortranInterface_ -> Retrieve( connection_.Model );
        
        // now we plunk the particle values into the db
        std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > statements; 
        for ( auto& p : connection_.Model.Particles ){
            p.InputId = connection_.InputId;
            statements.push_back( std::make_shared< Statements::Particle >( p, Statements::StatementType::Create ) );
        }
        if ( statements.size() > 0 ){
            db_.Execute( statements );
        }

        auto inputParams = Statements::Inputs( connection_, Statements::StatementType::Create );
        db_.Execute( inputParams );
    } catch (exception& e){
        connection_.Log.Error( e.what() );
    }
}
