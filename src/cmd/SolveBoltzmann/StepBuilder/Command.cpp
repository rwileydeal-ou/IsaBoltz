#include <cmd/SolveBoltzmann/StepBuilder/Command.h>
#include <chrono>

using namespace std;

BoltzmannStepBuilderCommand::BoltzmannStepBuilderCommand(
    Connection& connection, 
    std::shared_ptr< DataRelay > fortranInterface, 
    const Models::ScaleFactorPoint& reheatPoint, 
    const std::deque< Models::ParticleEvolution >& initialParticleEvolutions, 
    std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, 
    std::map< std::string, std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > >& partialWidths, 
    std::map< std::string, Models::TotalWidth >& totalWidths
) :
    connection_(connection),
    db_(connection),
    particles_(particles),
    partialWidths_(partialWidths),
    totalWidths_(totalWidths),
    reheatPoint_(reheatPoint),
    initialParticleEvolutions_(initialParticleEvolutions)
{
    fortranInterface_ = fortranInterface;

    ordinal_ = 0;
    isInitialized_ = false;
    tempDependentMassEnabled_ = false;

    currentParticleData_.reserve( initialParticleEvolutions.size() );

    resetParticleData();
    db_.Open();
    startTime_ = std::chrono::system_clock::now();
}

BoltzmannStepBuilderCommand::~BoltzmannStepBuilderCommand(){
    db_.Close();
}


/* 

    Methods used for initialization 

*/

void BoltzmannStepBuilderCommand::resetParticleData(){
    for (auto& p : initialParticleEvolutions_){
        ParticleData particle( p );
        for (auto& part : particles_){
            if ( part.Id == p.ParticleId ){
                particle.Spin = part.Spin;
                particle.Statistics = part.Statistics;
                particle.Charges = part.Charges;
                particle.Mass = part.Mass;
                particle.PdgCode = part.PdgCode;
                particle.IsLSP = false;
                
                // TODO: THIS IS A HACK.  SHOULD REFACTOR AND PUT IN CONFIG, AND MAKE A SETTING/STORE IN DB
                if (particle.ParticleKey == "axion"){
                    particle.TempDependentMass = true;
                    tempDependentMassEnabled_ = true;
                    double fTheta = pow( log( exp(1.) / (1. - pow(connection_.Model.PQSector.Theta_I / M_PI, 2.) ) ), 7./6. );
                    particle.Amplitude = 1.2 * connection_.Model.PQSector.Fa / connection_.Model.PQSector.nDW * connection_.Model.PQSector.Theta_I * sqrt(fTheta);
                }
                // TODO: THIS IS ALSO A HACK, REVISE...
                if (particle.ParticleKey == "neutralino1"){
                    particle.IsLSP = true;
                }
                break;
            }
        }

        particle.TotalWidth = (totalWidths_.find( particle.ParticleKey )->second).Width;
        sqlDataToPost_.ParticleDatas.push_front( particle );
    }
}

Models::ScaleFactorPoint BoltzmannStepBuilderCommand::pullPreviousScaleFactorPoint( int ordinal ){
    if (ordinal == 0){
        return reheatPoint_;
    }

    int ord = ordinal;
    for ( auto& s : sqlDataToPost_.ScaleFactors ){
        if ( s.Ordinal == ord ){
            if (s.ScaleFactor <= currentPoint_.ScaleFactor && !std::isinf(s.Hubble) || ( s.ScaleFactor == 1. && s.Ordinal < 2 )){
                return s;
            } else{
                currentPoint_.Ordinal -= 1;
                ordinal_ -= 1;
                ord -= 1;
            }
        }
    }

    throw_with_trace( std::logic_error("Could not find previous scale factor") );
} 

ParticleData BoltzmannStepBuilderCommand::pullParticleEvolution( std::string particleKey, ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId ){
    auto r2 = std::find_if( 
        sqlDataToPost_.ParticleDatas.begin(), sqlDataToPost_.ParticleDatas.end(), 
        [&particleKey, &scaleFactorId, &productionMechanism, this](const ParticleData& part){ 
            return ( part.ParticleKey == particleKey && part.ProductionMechanism == productionMechanism && part.ScaleFactorId == scaleFactorId ); 
        } 
    );

    if (r2 != sqlDataToPost_.ParticleDatas.end()){
        return *r2;
    }

    throw_with_trace( std::logic_error("Could not find particle data") );
}


/* 

    Methods used to compute RHS of Boltzmann equations 

*/

