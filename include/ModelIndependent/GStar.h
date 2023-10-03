#ifndef GStar_h
#define GStar_h

#include <cmath>
#include <deque>
#include <vector>
#define BOOST_ALLOW_DEPRECATED_HEADERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <Models/ModelBase.h>
#include <Models/Particle.h>
#include <Models/Connection.h>
#include <sql/DbManager.h>
#include <sql/Statements/Particle.h>
#include <sql/Filters/Particle.h>
#include <sql/Callbacks/Particle.h>

class GStar
{
private:
    static Models::Particle findParticle( const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, std::string key );
    typedef std::vector<double> state_type;
    typedef boost::numeric::odeint::runge_kutta4<state_type> error_stepper_type;
    static double integralEval(double numFactor, double denomFactor, double t0, double tf, double dt);
    static double defaultIntegralEval(const double& numFactor, const double& denomFactor);
    static double calculateSusy(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, const double& T, const CosmologicalTemperatures& temperatures);
    static double calculateLeptons(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, const double& T, const CosmologicalTemperatures& temperatures);
    static double calculateQCD(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, const double& T, const CosmologicalTemperatures& temperatures);
    static double calculateGaugeBosons(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, const double& T, const CosmologicalTemperatures& temperatures);
public:
    static double Calculate(const ModelBase& model, double T);
    static double CalculateEntropic(const ModelBase& model, double T);
    static double Calculate(Connection& connection, double T);
    static double CalculateEntropic(Connection& connection, double T);
    static double Calculate(DbManager& db, Connection& connection, double T);
    static double CalculateEntropic(DbManager& db, Connection& connection, double T, double gstr);
    static double Calculate(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, Connection& connection, double T);
    static double CalculateEntropic(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, Connection& connection, double T);
    static double CalculateEntropic(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, Connection& connection, double T, double gstr);
    GStar(/* args */);
    ~GStar();
};

#endif
