#ifndef TempEqualityReceiver_h
#define TempEqualityReceiver_h

#include <iostream>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <Models/TempEquality.h>
#include <io/Exceptions/NotImplemented.h>
#include <ModelIndependent/GStar.h>

class TempEqualityReceiver : public IReceiver
{
private: 
    Connection& connection_;
    Models::Particle& particle_;
protected:
    Models::TempEquality tempEquality_;
    double tempReheat_;
    virtual double tempEquality() = 0;
public:
    ~TempEqualityReceiver();
    TempEqualityReceiver(Connection& connection, Models::Particle& particle, double tempReheat);
    void Calculate() override;
    Models::TempEquality getTempEquality();
};

#endif