boost::numeric::ublas::vector<double> BoltzmannStepBuilderCommand::UpdateInitialConditions(){
    boost::numeric::ublas::vector<double> currentState ( 2 * currentParticleData_.size() );
    for (int index = 0; index < currentParticleData_.size(); ++index){
        auto p = currentParticleData_[index];        

/*        // update modes marked to be turned off
        if ( p.TurnOff ){
            // flip back so won't get ignored if reproduced from other mechanism
            p.TurnOff = false;
            currentState( 2 * index ) = 0.;
            currentState( 2 * index + 1 ) = 0.;
            continue;
        }
*/
        // update initial conditions for coh.osc. fields with temp-dependent mass
        if ( p.TempDependentMass && p.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION ){
            // applies to temp dependent coh.osc.masses only
            if ( !p.IsOscillating ){
                currentState( 2 * index ) = log( p.Mass * pow( p.Amplitude, 2. ) / 2. / reheatPoint_.Entropy );
            } else{
                currentState( 2 * index ) = x_[ 2 * index];
            }
            currentState( 2 * index + 1 ) = log( p.Mass / reheatPoint_.Temperature );
            continue;
        }

        // if not special condition above, set to expected value
        currentState( 2 * index ) = x_[ 2 * index];
        currentState( 2 * index + 1 ) = x_[ 2 * index + 1];
    }
    return currentState;
}



double BoltzmannStepBuilderCommand::tempRadiation(long double entropy, long double scaleFactor){
    long double X = pow(entropy, 1./3.) / scaleFactor;
    double gstarFit;

    //  Use fit for region around T=200 MeV (0.048 < S/R^3 < 0.33)
    if(X < 0.692 && X > 0.365){
        return -0.36928 + 3.41509 * X - 7.71709 * pow(X, 2.) + 7.50193 * pow(X, 3.) - 2.4241 * pow(X, 4.);
    }

    //  Initial guess:
    if(X < 1.01e-20){ gstarFit = 3.9; }
    else if(X < 3.3e-12){ gstarFit = 10.; }
    else if(X < 0.3){ gstarFit = 21.; }
    else if(X < 0.5){ gstarFit = 44.; }
    else if(X < 0.6){ gstarFit = 59.; }
    else if(X < 0.8){ gstarFit = 63.; }
    else if(X < 19.7){ gstarFit = 70.; }
    else{ gstarFit = 150.; }

    double T0 = X * pow( 45. / ( 2. * gstarFit * pow(M_PI, 2.) ), 1./3. );
    double T1 = 0.;
    double DT = 0.;

    while(abs(T0-T1) / T0 > 0.01){
        double gstr = GStar::CalculateEntropic(particles_, connection_, T0);
        T1 = X * pow( 45. / ( 2. * gstr * pow(M_PI, 2.) ), 1./3.);
        if( abs(T0-T1) == DT ){
            T0 = (T0 + T1) / 2.;
        }
        else{
            DT = abs( T0 - T1);
            T0 = T1;
        }
    }
    if (std::isnan(T1)){
        connection_.Log.Info(
            "X: " + boost::lexical_cast<std::string>(X)
            );
    }
    if (T1 > reheatPoint_.Temperature){
        connection_.Log.Debug("Radiation temperature calculated above reheat... Setting to reheat temperature");
        T1 = reheatPoint_.Temperature;
    }

    connection_.Log.Trace("Fitting Radiation Temperature to T=" + to_string(T1));
    return T1;
}

// based on the results of the previous step, we can set everything about the point EXCEPT the Hubble parameter
// Hubble requires total energy density - calculate that afterwards
Models::ScaleFactorPoint BoltzmannStepBuilderCommand::setCurrentPoint(){
    Models::ScaleFactorPoint point;
    point.Ordinal = ordinal_;
    point.InputId = connection_.InputId;
    point.ScaleFactor = expl(t_);
    point.Entropy = reheatPoint_.Entropy * expl( x_[0] );
    point.Temperature = tempRadiation( point.Entropy, point.ScaleFactor );
    point.GStar = GStar::Calculate( particles_, connection_, point.Temperature );
    point.GStarEntropic = GStar::CalculateEntropic( particles_, connection_, point.Temperature, point.GStar );
    return point;
}

void BoltzmannStepBuilderCommand::calculateHubble(){
    double energyDensity = 0.;
    for (auto& p : currentParticleData_){
        energyDensity += p.EnergyDensity;
    }
    currentPoint_.Hubble = sqrt( energyDensity / 3. ) / connection_.Model.Constants.mPlanck;
}

