#ifndef TempEqualityCommand_h
#define TempEqualityCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/TempEquality/Receiver.h>
#include <cmd/TempEquality/CohOscReceiver.h>
#include <cmd/TempEquality/ThermalReceiver.h>
#include <Models/Particle.h>
#include <Models/Connection.h>
#include <sql/Statements/TempEquality.h>

class TempEqualityCommand : public ICommand {
private:
    Connection& connection_;
    std::shared_ptr< TempEqualityReceiver > receiver_;
    Models::Particle particle_;
    ParticleProductionMechanism production_;
public:
    TempEqualityCommand(Connection& connection, Models::Particle& particle, ParticleProductionMechanism productionMechanism);
    ~TempEqualityCommand();
    void Execute() override;
};

#endif
