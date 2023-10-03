#ifndef DeltaNeffReceiver_h
#define DeltaNeffReceiver_h

#include <iostream>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <Models/ParticleEvolution.h>
#include <Models/ScaleFactor.h>
#include <Models/DeltaNeff.h>
#include <sql/DbManager.h>

class DeltaNeffReceiver : public IReceiver
{
private: 
protected:
    Connection& connection_;
    Models::ParticleEvolution particleEvo_;
    Models::DeltaNeff deltaNeff_;
    // overrides of this method should return DeltaNeff for a species
    virtual double calculateDeltaNeff() = 0;
public:
    ~DeltaNeffReceiver();
    DeltaNeffReceiver( Connection& connection, const Models::ParticleEvolution& particleEvo );
    void Calculate() override;
    Models::DeltaNeff getDeltaNeff();
};

#endif