// This method updates the necessary IDs for internal data if there are temperature dependent masses
void BoltzmannStepBuilderCommand::updateParticleMassAndId( ParticleData& particle, std::map<string, boost::uuids::uuid>& resetKeys, int index, const ParticleData& previousParticle, const Models::ScaleFactorPoint& previousPoint ){
    if (!tempDependentMassEnabled_ || !particle.TempDependentMass){
        return;
    }

    // TODO: create command for this/make modular
    if ( particle.ParticleKey == "axion" ){
        particle.Mass = ( 6.2e-3 ) 
        * min( 1., 0.018 * pow( connection_.Model.StandardModel.LambdaQCD / currentPoint_.Temperature, 4. ) )
        * connection_.Model.PQSector.nDW / connection_.Model.PQSector.Fa;
    }

    // pull the particle, then reassign necessary values
    auto newParticle = std::find_if( 
        particles_.begin(), particles_.end(), 
        [&particle, this](Models::Particle& p){ 
            return p.Key == particle.ParticleKey; 
        } 
    );

    auto updatedID = resetKeys.find( particle.ParticleKey );
    // if particle hasn't been updated yet, update now
    if ( updatedID == resetKeys.end() ){
        // define a new ID for particle and update data
        newParticle -> Id = boost::uuids::random_generator()();
        newParticle -> Mass = particle.Mass;
        resetKeys.insert( { particle.ParticleKey, newParticle -> Id } );
        particle.ParticleId = newParticle -> Id;
    } else{
        particle.ParticleId = updatedID->second;
    }

    if (
        particle.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION
        && ( particle.IsOscillating || particle.IsActive )
    ){
        particle.NumberDensity = reheatPoint_.Entropy * expl( particle.Y1 );
        // With our defn of Y2 = log( rho / (n*T_R) ), this gives us rho/n so that we can combine with n to get rho
        particle.EnergyDensity = reheatPoint_.Temperature * expl( particle.Y2 ) * particle.NumberDensity;
    }
}

// This method sets the initial conditions and corresponding densities for an evolution based on results of the previous step
void BoltzmannStepBuilderCommand::setEvolutionInitialState( ){
    // we use keys (for now) to identify when we've reset the mass for a particle which may have both thermal and coh.osc. components
    // this way we don't set a ParticleId then immediately reset it for the other production mechanism struct
    std::map<string, boost::uuids::uuid> resetKeys;

    // since we're calculating the step's initial state, easiest to just clear previous evolutions
    currentParticleData_.clear();

    // pull the previous step, then we can pull the associated particle data so we can start this step with any updated data of last step
    Models::ScaleFactorPoint previousPoint = pullPreviousScaleFactorPoint( ordinal_ - 1 );

    for (int index = 0; index < initialParticleEvolutions_.size(); ++index){
        auto p = initialParticleEvolutions_[index];        
        // we need to pull the previous evo to determine if this evo should start out active or not
        auto prevP = pullParticleEvolution( p.ParticleKey, p.ProductionMechanism, previousPoint.Id );
        ParticleData nextP(prevP);
        nextP.ParticleEvolutionId = boost::uuids::random_generator()();
        nextP.ScaleFactorId = currentPoint_.Id;
        nextP.EqnIndex = index;
        nextP.Y1 = x_[ 2 * index ];
        nextP.Y2 = x_[ 2 * index + 1 ];

        // set initial dxdt to 0, add components after other quantities computed
        dxdt_[ 2 * index ] = 0.;
        dxdt_[ 2 * index + 1 ] = 0.;

        for( int j = 0; j < 2 * initialParticleEvolutions_.size(); ++j ){
            jac_( 2 * index, j ) = 0.;
            jac_( 2 * index + 1, j ) = 0.;
        }

        if (nextP.ProductionMechanism == ParticleProductionMechanism::RADIATION){
            setRadiationInitialState( nextP );
        } else{
            setMatterInitialState( nextP );
        }

        updateParticleMassAndId( nextP, resetKeys, index, prevP, previousPoint );

        // finally calculate cross sections
        calculateCrossSection( nextP );
        currentParticleData_.push_back(nextP);
    }
}

