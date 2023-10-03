#ifndef TempOscillationCommand_h
#define TempOscillationCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/TempOscillation/Receiver.h>
#include <Models/Particle.h>
#include <Models/Connection.h>
#include <sql/DbManager.h>
#include <sql/Statements/TempOsc.h>

class TempOscillationCommand : public ICommand {
private:
    Connection& connection_;
    std::shared_ptr< TempOscillationReceiver > receiver_;
    Models::Particle particle_;
public:
    TempOscillationCommand(Connection& connection, Models::Particle& particle);
    ~TempOscillationCommand();
    void Execute() override;
};

#endif
