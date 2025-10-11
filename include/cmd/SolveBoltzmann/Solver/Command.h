#ifndef BoltzmannSolverCommand_h
#define BoltzmannSolverCommand_h

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta4.hpp>
#include <boost/numeric/odeint/stepper/rosenbrock4.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <memory>

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

class BoltzmannSolverCommand : public ICommand {
private:
    Connection& connection_;
    std::shared_ptr< DataRelay > fortranInterface_;
    Models::ScaleFactorPoint reheatPoint_;
    double finalTemp_;
    std::vector< std::string > enabledKeys_;
    Models::ScaleFactorPoint pullReheatScaleFactorPoint(boost::uuids::uuid scaleFactorId);

    typedef boost::numeric::ublas::vector<long double> state_type;
    typedef boost::numeric::odeint::rosenbrock4 <state_type> error_stepper_type;

    Models::ParticleEvolution pullParticleEvolution( DbManager& db, std::string particleKey, ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId );
    std::map< std::string, std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > > pullPartialWidths( const std::deque< Models::ParticleEvolution >& particleEvos );
    std::map< std::string, Models::TotalWidth > pullTotalWidths( const std::deque< Models::ParticleEvolution >& particleEvos );
    std::deque< Models::ParticleEvolution > pullParticleEvolutions();
    std::deque< Models::Particle, boost::pool_allocator<Models::Particle> > pullParticles();
    void purgeSQL(int startOrdinal, int endOrdinal);
public:
    BoltzmannSolverCommand(Connection& connection, std::shared_ptr< DataRelay > fortranInterface, boost::uuids::uuid reheatScaleFactorId, double finalTemp, std::vector< std::string > enabledKeys);
    ~BoltzmannSolverCommand();
    void Execute() override;
};

#endif