void BoltzmannStepBuilderCommand::setRadiationInitialState( ParticleData& radiation ){
    radiation.IsActive = true;
    long double n = boost::math::zeta(3.) * currentPoint_.GStarEntropic * pow(currentPoint_.Temperature, 3.) / pow(M_PI, 2.);
    radiation.NumberDensity = n;
    radiation.EquilibriumNumberDensity = 0.;
    long double rho = pow(M_PI, 2.) * currentPoint_.GStar * pow(currentPoint_.Temperature, 4.) / 30.;
    radiation.EnergyDensity = rho;
}

void BoltzmannStepBuilderCommand::setMatterInitialState( ParticleData& matter ){
    if (!matter.IsActive || matter.Y1 == 0. || matter.Y2 == 0. ){
        return;
    }
    
    // With our defn of Y1 = log(n/s0), just gets rid of initial entropy to recover n (Entropy / Comoving Volume = entropy density)
    matter.NumberDensity = reheatPoint_.Entropy * expl( matter.Y1 );
    // With our defn of Y2 = log( rho / (n*T_R) ), this gives us rho/n so that we can combine with n to get rho
    matter.EnergyDensity = reheatPoint_.Temperature * expl( matter.Y2 ) * matter.NumberDensity;

    if ( matter.ProductionMechanism == ParticleProductionMechanism::THERMAL ){
        // quickly calculate the degrees of freedom
        int degF =  ( (int)std::round(2. * matter.Spin) ) + 1;
        matter.EquilibriumNumberDensity = NumberDensityEq::Calculate( matter.Mass, currentPoint_.Temperature, degF, matter.Statistics );
    }
}

void BoltzmannStepBuilderCommand::calculateCrossSection( ParticleData& particle ){
    if ( 
        particle.ProductionMechanism == ParticleProductionMechanism::RADIATION 
        || 
        particle.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION 
    ){
        particle.AnnihilationCrossSection = 0.;
        return;
    }

    // only need to run IsaRed for neutralino sig.v
    if (particle.ParticleKey == "neutralino1" && false){
        // we only actually need to run IsaRed if it's marked as reload
        // if temps are close together, stores previous result until temp above certain percent
        if (fortranInterface_->Reload( currentPoint_.Temperature / particle.Mass )){
            IsaToolsCommand isaTools( IsaTool::IsaRed, connection_, fortranInterface_, true );
            isaTools.Execute();
        }
    }

    Models::Particle part;
    part.Id = particle.ParticleId;
    part.PdgCode = particle.PdgCode;
    part.Mass = particle.Mass;
    part.Key = particle.ParticleKey;

    CrossSectionCommand cs( connection_, part, std::make_shared< double >(currentPoint_.Temperature), fortranInterface_, currentPoint_.Id, true );
    // wait to post til later, do in batch
    cs.PostResult(false);
    cs.Execute();
    auto res = cs.getResult();
    particle.AnnihilationCrossSection = res.CrossSection;
}

void BoltzmannStepBuilderCommand::handleTemperatureDependence( ParticleData& particle ){
    auto newParticle = std::find_if( 
        particles_.begin(), particles_.end(), 
        [&particle, this](const Models::Particle& part){ 
            return part.Id == particle.ParticleId; 
        } 
    );

    if (newParticle == particles_.end()){
        throw logic_error("Failed to find particle");
    }

    BranchingRatioCommand brCmd( connection_, *newParticle, particles_, true );
    brCmd.PostResult(false);
    brCmd.Execute();
    auto brResult = brCmd.getResult();
    particle.TotalWidth = brResult.TotalWidth.Width;

    auto oldWidth = totalWidths_.find( particle.ParticleKey );
    if ( oldWidth != totalWidths_.end() ){
        totalWidths_.erase( oldWidth );
    }

    auto oldPartialWidths = partialWidths_.find( particle.ParticleKey );
    if ( oldPartialWidths != partialWidths_.end() ){
        partialWidths_.erase( oldPartialWidths );
    }

    totalWidths_.insert( { particle.ParticleKey, brResult.TotalWidth } );
    partialWidths_.insert( { particle.ParticleKey, brResult.PartialWidths } );
}

void BoltzmannStepBuilderCommand::handleTemperatureDependences(){
    if (!tempDependentMassEnabled_){
        return;
    }
    
    for ( auto& particle : currentParticleData_ ){
        if ( particle.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
            continue;
        }
        handleTemperatureDependence( particle );
    }
}

