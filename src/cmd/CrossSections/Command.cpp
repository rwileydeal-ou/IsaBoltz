#include <cmd/CrossSections/Command.h>

using namespace std;

CrossSectionCommand::CrossSectionCommand(
    Connection& connection, 
    DbManager& db,
    const Models::Particle& particle, 
    std::shared_ptr< double > temperature, 
    std::shared_ptr< DataRelay > fortranInterface, 
    bool squashLogs
) :
    connection_(connection),
    db_(db)
{
    scaleFactorId_ = boost::uuids::nil_generator()();
    this -> fortLib_ = fortranInterface;
    this -> temperature_ = temperature;
    particle_ = particle;
    squashLogs_ = squashLogs;
    postResult_ = true;
    this -> receiver_ = std::make_shared< CrossSectionReceiver >( 
        connection_, 
        particle_, 
        temperature_, 
        false, 
        fortLib_
    );
}

CrossSectionCommand::CrossSectionCommand(
    Connection& connection, 
    DbManager& db,
    const Models::Particle& particle, 
    std::shared_ptr< double > temperature, 
    std::shared_ptr< DataRelay > fortranInterface, 
    boost::uuids::uuid scaleFactorId, bool squashLogs
) :
    connection_(connection),
    db_(db)
{
    scaleFactorId_ = scaleFactorId;
    this -> fortLib_ = fortranInterface;
    this -> temperature_ = temperature;
    particle_ = particle;
    squashLogs_ = squashLogs;
    postResult_ = true;
    this -> receiver_ = std::make_shared< CrossSectionReceiver >( 
        connection_, 
        particle_, 
        temperature_, 
        false, 
        fortLib_
    );
}

CrossSectionCommand::CrossSectionCommand(
    Connection& connection, 
    DbManager& db,
    boost::uuids::uuid particleId, 
    std::shared_ptr< double > temperature, 
    std::shared_ptr< DataRelay > fortranInterface, 
    boost::uuids::uuid scaleFactorId, 
    bool squashLogs
) :
    connection_(connection),
    db_(db)
{
    scaleFactorId_ = scaleFactorId;
    this -> fortLib_ = fortranInterface;
    this -> temperature_ = temperature;

    squashLogs_ = squashLogs;
    postResult_ = true;

    auto statement = Statements::Particle( particle_, Statements::Read );
    auto filter = Filters::Particle( particleId, Filters::WhereUUID::Id );
    statement.AddFilter( filter );
    auto cb = Callbacks::Particle();

    db_.Execute( statement, cb.Callback, cb.CallbackReturn );

    if (cb.CallbackReturn.Particles.size() != 1){
        throw_with_trace( logic_error("Could not pull unique particle") );
    }

    particle_ = cb.CallbackReturn.Particles.front();
    this -> receiver_ = std::make_shared< CrossSectionReceiver >( 
        connection_, 
        particle_, 
        temperature_, 
        false, 
        fortLib_
    );
}

CrossSectionCommand::~CrossSectionCommand(){
}

void CrossSectionCommand::PostResult(bool post){
    postResult_ = post;
}

void CrossSectionCommand::postResult(){
    auto statement = Statements::CrossSection( result_, Statements::Create );
    db_.Execute( statement );
}

void CrossSectionCommand::Execute(){
    this -> receiver_ -> Calculate();
    result_ = receiver_ -> getThermalCrossSection();
    if (!scaleFactorId_.is_nil()){
        result_.ScaleFactorId = scaleFactorId_;
    }

    if (postResult_){
        postResult();
    }

    if (!squashLogs_){
        ostringstream logEntry;
        logEntry << "<σ.v> at T=" << *temperature_ << " GeV for " << particle_.Key << ": " << result_.CrossSection << " GeV";
        connection_.Log.Info( logEntry.str() );
    }
}

void CrossSectionCommand::UpdateInputs( 
    std::shared_ptr< double > temperature,
    Models::Particle& particle
){
    particle_ = particle;
    temperature_ = temperature;
    receiver_ -> UpdateInputs( temperature, particle );
}

SigmaV CrossSectionCommand::getResult(){
    return result_;
}
