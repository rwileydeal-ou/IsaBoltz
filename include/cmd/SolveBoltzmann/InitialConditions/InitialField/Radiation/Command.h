#ifndef InitialRadiationCommand_h
#define InitialRadiationCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/SolveBoltzmann/InitialConditions/InitialField/Radiation/Receiver.h>
#include <Models/Particle.h>
#include <Models/ParticleEvolution.h>
#include <sql/DbManager.h>
#include <sql/Statements/BoltzmannParticleEvolution.h>
#include <sql/Statements/Particle.h>
#include <sql/Filters/Particle.h>
#include <sql/Callbacks/Particle.h>

class RadiationCommand : public ICommand {
private:
    Connection& connection_;
    DbManager& db_;
    std::shared_ptr< RadiationReceiver > receiver_;
    Models::Particle particle_;
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint_;
    void pullParticle();
    void postBoltzmannParticleEvolution( const Models::ParticleEvolution& particleEvolution);
public:
    RadiationCommand(
        Connection& connection, 
        DbManager& db,
        std::shared_ptr< Models::ScaleFactorPoint > initialPoint
    );
    ~RadiationCommand();
    void Execute() override;
};

#endif