void BoltzmannStepBuilderCommand::checkTransitions(){
    if ( 
        currentPoint_.Temperature == 0. 
        || std::isnan( currentPoint_.Temperature ) 
        || std::isinf( currentPoint_.Temperature ) 
        || currentPoint_.Hubble == 0. 
        || std::isnan( currentPoint_.Hubble ) 
        || std::isinf( currentPoint_.Hubble ) 
    ){
        return;
    }
    for (auto& p : currentParticleData_){
        checkOscillationTransition( p );
        checkDecayTransition( p );
    }
}

void BoltzmannStepBuilderCommand::checkOscillationTransition( ParticleData& particle ){
    // we only care about non-active, coh.osc. particles
    if ( 
        particle.IsActive 
        || particle.ProductionMechanism != ParticleProductionMechanism::COHERENT_OSCILLATION 
        || particle.IsOscillating
        || ( 3. * currentPoint_.Hubble >= particle.Mass )
    ){
        return;
    }
    
    connection_.Log.Info("Oscillation temp for " + particle.ParticleKey + ": " + boost::lexical_cast<std::string>(currentPoint_.Temperature) + " GeV");

    particle.IsActive = true;
    particle.IsOscillating = true;

    // With our defn of Y1 = log(n/s0), just gets rid of initial entropy to recover n (Entropy / Comoving Volume = entropy density)
    particle.NumberDensity = reheatPoint_.Entropy * expl( particle.Y1 );
    // With our defn of Y2 = log( rho / (n*T_R) ), this gives us rho/n so that we can combine with n to get rho
    particle.EnergyDensity = reheatPoint_.Temperature * expl( particle.Y2 ) * particle.NumberDensity;

    Models::TempOscillation tempOsc( currentPoint_, particle.ParticleId );
    auto statement = Statements::TempOsc( tempOsc, Statements::StatementType::Create );
    tempOscStatements_.push_back( std::make_shared< Statements::TempOsc >( statement ) );
    forcePost_ = true;
}

void BoltzmannStepBuilderCommand::checkDecayTransition( ParticleData& particle ){
    // we only care about thermal/coh.osc. particles, and must be active
    if ( particle.ProductionMechanism == ParticleProductionMechanism::RADIATION || !particle.IsActive ){
        return;
    }

    if ( 
        !particle.IsDecaying
        && particle.EnergyDensity > 0.
        && particle.NumberDensity > 0.
        &&( particle.TotalWidth > currentPoint_.Hubble )
    ){
        particle.IsDecaying = true;
        Models::TempDecay tD( currentPoint_, particle.ParticleId );
        auto tempDecayStatement = Statements::TempDecay( tD, Statements::StatementType::Create );
        tempDecayStatements_.push_back( std::make_shared< Statements::TempDecay >( tempDecayStatement ) );
        connection_.Log.Info("Decay temperature for " + particle.ParticleKey + ": " + boost::lexical_cast<std::string>(tD.Temperature) + " GeV");
        forcePost_ = true;
    }
    // Here's a hacky way to turn off fields so we don't get weird numerical issues
    // Since Hubble rate vs Total Width determines dominant interaction, make sure decay is very dominant
    double numericalCutoff = 1.e-50;
    if ( ( particle.TotalWidth > 1000. * currentPoint_.Hubble
          || particle.NumberDensity < numericalCutoff
          || particle.EnergyDensity < numericalCutoff )
        && particle.IsDecaying
    ){
        long double rrMin = -100.;
        for (auto& p2 : currentParticleData_){
            if (
                p2.ProductionMechanism != ParticleProductionMechanism::RADIATION 
                && p2.IsActive 
                && p2.EnergyDensity > 0. 
                && p2.NumberDensity > 0.
                && ( p2.ParticleEvolutionId != particle.ParticleEvolutionId )
            ){
                rrMin = max( rrMin, log( particle.EnergyDensity / p2.EnergyDensity ) );
            }
        }
        if (rrMin < -7. || ( particle.NumberDensity < numericalCutoff && particle.EnergyDensity < numericalCutoff )){
            particle.IsActive = false;
            particle.NumberDensity = 0.;
            particle.EnergyDensity = 0.;
        }
    }
}

