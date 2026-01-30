#ifndef InitialCohOscMatterCommand_h
#define InitialCohOscMatterCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/SolveBoltzmann/InitialConditions/InitialField/CohOscMatter/Receiver.h>
#include <Models/Particle.h>
#include <Models/ParticleEvolution.h>
#include <Models/TempOsc.h>
#include <sql/DbManager.h>
#include <sql/Statements/BoltzmannParticleEvolution.h>
#include <sql/Statements/TempOsc.h>

class CohOscMatterCommand : public ICommand {
private:
    Connection& connection_;
    DbManager& db_;
    std::shared_ptr< CohOscMatterReceiver > receiver_;
    Models::Particle particle_;
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint_;
    void postBoltzmannParticleEvolution( const Models::ParticleEvolution& particleEvolution);
public:
    CohOscMatterCommand(
        Connection& connection, 
        DbManager& db,
        const Models::Particle& particle, 
        std::shared_ptr< Models::ScaleFactorPoint > initialPoint
    );
    ~CohOscMatterCommand();
    void Execute() override;
};

#endif
