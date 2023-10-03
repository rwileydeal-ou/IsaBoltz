#include <cmd/SolveBoltzmann/Solver/Command.h>

using namespace std;

BoltzmannSolverCommand::BoltzmannSolverCommand(Connection& connection, std::shared_ptr< DataRelay > fortranInterface, boost::uuids::uuid reheatScaleFactorId, double finalTemp, std::vector< std::string > enabledKeys) : 
    connection_(connection)
{
    reheatPoint_.Id = reheatScaleFactorId;
    finalTemp_ = finalTemp;
    fortranInterface_ = fortranInterface;
    enabledKeys_ = enabledKeys;
}

BoltzmannSolverCommand::~BoltzmannSolverCommand(){
}

// this method pulls the particle evolution object for an initial ScaleFactor
// must be unique
Models::ParticleEvolution BoltzmannSolverCommand::pullParticleEvolution( DbManager& db, std::string particleKey, ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId ){
    Models::ParticleEvolution p;
    auto statement = Statements::BoltzmannParticleEvolution( p, Statements::StatementType::Read );
    auto filter = Filters::ParticleEvolution( particleKey, productionMechanism, scaleFactorId );
    statement.AddFilter( filter );
    auto cb = Callbacks::ParticleEvolution();
    db.Execute( statement, cb.Callback, cb.CallbackReturn );

    if ( cb.CallbackReturn.ParticleEvolutions.size() != 1 ){
        throw_with_trace( logic_error("Could not find unique ParticleEvolution") );
    }
    return cb.CallbackReturn.ParticleEvolutions.front();
}

deque< Models::ParticleEvolution > BoltzmannSolverCommand::pullParticleEvolutions()
{
    deque < Models::ParticleEvolution > particleEvolutions;
    // define since we need to also add radiation (always needed, so not given as an option)
    vector<string> enabledFields = enabledKeys_;
    // we also insert the radiation key at the beginning, so we always know the radiation index as 0
    enabledFields.insert(enabledFields.begin(), "photon");

    // now we pull all the ParticleEvolutions for the initial condition ScaleFactorID
    DbManager db(connection_);
    db.Open();
    for (auto& key : enabledFields){
        auto keyComponents = FileIO::Split(key, "_");
        string particleKey = ( keyComponents.size() > 1 ) ? keyComponents[1] : keyComponents[0];
        ParticleProductionMechanism production;
        if (keyComponents[0] == "cohosc"){
            production = ParticleProductionMechanism::COHERENT_OSCILLATION;
        } else if (particleKey == "photon"){
            production = ParticleProductionMechanism::RADIATION;
        } else{
            production = ParticleProductionMechanism::THERMAL;
        }
        particleEvolutions.push_back( pullParticleEvolution( db, particleKey, production, reheatPoint_.Id ) );
    }
    db.Close();

    // we also need to do a validation that the "enabledFields" length is the same as the number of ParticleEvolutions pulled
    // otherwise something BAD happened
    if (particleEvolutions.size() != enabledFields.size()){
        throw_with_trace( logic_error("Mismatch error...") );
    }
    return particleEvolutions;
}

Models::ScaleFactorPoint BoltzmannSolverCommand::pullReheatScaleFactorPoint(boost::uuids::uuid scaleFactorId){
    Models::ScaleFactorPoint scaleFactor;
    DbManager db(connection_);
    db.Open();
    auto statement = Statements::ScaleFactor( scaleFactor, Statements::Read );
    auto filter = Filters::ScaleFactor( scaleFactorId, Filters::WhereUUID::Id );
    statement.AddFilter( filter );
    auto cb = Callbacks::ScaleFactor();
    db.Execute( statement, cb.Callback, cb.CallbackReturn );
    db.Close();

    // better be unique!
    if (cb.CallbackReturn.ScaleFactors.size() != 1){
        throw_with_trace( logic_error("Could not find unique ScaleFactor") );
    }

    scaleFactor = cb.CallbackReturn.ScaleFactors.front();
    return scaleFactor;
}

std::deque< Models::Particle, boost::pool_allocator<Models::Particle> > BoltzmannSolverCommand::pullParticles(){
    Models::Particle p;
    DbManager db(connection_);
    db.Open();
    auto statement = Statements::Particle(p, Statements::Read);
    auto filter = Filters::Particle( connection_.InputId, Filters::WhereUUID::InputId );
    statement.AddFilter( filter );
    auto cb = Callbacks::Particle();
    db.Execute(statement, cb.Callback, cb.CallbackReturn);
    db.Close();

    if (cb.CallbackReturn.Particles.size() == 0){
        throw_with_trace( logic_error("Could not find particles!") );
    }
    return cb.CallbackReturn.Particles;
}