// This method adds up the RHS of the relevant terms in the Boltzmann equations, and gives the state vector dxdt
// The numerical integrator then solves the dxdt for the given step
void BoltzmannStepBuilderCommand::addComponents(){
    dxdt_[0] = 0.;

    auto rad = currentParticleData_.front();
    if ( rad.ProductionMechanism != ParticleProductionMechanism::RADIATION ){
        throw_with_trace( logic_error("Evolution deque corrupted") );
    }

    BoltzmannData data( currentParticleData_, currentPoint_, reheatPoint_, particles_, partialWidths_ );
    BoltzmannBuilder b(connection_, data);

    int i = 0;

    for( auto& p : currentParticleData_ ){
        // we already set radiation
        if ( p.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
            i++;
            continue;
        }

        auto builder = b.Build_Particle_Boltzmann_Eqs( t_, p, rad );
        dxdt_[ 2*i ] += builder.NumberDensityEquation;
        dxdt_[ 2*i + 1] += builder.EnergyDensityEquation;
        dxdt_[0] += builder.EntropyEquation;

        double logCutoff = 1.e20;
        if ( std::abs(dxdt_[2*i]) > logCutoff || std::abs(dxdt_[2*i+1]) > logCutoff || dxdt_[0] > logCutoff ){
            connection_.Log.Info("BAD POINT for " + p.ParticleKey);
            connection_.Log.Info("dxdt[2i]: " + boost::lexical_cast<std::string>(dxdt_[2*i]));
            connection_.Log.Info("dxdt[2i+1]: " + boost::lexical_cast<std::string>(dxdt_[2*i+1]));
            connection_.Log.Info("dxdt[0]: " + boost::lexical_cast<std::string>(dxdt_[0]));

        }

        for (int j = 0; j < currentParticleData_.size(); ++j){
            jac_( 0, 2*j ) = builder.EntropyJacobian[2*j];
            jac_( 0, 2*j+1 ) = builder.EntropyJacobian[2*j+1];
            jac_( 2*i, 2*j ) = builder.NumberDensityJacobian[2*j];
            jac_( 2*i, 2*j+1 ) = builder.NumberDensityJacobian[2*j+1];
            jac_( 2*i+1, 2*j+1 ) = builder.EnergyDensityJacobian[2*j+1]; // 2j???
            jac_( 2*i+1, 2*j ) = builder.EnergyDensityJacobian[2*j]; // 2j+1???
            
            if ( std::abs(jac_(0,2*j)) > logCutoff || std::abs(jac_(0,2*j+1)) > logCutoff ){
                connection_.Log.Info("BAD ENTROPY JACOBIAN for " + p.ParticleKey);
                connection_.Log.Info("jac(0, 2j): " + boost::lexical_cast<std::string>(jac_(0,2*j)));
                connection_.Log.Info("jac(0, 2j+1): " + boost::lexical_cast<std::string>(jac_(0,2*j+1)));
            }
            if ( std::abs(jac_(2*i,2*j)) > logCutoff || std::abs(jac_(2*i,2*j+1)) > logCutoff ){
                connection_.Log.Info("BAD NUMBER DENSITY JACOBIAN for " + p.ParticleKey);
                connection_.Log.Info("jac(2i, 2j): " + boost::lexical_cast<std::string>(jac_(2*i,2*j)));
                connection_.Log.Info("jac(2i, 2j+1): " + boost::lexical_cast<std::string>(jac_(2*i,2*j+1)));
            }
            if ( std::abs(jac_(2*i+1,2*j)) > logCutoff || std::abs(jac_(2*i+1,2*j+1)) > logCutoff ){
                connection_.Log.Info("BAD ENERGY DENSITY JACOBIAN for " + p.ParticleKey);
                connection_.Log.Info("jac(2i+1, 2j): " + boost::lexical_cast<std::string>(jac_(2*i+1,2*j)));
                connection_.Log.Info("jac(2i+1, 2j+1): " + boost::lexical_cast<std::string>(jac_(2*i+1,2*j+1)));
            }            
        }

        i++;
    }
}


/* 

    Methods used for posting data 

*/

void BoltzmannStepBuilderCommand::postCrossSection( const ParticleData& particle ){
    SigmaV c;
    c.InputId = particle.InputId;
    c.ParticleId = particle.ParticleId;
    c.ScaleFactorId = particle.ScaleFactorId;
    c.Temperature = currentPoint_.Temperature;
    c.CrossSection = particle.AnnihilationCrossSection;

    auto statement = Statements::CrossSection( c, Statements::Create );
    crossSectionStatements_.push_back( std::make_shared< Statements::CrossSection >( statement ) );
}

/* 

    Public methods

*/

