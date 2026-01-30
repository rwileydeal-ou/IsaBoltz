#ifndef DeltaNeffCommand_h
#define DeltaNeffCommand_h

#include <memory>
#include <io/FileIO.h>
#include <cmd/ICommand.h>
#include <cmd/DeltaNeff/Receiver.h>
#include <cmd/DeltaNeff/EstimateReceiver.h>
#include <cmd/DeltaNeff/BoltzmannReceiver.h>
#include <Models/Particle.h>
#include <Models/Connection.h>
#include <sql/Statements/DeltaNeff.h>
#include <sql/Statements/ScaleFactor.h>
#include <sql/Filters/ScaleFactor.h>
#include <sql/Callbacks/ScaleFactor.h>
#include <sql/Statements/BoltzmannParticleEvolution.h>
#include <sql/Filters/BoltzmannParticleEvolution.h>
#include <sql/Callbacks/BoltzmannParticleEvolution.h>

class DeltaNeffCommand : public ICommand {
private:
    Connection& connection_;
    DbManager& db_;
    std::shared_ptr< DeltaNeffReceiver > receiver_;
    Models::ParticleEvolution particleEvo_;
    Models::Particle particle_;
    Models::ScaleFactorPoint finalScaleFactorPoint_;
public:
    DeltaNeffCommand(
        Connection& connection, 
        DbManager& db,
        std::string particleKey
    );
    DeltaNeffCommand(
        Connection& connection, 
        DbManager& db,
        Models::Particle& particle, 
        std::vector< std::vector< boost::uuids::uuid > > childrenIdPairs
    );
    ~DeltaNeffCommand();
    void Execute() override;
};

#endif
