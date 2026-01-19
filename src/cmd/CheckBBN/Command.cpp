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

    auto statementEvo = Statements::BoltzmannParticleEvolution( 
        particleEvolution, 
        Statements::StatementType::Read 
    );
    auto filterEvo = Filters::ParticleEvolution( 
        particleKey, 
        production, 
        connection_.InputId, 
        Filters::WhereUUID::InputId 
    );
    statementEvo.AddFilter( filterEvo );
    auto cbEvo = Callbacks::ParticleEvolution();
    db.Execute( statementEvo, cbEvo.Callback, cbEvo.CallbackReturn );

    if ( cbEvo.CallbackReturn.ParticleEvolutions.size() == 0 ){
        throw_with_trace( logic_error("Could not find ParticleEvolutions") );
    }


    // pull all scale factors now
    auto statementScales = Statements::ScaleFactor( 
        scaleFactor, 
        Statements::StatementType::Read 
    );
    auto filterScales = Filters::ScaleFactor( 
        connection_.InputId, 
        Filters::WhereUUID::InputId 
    );
    statementScales.AddFilter( filterScales );
    auto cbScales = Callbacks::ScaleFactor();
    db.Execute( 
        statementScales, 
        cbScales.Callback, 
        cbScales.CallbackReturn 
    );
    auto scaleFactors = cbScales.CallbackReturn.ScaleFactors;

    // pull the particle objects (needed for mass)
    auto statementParticles = Statements::Particle( 
        particle_, 
        Statements::StatementType::Read 
    );
    auto filterParticles = Filters::Particle( 
        connection_.InputId, 
        Filters::WhereUUID::InputId 
    );
    statementParticles.AddFilter( filterParticles );
    auto cbParticles = Callbacks::Particle();
    db.Execute( 
        statementParticles, 
        cbParticles.Callback, 
        cbParticles.CallbackReturn 
    );
    auto particles = cbParticles.CallbackReturn.Particles;

    // pull total widths
    auto statementWidths = Statements::TotalWidth( 
        totalWidth, 
        Statements::StatementType::Read 
    );
    auto filterWidths = Filters::TotalWidth( 
        connection_.InputId, 
        Filters::WhereUUID::InputId
    );
    statementWidths.AddFilter( filterWidths );
    auto cbWidths = Callbacks::TotalWidth();
    db.Execute( 
        statementWidths, 
        cbWidths.Callback, 
        cbWidths.CallbackReturn 
    );
    auto totalWidths = cbWidths.CallbackReturn.TotalWidths;


    for (int i = 0; i < cbEvo.CallbackReturn.ParticleEvolutions.size(); ++i ){
        particleEvolution = cbEvo.CallbackReturn.ParticleEvolutions[cbEvo.CallbackReturn.ParticleEvolutions.size() - 1 - i];

        particle_ = findParticle( 
            particles, 
            particleEvolution.ParticleId
        );

        // bail on the iteration if can't compute relevant quantities
        if ( particleEvolution.EnergyDensity == 0. ||  particleEvolution.NumberDensity == 0. ){
            continue;
        }

        // pull associated data
        scaleFactor = findScaleFactor( 
            scaleFactors, 
            particleEvolution.ScaleFactorId 
        );
        totalWidth = findTotalWidth( 
            totalWidths, 
            particleEvolution.ParticleId 
        );

        if ( totalWidth.Width * particleEvolution.EnergyDensity / ( scaleFactor.Hubble * particle_.Mass * particleEvolution.NumberDensity) < 2. ){
            // we found it.  
            break;
        }
    }

    db.Close();
    this -> receiver_ = std::make_shared< CheckBBNReceiver >( 
        connection_, 
        particle_, 
        particleEvolution, 
        scaleFactor, 
        totalWidth 
    );
}
CheckBBNCommand::~CheckBBNCommand(){
}

Models::ScaleFactorPoint CheckBBNCommand::findScaleFactor( 
    std::deque<Models::ScaleFactorPoint>& scaleFactors,  
    boost::uuids::uuid scaleFactorId
){
    for (auto& scaleFactor : scaleFactors){
        if ( scaleFactor.Id == scaleFactorId ){
            return scaleFactor;
        }
    }
    throw_with_trace( logic_error("Could not find scale factor") );
}

Models::TotalWidth CheckBBNCommand::findTotalWidth( 
    std::deque<Models::TotalWidth>& totalWidths,  
    boost::uuids::uuid parentId
){
    for (auto& totalWidth : totalWidths){
        if ( totalWidth.ParentId == parentId ){
            return totalWidth;
        }
    }
    connection_.Log.Warn("Could not find total width for parent " + boost::lexical_cast<std::string>( parentId ));
    return Models::TotalWidth();
}

Models::Particle CheckBBNCommand::findParticle( 
    std::deque<Models::Particle>& particles,  
    boost::uuids::uuid particleId
){
    for (auto& particle : particles){
        if ( particle.Id == particleId ){
            return particle;
        }
    }
    throw_with_trace( logic_error("Could not find particle") );
}

void CheckBBNCommand::Execute(){
    this -> receiver_ -> Calculate();
    auto bbn = receiver_ -> getCheckBBN();

    DbManager db(connection_.SqlConnectionString, connection_.Log);
    db.Open();
    auto statement = Statements::CheckBBN( bbn, Statements::Create );
    db.Execute( statement );
    db.Close();

    ostringstream logEntry;
    logEntry << "BBN constraint for " << particle_.Key << ": " << bbn.ConstraintSatisfied;
    connection_.Log.Info( logEntry.str() );
}
