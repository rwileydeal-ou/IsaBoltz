#ifndef CriticalAbundanceCommand_h
#define CriticalAbundanceCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/CriticalAbundance/Receiver.h>
#include <Models/Particle.h>
#include <Models/Connection.h>

class CriticalAbundanceCommand : public ICommand {
private:
    Connection& connection_;
    std::shared_ptr< CriticalAbundanceReceiver > receiver_;
    Models::Particle particle_;
public:
    CriticalAbundanceCommand(Connection& connection, Models::Particle& particle);
    ~CriticalAbundanceCommand();
    void Execute() override;
};

#endif
