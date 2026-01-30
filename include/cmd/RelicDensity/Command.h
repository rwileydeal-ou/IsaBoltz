#ifndef RelicDensityCommand_h
#define RelicDensityCommand_h

#include <memory>
#include <io/FileIO.h>
#include <cmd/ICommand.h>
#include <cmd/RelicDensity/Receiver.h>
#include <cmd/RelicDensity/BoltzmannReceiver.h>
#include <Models/Particle.h>
#include <Models/ParticleEvolution.h>
#include <Models/ScaleFactor.h>
#include <Models/Connection.h>
#include <Models/RelicDensity.h>
#include <sql/DbManager.h>
#include <sql/Statements/Particle.h>
#include <sql/Filters/Particle.h>
#include <sql/Callbacks/Particle.h>
#include <sql/Statements/BoltzmannParticleEvolution.h>
#include <sql/Filters/BoltzmannParticleEvolution.h>
#include <sql/Callbacks/BoltzmannParticleEvolution.h>
#include <sql/Statements/ScaleFactor.h>
#include <sql/Filters/ScaleFactor.h>
#include <sql/Callbacks/ScaleFactor.h>
#include <sql/Statements/RelicDensity.h>

class RelicDensityCommand : public ICommand {
private:
    Connection& connection_;
    DbManager& db_;
    std::shared_ptr< RelicDensityReceiver > receiver_;
    Models::ParticleEvolution particleEvo_;
    Models::Particle particle_;
    Models::ScaleFactorPoint finalScaleFactorPoint_;
    void postRelicDensity( const Models::RelicDensity& relicDensity );
public:
    RelicDensityCommand(
        Connection& connection, 
        DbManager& db,
        const Models::ParticleEvolution& particleEvo, 
        const Models::Particle& particle, 
        const Models::ScaleFactorPoint& finalScaleFactorPoint 
    );
    RelicDensityCommand(
        Connection& connection, 
        DbManager& db,
        std::string enabledKey
    );
    ~RelicDensityCommand();
    void Execute() override;
};

#endif
