#ifndef FreezeoutAbundanceReceiver_h
#define FreezeoutAbundanceReceiver_h

#include <iostream>
#include <cmath>
#include <cmd/IReceiver.h>
#include <sql/DbManager.h>
#include <sql/Callbacks/CrossSection.h>
#include <sql/Statements/CrossSection.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <ModelIndependent/GStar.h>

class FreezeoutAbundanceReceiver : public IReceiver
{
private: 
    Connection& connection_;
    DbManager& db_;
    Models::Particle& particle_;
    boost::uuids::uuid scaleFactorId_;
    double freezeoutAbundance_;
    double freezeoutNumberDensity_;
    SigmaV getCrossSection();
public:
    ~FreezeoutAbundanceReceiver();
    FreezeoutAbundanceReceiver(
        Connection& connection, 
        DbManager& db,
        Models::Particle& particle, 
        boost::uuids::uuid scaleFactorId
    );
    void Calculate() override;
    double getFreezeoutAbundance();
    double getFreezeoutNumberDensity();
};

#endif
