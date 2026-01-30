#include <cmd/SolveBoltzmann/InitialConditions/InitialField/CohOscMatter/Command.h>

using namespace std;

CohOscMatterCommand::CohOscMatterCommand(
    Connection& connection, 
    DbManager& db,
    const Models::Particle& particle, 
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint
) :
    connection_(connection),
    db_(db)
{
    this -> initialPoint_ = initialPoint;
    particle_ = particle;
    this -> receiver_ = std::make_shared< CohOscMatterReceiver >( 
        connection_, 
        initialPoint_, 
        particle_ 
    );
}
CohOscMatterCommand::~CohOscMatterCommand(){
}

void CohOscMatterCommand::postBoltzmannParticleEvolution( 
    const Models::ParticleEvolution& particleEvolution
){
    auto statement = Statements::BoltzmannParticleEvolution( particleEvolution, Statements::StatementType::Create );
    db_.Execute(statement);

    // if active, it's oscillating -> record TRH as oscillation temp(for now, actual onset of oscillations has T > TRH but currently starts at TRH)
    if ( particleEvolution.IsActive ){
        Models::TempOscillation tempOsc( *initialPoint_, particleEvolution );
        auto statementOsc = Statements::TempOsc( tempOsc, Statements::StatementType::Create );
        db_.Execute( statementOsc );
    }
}

void CohOscMatterCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto a = receiver_ -> getBoltzmannParticleEvolution();
    postBoltzmannParticleEvolution( a );
    
    ostringstream logEntry;
    logEntry << "Log(n/s0) for " << particle_.Key << ": " << a.Y1;
    connection_.Log.Info( logEntry.str() );
    logEntry.str("");
    logEntry.clear();
    logEntry << "Log(rho/n*T_R) for " << particle_.Key << ": " << a.Y2;
    connection_.Log.Info( logEntry.str() );
}
