#ifndef CheckBBNCommand_h
#define CheckBBNCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/CheckBBN/Receiver.h>
#include <Models/Particle.h>
#include <Models/Connection.h>
#include <sql/Statements/CheckBBN.h>

class CheckBBNCommand : public ICommand {
private:
    Connection& connection_;
    boost::uuids::uuid scaleFactorId_;
    std::shared_ptr< CheckBBNReceiver > receiver_;
    Models::Particle particle_;
public:
    CheckBBNCommand(Connection& connection, std::string enabledKey);
    ~CheckBBNCommand();
    void Execute() override;
};

#endif
