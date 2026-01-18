#include <cmd/RelicDensity/Command.h>

using namespace std;

RelicDensityCommand::RelicDensityCommand( Connection& connection, const Models::ParticleEvolution& particleEvo, const Models::Particle& particle, const Models::ScaleFactorPoint& finalScaleFactorPoint ) :
    connection_(connection),
    particleEvo_(particleEvo),
    particle_(particle),
    finalScaleFactorPoint_(finalScaleFactorPoint)
{
    this -> receiver_ = std::make_shared< BoltzmannRelicDensityReceiver >( connection_, particleEvo_, particle_, finalScaleFactorPoint_ );
}
RelicDensityCommand::RelicDensityCommand(Connection& connection, std::string enabledKey) :
    connection_(connection)
{
    // need to pull particle evolution from db in this case
    DbManager db(connection_);
    db.Open();
    
    // first pull final scale factor
    auto statementScale = Statements::ScaleFactor( finalScaleFactorPoint_, Statements::StatementType::Read );
    auto filter = Filters::ScaleFactor( connection_.InputId, Filters::WhereUUID::InputId );
    statementScale.AddFilter( filter );
    auto cbScale = Callbacks::ScaleFactor();
    db.Execute( statementScale, cbScale.Callback, cbScale.CallbackReturn );

    if ( cbScale.CallbackReturn.ScaleFactors.size() == 0 ){
        throw_with_trace( logic_error("Could not find ScaleFactors") );
    }
    finalScaleFactorPoint_ = cbScale.CallbackReturn.ScaleFactors[ 0 ];

    if ( finalScaleFactorPoint_.Ordinal != cbScale.CallbackReturn.ScaleFactors.size() - 1){
        throw_with_trace( logic_error("Error in relic density - final scale factor not matching expected ordinal") );
    }

    // now we can pull the final particle evolution
    auto keyComponents = FileIO::Split(enabledKey, "_");
    string particleKey = ( keyComponents.size() > 1 ) ? keyComponents[1] : keyComponents[0];
    ParticleProductionMechanism production;
    if (keyComponents[0] == "cohosc"){
        production = ParticleProductionMechanism::COHERENT_OSCILLATION;
    } else if (particleKey == "photon"){
        production = ParticleProductionMechanism::RADIATION;
    } else{
        production = ParticleProductionMechanism::THERMAL;
    }

    auto statementEvo = Statements::BoltzmannParticleEvolution( particleEvo_, Statements::StatementType::Read );
    auto filterEvo = Filters::ParticleEvolution( particleKey, production, finalScaleFactorPoint_.Id, Filters::WhereUUID::ScaleFactorId );
    statementEvo.AddFilter( filterEvo );
    auto cbEvo = Callbacks::ParticleEvolution();
    db.Execute( statementEvo, cbEvo.Callback, cbEvo.CallbackReturn );

    if ( cbEvo.CallbackReturn.ParticleEvolutions.size() != 1 ){
        throw_with_trace( logic_error("Could not find unique ParticleEvolution") );
    }
    particleEvo_ = cbEvo.CallbackReturn.ParticleEvolutions.front();

    // now pull the particle object - needed for mass
    auto statementParticle = Statements::Particle( particle_, Statements::StatementType::Read );
    auto filterParticle = Filters::Particle( particleEvo_.ParticleId, Filters::WhereUUID::Id );
    statementParticle.AddFilter( filterParticle );
    auto cbParticle = Callbacks::Particle();
    db.Execute( statementParticle, cbParticle.Callback, cbParticle.CallbackReturn );

    if ( cbParticle.CallbackReturn.Particles.size() != 1 ){
        throw_with_trace( logic_error("Could not find unique particle") );
    }
    particle_ = cbParticle.CallbackReturn.Particles.front();

    db.Close();

    this -> receiver_ = std::make_shared< BoltzmannRelicDensityReceiver >( connection_, particleEvo_, particle_, finalScaleFactorPoint_ );
}

RelicDensityCommand::~RelicDensityCommand(){
}

void RelicDensityCommand::postRelicDensity( const Models::RelicDensity& relicDensity){
    auto statement = Statements::RelicDensity( relicDensity, Statements::StatementType::Create );
    DbManager db( connection_ );
    db.Open();
    db.Execute( statement );
    db.Close();
}

void RelicDensityCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto a = receiver_ -> getRelicDensity();

    postRelicDensity( a );

    ostringstream logEntry;
    logEntry << "Relic abundance for " << particleEvo_.ParticleKey << ": " << a.OmegaHSqr;
    connection_.Log.Info( logEntry.str() );
}
