#include <cmd/CheckBBN/Command.h>

using namespace std;

CheckBBNCommand::CheckBBNCommand(Connection& connection, std::string enabledKey) :
    connection_(connection)
{
    // need to first pull all particle evolutions from db
    DbManager db(connection_);
    db.Open();
    Models::ParticleEvolution particleEvolution;
    Models::ScaleFactorPoint scaleFactor;
    Models::TotalWidth totalWidth;

    // pull all particle evolutions for the enabledKey
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

    auto statementEvo = Statements::BoltzmannParticleEvolution( particleEvolution, Statements::StatementType::Read );
    auto filterEvo = Filters::ParticleEvolution( particleKey, production, connection_.InputId, Filters::WhereUUID::InputId );
    statementEvo.AddFilter( filterEvo );
    auto cbEvo = Callbacks::ParticleEvolution();
    db.Execute( statementEvo, cbEvo.Callback, cbEvo.CallbackReturn );

    if ( cbEvo.CallbackReturn.ParticleEvolutions.size() == 0 ){
        throw_with_trace( logic_error("Could not find ParticleEvolutions") );
    }

    for (int i = 0; i < cbEvo.CallbackReturn.ParticleEvolutions.size(); ++i ){
        particleEvolution = cbEvo.CallbackReturn.ParticleEvolutions[cbEvo.CallbackReturn.ParticleEvolutions.size() - 1 - i];

        // pull the particle object (needed for mass) - can't move outside loop since m can be temp-dependent
        auto statementParticle = Statements::Particle( particle_, Statements::StatementType::Read );
        auto filterParticle = Filters::Particle( particleEvolution.ParticleId, Filters::WhereUUID::Id );
        statementParticle.AddFilter( filterParticle );
        auto cbParticle = Callbacks::Particle();
        db.Execute( statementParticle, cbParticle.Callback, cbParticle.CallbackReturn );

        if ( cbParticle.CallbackReturn.Particles.size() != 1 ){
            throw_with_trace( logic_error("Could not find unique particle") );
        }
        particle_ = cbParticle.CallbackReturn.Particles.front();

        // bail on the iteration if can't compute relevant quantities
        if ( particleEvolution.EnergyDensity == 0. ||  particleEvolution.NumberDensity == 0. ){
            continue;
        }

        // pull associated scale factor
        auto statementScale = Statements::ScaleFactor( scaleFactor, Statements::StatementType::Read );
        auto filterScale = Filters::ScaleFactor( particleEvolution.ScaleFactorId, Filters::WhereUUID::Id );
        statementScale.AddFilter( filterScale );
        auto cbScale = Callbacks::ScaleFactor();
        db.Execute( statementScale, cbScale.Callback, cbScale.CallbackReturn );

        if ( cbScale.CallbackReturn.ScaleFactors.size() != 1 ){
            throw_with_trace( logic_error("Could not find unique ScaleFactor") );
        }
        scaleFactor = cbScale.CallbackReturn.ScaleFactors.front();

        // pull associated total width
        auto statementWidth = Statements::TotalWidth( totalWidth, Statements::StatementType::Read );
        auto filterWidth = Filters::TotalWidth( particleEvolution.InputId, particleEvolution.ParticleId );
        statementWidth.AddFilter( filterWidth );
        auto cbWidth = Callbacks::TotalWidth();
        db.Execute( statementWidth, cbWidth.Callback, cbWidth.CallbackReturn );

        if ( cbWidth.CallbackReturn.TotalWidths.size() == 1 ){
            totalWidth = cbWidth.CallbackReturn.TotalWidths.front();
        }else{
            connection_.Log.Warn("Could not find unique total width for " + enabledKey + " with particleId " + to_string(particleEvolution.ParticleId));
        }

        if ( totalWidth.Width * particleEvolution.EnergyDensity / ( scaleFactor.Hubble * particle_.Mass * particleEvolution.NumberDensity) < 2. ){
            // we found it.  
            connection_.Log.Info("w/H: " + to_string(totalWidth.Width / scaleFactor.Hubble));
            connection_.Log.Info("rho/mn: " + to_string(particleEvolution.EnergyDensity / ( particle_.Mass * particleEvolution.NumberDensity )));
            connection_.Log.Info("Energy Density: " + to_string(particleEvolution.EnergyDensity));
            break;
        }
    }

    db.Close();
    this -> receiver_ = std::make_shared< CheckBBNReceiver >( connection_, particle_, particleEvolution, scaleFactor, totalWidth );
}
CheckBBNCommand::~CheckBBNCommand(){
}

void CheckBBNCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto a = receiver_ -> getCheckBBN();

    DbManager db(connection_.SqlConnectionString, connection_.Log);
    db.Open();
    auto statement = Statements::CheckBBN( a, Statements::Create );
    db.Execute( statement );
    db.Close();

    ostringstream logEntry;
    logEntry << "BBN constraint for " << particle_.Key << ": " << a.ConstraintSatisfied;
    connection_.Log.Info( logEntry.str() );
}
