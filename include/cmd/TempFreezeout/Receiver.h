#ifndef TempFreezeoutReceiver_h
#define TempFreezeoutReceiver_h

#include <iostream>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>

class TempFreezeoutReceiver : public IReceiver
{
private: 
    Connection& connection_;
    Models::Particle& particle_;
    double tempFreezeout_;
public:
    ~TempFreezeoutReceiver();
    TempFreezeoutReceiver(Connection& connection, Models::Particle& particle);
    void Calculate() override;
    double getTempFreezeout();
};

#endif
