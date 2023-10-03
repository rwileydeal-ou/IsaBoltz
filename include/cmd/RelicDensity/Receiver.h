#ifndef RelicDensityReceiver_h
#define RelicDensityReceiver_h

#include <iostream>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <Models/ParticleEvolution.h>
#include <Models/ScaleFactor.h>
#include <Models/RelicDensity.h>
#include <sql/DbManager.h>

class RelicDensityReceiver : public IReceiver
{
private: 
protected:
    Connection& connection_;
    Models::ParticleEvolution particleEvo_;
    Models::Particle particle_;
    Models::ScaleFactorPoint finalScaleFactorPoint_;
    Models::RelicDensity relicDensity_;
    virtual double calculateRelicDensity() = 0;
public:
    ~RelicDensityReceiver();
    RelicDensityReceiver( Connection& connection, const Models::ParticleEvolution& particleEvo, const Models::Particle& particle, const Models::ScaleFactorPoint& finalScaleFactorPoint );
    void Calculate() override;
    Models::RelicDensity getRelicDensity();
};

#endif
