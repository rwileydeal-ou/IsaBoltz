#include <cmd/SolveBoltzmann/Solver/Command.h>

using namespace std;

BoltzmannSolverCommand::BoltzmannSolverCommand(
    Connection& connection, 
    DbManager& db,
    std::shared_ptr< DataRelay > fortranInterface, 
    boost::uuids::uuid reheatScaleFactorId, 
    double finalTemp, 
    std::vector< std::string > enabledKeys
) : 
    connection_(connection),
    db_(db)
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
Models::ParticleEvolution BoltzmannSolverCommand::pullParticleEvolution( 
    std::string particleKey, 
    ParticleProductionMechanism productionMechanism, 
    boost::uuids::uuid scaleFactorId 
){
    Models::ParticleEvolution p;
    auto statement = Statements::BoltzmannParticleEvolution( p, Statements::StatementType::Read );
    auto filter = Filters::ParticleEvolution( 
        particleKey, 
        productionMechanism, 
        scaleFactorId, 
        Filters::WhereUUID::ScaleFactorId 
    );
    statement.AddFilter( filter );
    auto cb = Callbacks::ParticleEvolution();
    db_.Execute( statement, cb.Callback, cb.CallbackReturn );

    if ( cb.CallbackReturn.ParticleEvolutions.size() != 1 ){
        throw_with_trace( logic_error("Could not find unique ParticleEvolution") );
    }
    return cb.CallbackReturn.ParticleEvolutions.front();
}

vector< Models::ParticleEvolution > BoltzmannSolverCommand::pullParticleEvolutions()
{
    vector < Models::ParticleEvolution > particleEvolutions;
    // define since we need to also add radiation (always needed, so not given as an option)
    vector<string> enabledFields = enabledKeys_;
    // we also insert the radiation key at the beginning, so we always know the radiation index as 0
    enabledFields.insert(enabledFields.begin(), "photon");

    // now we pull all the ParticleEvolutions for the initial condition ScaleFactorID
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
        particleEvolutions.push_back( 
            pullParticleEvolution( particleKey, production, reheatPoint_.Id ) 
        );
    }

    // we also need to do a validation that the "enabledFields" length is the same as the number of ParticleEvolutions pulled
    // otherwise something BAD happened
    if (particleEvolutions.size() != enabledFields.size()){
        throw_with_trace( logic_error("Mismatch error...") );
    }
    return particleEvolutions;
}

Models::ScaleFactorPoint BoltzmannSolverCommand::pullReheatScaleFactorPoint(
    boost::uuids::uuid scaleFactorId
){
    Models::ScaleFactorPoint scaleFactor;
    auto statement = Statements::ScaleFactor( scaleFactor, Statements::Read );
    auto filter = Filters::ScaleFactor( scaleFactorId, Filters::WhereUUID::Id );
    statement.AddFilter( filter );
    auto cb = Callbacks::ScaleFactor();
    db_.Execute( statement, cb.Callback, cb.CallbackReturn );

    // better be unique!
    if (cb.CallbackReturn.ScaleFactors.size() != 1){
        throw_with_trace( logic_error("Could not find unique ScaleFactor") );
    }

    scaleFactor = cb.CallbackReturn.ScaleFactors.front();
    return scaleFactor;
}

std::vector< Models::Particle > BoltzmannSolverCommand::pullParticles(
    std::unordered_map< 
        boost::uuids::uuid, 
        Models::Particle, 
        boost::hash<boost::uuids::uuid> 
    >& particleCache
){
    Models::Particle p;
    auto statement = Statements::Particle(p, Statements::Read);
    auto filter = Filters::Particle( connection_.InputId, Filters::WhereUUID::InputId );
    statement.AddFilter( filter );
    auto cb = Callbacks::Particle();
    db_.Execute(statement, cb.Callback, cb.CallbackReturn);

    if (cb.CallbackReturn.Particles.size() == 0){
        throw_with_trace( logic_error("Could not find particles!") );
    }

    for (auto& part : cb.CallbackReturn.Particles){
    }

    return cb.CallbackReturn.Particles;
}

