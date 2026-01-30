#include <cmd/DeltaNeff/Command.h>

using namespace std;

DeltaNeffCommand::DeltaNeffCommand(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle, 
    std::vector< std::vector< boost::uuids::uuid > > childrenIdPairs
) :
    connection_(connection),
    db_(db)
{
    particle_ = particle;
    Models::ParticleEvolution particleEvo;
    particleEvo.ParticleId = particle.Id;
    particleEvo.InputId = connection.InputId;
    this -> receiver_ = std::make_shared< EstimateDeltaNeffReceiver >( 
        connection_, 
        db_,
        particleEvo, 
        particle_, 
        childrenIdPairs 
    );
}
// This constructor computes Delta Neff for <enabledKey>
// Assumes there is already a ParticleEvolution object in the database, computes DeltaNeff from 
DeltaNeffCommand::DeltaNeffCommand(
    Connection& connection, 
    DbManager& db,
    std::string enabledKey
) :
    connection_(connection),
    db_(db)
{    
    // first pull final scale factor
    auto statementScale = Statements::ScaleFactor( 
        finalScaleFactorPoint_, 
        Statements::StatementType::Read 
    );
    auto filter = Filters::ScaleFactor( 
        connection_.InputId, 
        Filters::WhereUUID::InputId 
    );
    statementScale.AddFilter( filter );
    auto cbScale = Callbacks::ScaleFactor();
    db_.Execute( 
        statementScale, 
        cbScale.Callback, 
        cbScale.CallbackReturn 
    );

    if ( cbScale.CallbackReturn.ScaleFactors.size() == 0 ){
        throw_with_trace( logic_error("Could not find ScaleFactors") );
    }
    finalScaleFactorPoint_ = cbScale.CallbackReturn.ScaleFactors[ 0 ];

    if ( finalScaleFactorPoint_.Ordinal != cbScale.CallbackReturn.ScaleFactors.size() - 1){
        throw_with_trace( logic_error("Error in Delta Neff - final scale factor not matching expected ordinal") );
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

    auto statementEvo = Statements::BoltzmannParticleEvolution( 
        particleEvo_, 
        Statements::StatementType::Read 
    );
    auto filterEvo = Filters::ParticleEvolution( 
        particleKey, 
        production, 
        finalScaleFactorPoint_.Id, 
        Filters::WhereUUID::ScaleFactorId 
    );
    statementEvo.AddFilter( filterEvo );
    auto cbEvo = Callbacks::ParticleEvolution();
    db_.Execute( 
        statementEvo, 
        cbEvo.Callback, 
        cbEvo.CallbackReturn 
    );

    if ( cbEvo.CallbackReturn.ParticleEvolutions.size() != 1 ){
        throw_with_trace( logic_error("Could not find unique ParticleEvolution") );
    }
    particleEvo_ = cbEvo.CallbackReturn.ParticleEvolutions.front();

    // now pull the particle object - needed for mass
    auto statementParticle = Statements::Particle( 
        particle_, 
        Statements::StatementType::Read 
    );
    auto filterParticle = Filters::Particle( 
        particleEvo_.ParticleId, 
        Filters::WhereUUID::Id 
    );
    statementParticle.AddFilter( filterParticle );
    auto cbParticle = Callbacks::Particle();
    db_.Execute( 
        statementParticle, 
        cbParticle.Callback, 
        cbParticle.CallbackReturn 
    );

    if ( cbParticle.CallbackReturn.Particles.size() != 1 ){
        throw_with_trace( logic_error("Could not find unique particle") );
    }
    particle_ = cbParticle.CallbackReturn.Particles.front();

    this -> receiver_ = std::make_shared< BoltzmannDeltaNeffReceiver >( 
        connection_, 
        particleEvo_, 
        particle_, 
        finalScaleFactorPoint_ 
    );
}
DeltaNeffCommand::~DeltaNeffCommand(){
}

void DeltaNeffCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto result = receiver_ -> getDeltaNeff();

    auto statement = Statements::DeltaNeff( 
        result, 
        Statements::StatementType::Create 
    );
    db_.Execute( statement );

    ostringstream logEntry;
    logEntry << "Delta N_eff for " << particle_.Key << ": " << result.Delta_Neff;
    connection_.Log.Info( logEntry.str() );
}
