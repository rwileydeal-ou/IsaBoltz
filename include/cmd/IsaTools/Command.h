#ifndef IsaToolsCommand_h
#define IsaToolsCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/IsaTools/Receiver.h>
#include <DataRelay.h>
#include <Models/Connection.h>

class IsaToolsCommand : public ICommand {
private:
    Connection& connection_;
    std::shared_ptr< IsaToolsReceiver > receiver_;
    IsaTool tool_;
    std::shared_ptr< DataRelay > fortLib_;
public:
    IsaToolsCommand(IsaTool tool, Connection& connection, std::shared_ptr< DataRelay > fortranInterface, bool squashLogs = false);
    ~IsaToolsCommand();
    
    void Execute() override;
};

#endif