void BoltzmannStepBuilderCommand::Execute()
{
    auto startTime = std::chrono::steady_clock::now();
    forcePost_ = false;
    // based on results of previous step, set the current point
    currentPoint_ = setCurrentPoint();

    // here we set the initial states 
    setEvolutionInitialState();

    // Hubble parameter depends on energy densities, which are calculated in setEvolutionInitialState method
    calculateHubble();

    // add current point to tentative data to post
    sqlDataToPost_.ScaleFactors.push_front( currentPoint_ );

    // now take into account temperature-dependent initial conditions (e.g. axion coh. osc. initial densities)
    handleTemperatureDependences();

    // now we want to check if any major transitions happen
    // must happen after Hubble is calculated since transitions typically depend on H
    checkTransitions();

    for (auto& p : currentParticleData_){
        sqlDataToPost_.ParticleDatas.push_front( p );
    }

    auto midTimer = std::chrono::steady_clock::now();

    // now that all the necessary data for the step is calculated, we can assemble the Boltzmann equations
    addComponents();
    auto endTimer = std::chrono::steady_clock::now();
   
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(midTimer - startTime);
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(endTimer - midTimer);
    
//    connection_.Log.Info( "start timer: " + std::to_string(duration1.count()) );
//    connection_.Log.Info( "end timer: " + std::to_string( duration2.count() ) );
}

bool BoltzmannStepBuilderCommand::Exit(){
    if ( currentPoint_.Temperature <= 1.e-8 ){
        return true;
    }
    return false;
}

boost::numeric::ublas::vector<long double> BoltzmannStepBuilderCommand::getDxDt(){
    return dxdt_;
}

boost::numeric::ublas::matrix<long double> BoltzmannStepBuilderCommand::getJacobian(){
    return jac_;
}

void BoltzmannStepBuilderCommand::Reset(){
    isInitialized_ = false;
}

bool BoltzmannStepBuilderCommand::IsInitialized(){
    return isInitialized_;
}

int BoltzmannStepBuilderCommand::getCurrentOrdinal(){
    return ordinal_;
}

void BoltzmannStepBuilderCommand::resetCurrentOrdinal(int ordinal){
    ordinal_ = ordinal;
}

bool BoltzmannStepBuilderCommand::ForcePost(){
    return forcePost_;
}

void BoltzmannStepBuilderCommand::cleanScaleFactorData(){
    sqlDataToPost_.ScaleFactors.erase( 
        std::remove_if(
            sqlDataToPost_.ScaleFactors.begin(),
            sqlDataToPost_.ScaleFactors.end(),
            [this]( const Models::ScaleFactorPoint& s ){
                return s.Id != currentPoint_.Id;
            }
        ),
        sqlDataToPost_.ScaleFactors.end() 
    );
}

void BoltzmannStepBuilderCommand::cleanParticleData(){
    sqlDataToPost_.ParticleDatas.erase( 
        std::remove_if(
            sqlDataToPost_.ParticleDatas.begin(),
            sqlDataToPost_.ParticleDatas.end(),
            [this]( const ParticleData& p ){
                return p.ScaleFactorId != currentPoint_.Id;
            }
        ),
        sqlDataToPost_.ParticleDatas.end() 
    );
}

void BoltzmannStepBuilderCommand::Post(){
    if ( evoStatements_.size() > 0 ){
        db_.Execute( evoStatements_ );
        evoStatements_.clear();
    }
    if ( particleStatements_.size() > 0 ){
        db_.Execute( particleStatements_ );
        particleStatements_.clear();
    }
    if ( scaleFactorStatements_.size() > 0 ){
        db_.Execute( scaleFactorStatements_ );
        scaleFactorStatements_.clear();
    }
    if ( updateScaleFactorStatements_.size() > 0 ){
        for ( auto& s : updateScaleFactorStatements_ ){
            db_.Execute( *s );
        }
        updateScaleFactorStatements_.clear();
    }
    if ( tempOscStatements_.size() > 0 ){
        db_.Execute( tempOscStatements_ );
        tempOscStatements_.clear();
    }
    if ( tempDecayStatements_.size() > 0 ){
        db_.Execute( tempDecayStatements_ );
        tempDecayStatements_.clear();
    }
    if ( crossSectionStatements_.size() > 0 ){
        db_.Execute( crossSectionStatements_ );
        crossSectionStatements_.clear();
    }
}

