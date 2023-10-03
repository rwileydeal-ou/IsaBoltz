#include <cmd/MssmSpectrum/Receiver.h>

using namespace std;

MssmSpectrumReceiver::MssmSpectrumReceiver(ModelBase& model, std::shared_ptr< DataRelay > fortranInterface, boost::property_tree::ptree model_tree, Logger& logger, string sqlConnectionString, boost::uuids::uuid inputId) :
    model_(model),
    logger_(logger)
{
    sqlConnectionString_ = sqlConnectionString;
    inputId_ = inputId;
    fortranInterface_ = fortranInterface;
    outputTree_ = model_tree;
}

MssmSpectrumReceiver::~MssmSpectrumReceiver(){
}

void MssmSpectrumReceiver::Calculate(){
    try{
        // run sugra to solve the rges down to weak scale
        fortranInterface_ -> RunSugra( model_ );
        // initialize SM and SUSY parameters
        int iallow;
        fortranInterface_ -> InitializeMssmSpectra( model_, iallow );
        // find the lightest susy particle
        fortranInterface_ -> FindLsp( model_ );
        // Test params here
        fortranInterface_ -> TestParameters( model_, iallow );
        // now get the parameters from fortran and assign them to the model
        fortranInterface_ -> Retrieve( model_ );
        
        // now we plunk the particle values into the db
        DbManager db(sqlConnectionString_, logger_);
        db.Open();

        std::deque< std::shared_ptr< Statements::IStatement >, boost::pool_allocator< std::shared_ptr< Statements::IStatement > > > statements; 
        for ( auto& p : model_.Particles ){
            p.InputId = inputId_;
            statements.push_back( std::make_shared< Statements::Particle >( p, Statements::StatementType::Create ) );
        }
        if ( statements.size() > 0 ){
            db.Execute( statements );
        }
        db.Close();
    } catch (exception& e){
        logger_.Error( e.what() );
    }
}
