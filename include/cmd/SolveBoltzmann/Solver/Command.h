#ifndef BoltzmannSolverCommand_h
#define BoltzmannSolverCommand_h

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta4.hpp>
#include <boost/numeric/odeint/stepper/rosenbrock4.hpp>

#include <cmd/SolveBoltzmann/Solver/RadauDense.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <memory>
#include <unordered_map>

#include <client/Sender.h>
#include <cmd/ICommand.h>
#include <cmd/SolveBoltzmann/StepBuilder/Command.h>

#include <io/Exceptions/NotImplemented.h>
#include <Models/ScaleFactor.h>
#include <Models/Connection.h>
#include <sql/Statements/ScaleFactor.h>
#include <sql/Filters/ScaleFactor.h>
#include <sql/Callbacks/ScaleFactor.h>
#include <sql/Statements/PartialWidth.h>
#include <sql/Filters/PartialWidth.h>
#include <sql/Callbacks/PartialWidth.h>
#include <sql/Statements/TotalWidth.h>
#include <sql/Filters/TotalWidth.h>
#include <sql/Callbacks/TotalWidth.h>
#include <cmd/SolveBoltzmann/StepInfo.h>
#include <cmd/SolveBoltzmann/Solver/System.h>
#include <cmd/SolveBoltzmann/Solver/SystemJacobian.h>
#include <cmd/SolveBoltzmann/Solver/Observer.h>
#include <cmd/RelicDensity/Command.h>
#include <cmd/SolveBoltzmann/StepBuilder/GStarSpline.h>

class BoltzmannSolverCommand : public ICommand {
private:
    Connection& connection_;
    DbManager& db_;
    std::shared_ptr< DataRelay > fortranInterface_;
    Models::ScaleFactorPoint reheatPoint_;
    double finalTemp_;
    std::vector< std::string > enabledKeys_;
    Models::ScaleFactorPoint pullReheatScaleFactorPoint(
        boost::uuids::uuid scaleFactorId
    );

    typedef boost::numeric::ublas::vector<long double> state_type;
    typedef boost::numeric::odeint::rosenbrock4 <state_type> error_stepper_type;

    Models::ParticleEvolution pullParticleEvolution( 
        std::string particleKey, 
        ParticleProductionMechanism productionMechanism, 
        boost::uuids::uuid scaleFactorId 
    );
    std::map< 
        std::string, 
        std::vector< Models::PartialWidth > 
    > pullPartialWidths( 
        const std::vector< Models::ParticleEvolution >& particleEvos 
    );
    std::map< std::string, Models::TotalWidth > pullTotalWidths( 
        const std::vector< Models::ParticleEvolution >& particleEvos 
    );
    std::vector< Models::ParticleEvolution > pullParticleEvolutions();
    std::vector< Models::Particle > pullParticles(
        std::unordered_map< 
            boost::uuids::uuid, 
            Models::Particle, 
            boost::hash<boost::uuids::uuid> 
        >& particleCache
    );
    void purgeSQL(
        int startOrdinal, 
        int endOrdinal
    );
public:
    BoltzmannSolverCommand(
        Connection& connection, 
        DbManager& db_,
        std::shared_ptr< DataRelay > fortranInterface, 
        boost::uuids::uuid reheatScaleFactorId, 
        double finalTemp, 
        std::vector< std::string > enabledKeys
    );
    ~BoltzmannSolverCommand();
    void Execute() override;
};

#endif