std::map< std::string, std::vector< Models::PartialWidth > > BoltzmannSolverCommand::pullPartialWidths( 
    const std::vector< Models::ParticleEvolution >& particleEvos 
){
    std::map< std::string, std::vector< Models::PartialWidth > > partialWidths;

    Models::PartialWidth p;
    for (auto& evo : particleEvos){
        auto statement = Statements::PartialWidth( p, Statements::StatementType::Read );
        auto filter = Filters::PartialWidth( connection_.InputId, evo.ParticleId );
        statement.AddFilter( filter );
        auto cb = Callbacks::PartialWidth();
        db_.Execute( statement, cb.Callback, cb.CallbackReturn );

        partialWidths.insert( { evo.ParticleKey, cb.CallbackReturn.PartialWidths } );
    }
    return partialWidths;
}

// use a map that uses particleKey to map to total width
std::map< std::string, Models::TotalWidth > BoltzmannSolverCommand::pullTotalWidths( 
    const std::vector< Models::ParticleEvolution >& particleEvos 
){
    std::map<std::string, Models::TotalWidth> totalWidths;

    Models::TotalWidth p;
    for (auto& evo : particleEvos){
        if ( evo.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
            continue;
        }
        auto statement = Statements::TotalWidth( p, Statements::StatementType::Read );
        auto filter = Filters::TotalWidth( connection_.InputId, evo.ParticleId );
        statement.AddFilter( filter );
        auto cb = Callbacks::TotalWidth();
        db_.Execute( statement, cb.Callback, cb.CallbackReturn );

        if ( cb.CallbackReturn.TotalWidths.size() != 1){
            throw_with_trace( logic_error("Could not find unique TotalWidth") );
        }
        totalWidths.insert( {evo.ParticleKey, cb.CallbackReturn.TotalWidths.front()} );
    }
    return totalWidths;
}

void BoltzmannSolverCommand::Execute(){
    StepInfo step( 0., 37., 1.e-5, 1.0e-8, 1.0e-6 );

    // make sure we have correct reheat object 
    reheatPoint_ = pullReheatScaleFactorPoint( reheatPoint_.Id );

    // construct hashmap
    std::unordered_map< 
        boost::uuids::uuid, 
        Models::Particle, 
        boost::hash<boost::uuids::uuid> 
    > particleCache;

    // pull ParticleEvolution objects now to eliminate redundant SQL calls
    auto particleEvolutions = pullParticleEvolutions();
    auto particleDefs = pullParticles(particleCache);
    auto partialWidths = pullPartialWidths( particleEvolutions );
    auto totalWidths = pullTotalWidths( particleEvolutions );

    // setting initial conditions here
    boost::numeric::ublas::vector< double> xInit(2 * particleEvolutions.size()-1);
    xInit(0) = particleEvolutions[0].Y1; // radiation is always the first element, regardless of input ordering
    for (size_t i = 1; i < particleEvolutions.size(); ++i){
        xInit (2*i-1) = particleEvolutions[i].Y1;
        xInit (2*i) = particleEvolutions[i].Y2;
    }

//    boost::numeric::odeint::radau5_dense_ext< double, boost::numeric::ublas::vector<double>, boost::numeric::ublas::matrix<double> >
//    baseStepper( /* rtol */ 1e-03, /* atol */ 1e-06 );

//    auto stepper = boost::numeric::odeint::make_dense_output( 1e-03, 1e-04, baseStepper );
    auto stepperType = boost::numeric::odeint::rosenbrock4< double >();
    auto stepper = boost::numeric::odeint::make_dense_output(1.e-03, 1.e-03, 0.1, stepperType);
//    auto stepper = boost::numeric::odeint::make_dense_output(1.e-04, 1.e-03, stepperType);

    GStarSpline::initialize(particleDefs, connection_);

    // delegate building the step to the proper command
    BoltzmannStepBuilderCommand stepSolver(
        connection_, 
        db_,
        fortranInterface_,
        reheatPoint_, 
        particleEvolutions,
        particleDefs,
        partialWidths,
        totalWidths,
        particleCache
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
                make_pair( 
                    sys, sysJac 
                )
            );

            stepper.initialize(
                stepSolver.UpdateInitialConditions(),
                stepper.current_time(),
                stepper.current_time_step()
            );

            observer.operator()( stepper.current_state(), stepper.current_time() );
            if ( stepSolver.Exit() ){
                break;
            }
        } catch( boost::numeric::ublas::internal_logic& e ){
            ostringstream oss;
            auto xI = stepper.previous_state();
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
