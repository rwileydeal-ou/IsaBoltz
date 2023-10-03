#ifndef BoltzmannDeltaNeffReceiver_h
#define BoltzmannDeltaNeffReceiver_h

#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/ParticleEvolution.h>
#include <Models/ScaleFactor.h>
#include <Models/DeltaNeff.h>
#include <sql/DbManager.h>
#include <cmd/DeltaNeff/Receiver.h>
#include <ModelIndependent/GStar.h>

class BoltzmannDeltaNeffReceiver : public DeltaNeffReceiver
{
private:
    Models::Particle particle_;
    Models::ScaleFactorPoint finalScaleFactorPoint_;
    double calculateDeltaNeff() override;
public:
    ~BoltzmannDeltaNeffReceiver();
    BoltzmannDeltaNeffReceiver( Connection& connection, const Models::ParticleEvolution& particleEvo, const Models::Particle& particle, const Models::ScaleFactorPoint& finalScaleFactorPoint );
};

#endif
