#ifndef InitialThermalMatterCommand_h
#define InitialThermalMatterCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/SolveBoltzmann/InitialConditions/InitialField/ThermalMatter/Receiver.h>
#include <Models/Particle.h>
#include <Models/ParticleEvolution.h>
#include <sql/DbManager.h>
#include <sql/Statements/BoltzmannParticleEvolution.h>

class ThermalMatterCommand : public ICommand {
private:
    Connection& connection_;
    std::shared_ptr< ThermalMatterReceiver > receiver_;
    Models::Particle particle_;
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint_;
    void postBoltzmannParticleEvolution( const Models::ParticleEvolution& particleEvolution);
public:
    ThermalMatterCommand(Connection& connection, const Models::Particle& particle, std::shared_ptr< Models::ScaleFactorPoint > initialPoint);
    ~ThermalMatterCommand();
    void Execute() override;
};

#endif
