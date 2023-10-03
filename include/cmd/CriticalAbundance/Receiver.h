#ifndef CriticalAbundanceReceiver_h
#define CriticalAbundanceReceiver_h

#include <iostream>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <ModelIndependent/GStar.h>
#include <sql/DbManager.h>
#include <sql/Statements/CrossSection.h>
#include <sql/Callbacks/CrossSection.h>
#include <sql/Filters/CrossSection.h>

class CriticalAbundanceReceiver : public IReceiver
{
private: 
    Connection& connection_;
    SigmaV crossSection_;
    double criticalAbundance_;
    double criticalNumberDensity_;
public:
    ~CriticalAbundanceReceiver();
    CriticalAbundanceReceiver(Connection& connection);
    void Calculate() override;
    double getCriticalAbundance();
    double getCriticalNumberDensity();
};

#endif
