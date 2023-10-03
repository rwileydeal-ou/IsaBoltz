#ifndef TempFreezeoutCommand_h
#define TempFreezeoutCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/TempFreezeout/Receiver.h>
#include <Models/Particle.h>
#include <Models/Connection.h>

class TempFreezeoutCommand : public ICommand {
private:
    Connection& connection_;
    std::shared_ptr< TempFreezeoutReceiver > receiver_;
    Models::Particle particle_;
    std::shared_ptr< double > tempFreezeout_;
public:
    TempFreezeoutCommand(Connection& connection, Models::Particle& particle);
    TempFreezeoutCommand(Connection& connection, Models::Particle& particle, std::shared_ptr< double > tempFreezeout);
    ~TempFreezeoutCommand();
    void Execute() override;
};

#endif