void BoltzmannStepBuilderCommand::SetResult(){
    auto startTimer = std::chrono::steady_clock::now();
    cleanScaleFactorData();
    cleanParticleData();

    if ( sqlDataToPost_.ScaleFactors.size() == 1 ){
        auto statement1 = Statements::ScaleFactor( sqlDataToPost_.ScaleFactors.front(), Statements::StatementType::Create );
        scaleFactorStatements_.push_back( 
            std::make_shared< Statements::ScaleFactor >( statement1 ) 
        );
    } else if ( currentPoint_.Ordinal != 0 ){
        connection_.Log.Info("ScaleFactors size: " + to_string( sqlDataToPost_.ScaleFactors.size() ));
        throw logic_error("Did not sanitize scale factors correctly");
    }

    bool addedAxion = false;

    for ( auto& p : sqlDataToPost_.ParticleDatas ){
        if ( p.ScaleFactorId != currentPoint_.Id ){
            throw logic_error("Did not sanitize particles correctly");
        }

        if (p.ProductionMechanism == ParticleProductionMechanism::THERMAL){
            postCrossSection( p );
        }

        Models::ParticleEvolution pEvo;
        pEvo.Id = p.ParticleEvolutionId;
        pEvo.InputId = p.InputId;
        pEvo.ParticleId = p.ParticleId;
        pEvo.ScaleFactorId = p.ScaleFactorId;

        pEvo.IsActive = p.IsActive;
        pEvo.IsDecaying = p.IsDecaying;
        pEvo.IsOscillating = p.IsOscillating;
        pEvo.NumberDensity = p.NumberDensity;
        pEvo.EquilibriumNumberDensity = p.EquilibriumNumberDensity;
        pEvo.EnergyDensity = p.EnergyDensity;

        pEvo.ParticleKey = p.ParticleKey;
        pEvo.ProductionMechanism = p.ProductionMechanism;
        pEvo.Y1 = p.Y1;
        pEvo.Y2 = p.Y2;

        evoStatements_.push_back( 
            std::make_shared< Statements::BoltzmannParticleEvolution>( pEvo, Statements::StatementType::Create ) 
        );

        // TODO: FIX SECOND CONDITION!!! 
        // IT'S A BAD HACK TO AVOID ADDING TWO VERSIONS OF SAME PARTICLE IF BOTH COH.OSC. AND THERMAL ARE ENABLED
        // ONLY WORKS BECAUSE WE'RE ONLY CONSIDERING AXION TO BE TEMP-DEPENDENT
        if( p.TempDependentMass && !addedAxion ){
            Models::Particle particle;
            auto st = Statements::Particle( particle, Statements::StatementType::Read );
            auto fil = Filters::Particle( p.ParticleKey, p.InputId );
            st.AddFilter( fil );
            auto cb = Callbacks::Particle();
            db_.Execute( st, cb.Callback, cb.CallbackReturn );
            if ( cb.CallbackReturn.Particles.size() == 0 ){
                throw_with_trace( logic_error("Could not pull particle to update") );
            }
            particle = cb.CallbackReturn.Particles.front();
            particle.Id = p.ParticleId;
            particle.Mass = p.Mass;
            particle.ScaleFactorId = p.ScaleFactorId;

            particleStatements_.push_back( 
                std::make_shared< Statements::Particle >( particle, Statements::StatementType::Create ) 
            );
            addedAxion = true;
        }
    }

    if (currentPoint_.Ordinal == 0){
        resetParticleData();
    }
    auto endTimer = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTimer - startTimer);

    connection_.Log.Info( "post timer: " + std::to_string(duration.count() ) );
}

void BoltzmannStepBuilderCommand::UpdateData( const state_type& x, const double& t ){
    if (!isInitialized_){
        // increment the ordinal on each step (step 0 is initial condition, already calculated)
        ordinal_ += 1;
        x_ = x;
        t_ = t;

        if (dxdt_.size() != x_.size()){
            dxdt_.resize( x_.size() );
        }
        if (jac_.size1() != x_.size() || jac_.size2() != x_.size()){
            jac_.resize( x_.size(), x_.size() );
        } 

        isInitialized_ = true;
    }
}

void BoltzmannStepBuilderCommand::UpdateScaleFactor(){
    currentPoint_.Ordinal = ordinal_;
    if ( sqlDataToPost_.ScaleFactors.size() == 1 ){
        sqlDataToPost_.ScaleFactors[0].Ordinal = ordinal_;
    }

    auto update = Statements::ScaleFactor( currentPoint_, Statements::StatementType::Update );
    auto updateFilter = Filters::ScaleFactor( currentPoint_.Id, Filters::WhereUUID::Id );
    update.AddFilter( updateFilter );
    updateScaleFactorStatements_.push_back( std::make_shared< Statements::ScaleFactor >( update ) );
}