std::map< std::string, std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > > BoltzmannSolverCommand::pullPartialWidths( const std::deque< Models::ParticleEvolution >& particleEvos ){
    std::map< std::string, std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > > partialWidths;
    DbManager db(connection_);
    db.Open();

    Models::PartialWidth p;
    for (auto& evo : particleEvos){
        auto statement = Statements::PartialWidth( p, Statements::StatementType::Read );
        auto filter = Filters::PartialWidth( connection_.InputId, evo.ParticleId );
        statement.AddFilter( filter );
        auto cb = Callbacks::PartialWidth();
        db.Execute( statement, cb.Callback, cb.CallbackReturn );

        partialWidths.insert( { evo.ParticleKey, cb.CallbackReturn.PartialWidths } );
    }
    db.Close();
    return partialWidths;
}

// use a map that uses particleKey to map to total width
std::map< std::string, Models::TotalWidth > BoltzmannSolverCommand::pullTotalWidths( const std::deque< Models::ParticleEvolution >& particleEvos ){
    std::map<std::string, Models::TotalWidth> totalWidths;
    DbManager db(connection_);
    db.Open();

    Models::TotalWidth p;
    for (auto& evo : particleEvos){
        if ( evo.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
            continue;
        }
        auto statement = Statements::TotalWidth( p, Statements::StatementType::Read );
        auto filter = Filters::TotalWidth( connection_.InputId, evo.ParticleId );
        statement.AddFilter( filter );
        auto cb = Callbacks::TotalWidth();
        db.Execute( statement, cb.Callback, cb.CallbackReturn );

        if ( cb.CallbackReturn.TotalWidths.size() != 1){
            throw_with_trace( logic_error("Could not find unique TotalWidth") );
        }
        totalWidths.insert( {evo.ParticleKey, cb.CallbackReturn.TotalWidths.front()} );
    }
    db.Close();
    return totalWidths;
}

void BoltzmannSolverCommand::Execute(){
    StepInfo step( 0., 37., 0.01, 1.0e-8, 1.0e-6 );

    // make sure we have correct reheat object 
    reheatPoint_ = pullReheatScaleFactorPoint( reheatPoint_.Id );

    // pull ParticleEvolution objects now to eliminate redundant SQL calls
    auto particleEvolutions = pullParticleEvolutions();
    auto particleDefs = pullParticles();
    auto partialWidths = pullPartialWidths( particleEvolutions );
    auto totalWidths = pullTotalWidths( particleEvolutions );

    // setting initial conditions here
    boost::numeric::ublas::vector< double> xInit(2 * particleEvolutions.size());
    for (int i = 0; i < particleEvolutions.size(); ++i){
        xInit (2*i) = particleEvolutions[i].Y1;
        xInit (2*i+1) = particleEvolutions[i].Y2;
    }

    auto stepperType = boost::numeric::odeint::rosenbrock4< double >();
    auto stepper = boost::numeric::odeint::make_dense_output(1.e-03, 1.e-03, stepperType);

    // delegate building the step to the proper command
    BoltzmannStepBuilderCommand stepSolver(
        connection_, 
        fortranInterface_,
        reheatPoint_, 
        particleEvolutions,
        particleDefs,
        partialWidths,
        totalWidths
    );

    System sys( stepSolver );
    SystemJacobian sysJac( stepSolver );
    BoltzmannObserver observer( stepSolver, connection_ );

    double T = step.T0;
    double dT = step.CurrentDeltaT;
    stepper.initialize( 
        xInit, 
        T, dT
    );
    observer.operator()( stepper.current_state(), stepper.current_time() );
    while (true){
        try{
            stepper.do_step( 
                make_pair( sys, sysJac )
            );
            observer.operator()( stepper.current_state(), stepper.current_time() );
            if ( stepSolver.Exit() ){
                break;
            }
        } catch( boost::numeric::ublas::internal_logic& e ){
            ostringstream oss;
            auto xI = stepper.current_state();
            for (auto& x : xI ){
                oss << x << std::endl;
            }
            connection_.Log.Error("Internal logic error encountered...");
            connection_.Log.Info("Current state: " + oss.str());
            throw_with_trace( e );
        }
    }
    stepSolver.Post();
    connection_.Log.Info("Finished solving Boltzmann equations");
}
