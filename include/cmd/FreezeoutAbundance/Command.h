#ifndef FreezeoutAbundanceCommand_h
#define FreezeoutAbundanceCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/FreezeoutAbundance/Receiver.h>
#include <Models/Particle.h>
#include <Models/Connection.h>

class FreezeoutAbundanceCommand : public ICommand {
private:
    Connection& connection_;
    std::shared_ptr< FreezeoutAbundanceReceiver > receiver_; 
    Models::Particle particle_;
    boost::uuids::uuid scaleFactorId_;
public:
    FreezeoutAbundanceCommand(Connection& connection, Models::Particle& particle, boost::uuids::uuid scaleFactorId);
    ~FreezeoutAbundanceCommand();
    void Execute() override;
};

#endif
