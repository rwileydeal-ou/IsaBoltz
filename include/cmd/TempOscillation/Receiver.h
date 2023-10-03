#ifndef TempOscillationReceiver_h
#define TempOscillationReceiver_h

#include <iostream>
#include <stdexcept>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <Models/TempOsc.h>
#include <ModelIndependent/GStar.h>

class TempOscillationReceiver : public IReceiver
{
private: 
    Connection& connection_;
    Models::Particle& particle_;
    Models::TempOscillation tempOscillation_;
    double tempReheat_;
    double gstar_;

    double tempOsc_lessThan_tempReheat(double initialGuess, double& gstar);
    double tempOsc_greaterThan_tempReheat(double initialGuess, double& gstar);    
public:
    ~TempOscillationReceiver();
    TempOscillationReceiver(Connection& connection, Models::Particle& particle, double tempReheat);
    void Calculate() override;
    Models::TempOscillation getTempOscillation();
};

#endif
