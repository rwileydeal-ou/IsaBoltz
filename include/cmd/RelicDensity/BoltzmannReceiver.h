#ifndef BoltzmannRelicDensityReceiver_h
#define BoltzmannRelicDensityReceiver_h

#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/ParticleEvolution.h>
#include <Models/ScaleFactor.h>
#include <Models/RelicDensity.h>
#include <sql/DbManager.h>
#include <cmd/RelicDensity/Receiver.h>
#include <ModelIndependent/GStar.h>

class BoltzmannRelicDensityReceiver : public RelicDensityReceiver
{
private:
    double currentTemperature_;
    double currentGStarEntropic_;
    double initialTemperature_;
    double initialGStarEntropic_;
    double rhoCriticalh2_;
    double calculateRelicDensity() override;
    double cohosc_relicDensity();
    double thermal_relicDensity();
    double radiation_relicDensity();

    // these are used for numerical integration, to be revised later
    typedef std::vector<double> state_type;
    typedef boost::numeric::odeint::runge_kutta_cash_karp54<state_type> error_stepper_type;
    double getRhoNf(double mass, double rhoN, double x0, double xF);
public:
    ~BoltzmannRelicDensityReceiver();
    BoltzmannRelicDensityReceiver( Connection& connection, const Models::ParticleEvolution& particleEvo, const Models::Particle& particle, const Models::ScaleFactorPoint& finalScaleFactorPoint );
};

#endif
