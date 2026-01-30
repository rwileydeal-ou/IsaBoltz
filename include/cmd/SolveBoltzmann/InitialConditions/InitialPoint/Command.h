#ifndef InitialPointCommand_h
#define InitialPointCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/SolveBoltzmann/InitialConditions/InitialPoint/Receiver.h>
#include <io/Exceptions/NotImplemented.h>
#include <Models/ScaleFactor.h>
#include <Models/Connection.h>
#include <sql/Statements/ScaleFactor.h>

class InitialPointCommand : public ICommand {
private:
    Connection& connection_;
    DbManager& db_;
    std::shared_ptr< InitialPointReceiver > receiver_;
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint_;
public:
    InitialPointCommand(
        Connection& connection,
        DbManager& db
    );
    InitialPointCommand(
        Connection& connection, 
        DbManager& db,
        std::shared_ptr< Models::ScaleFactorPoint > initialPoint
    );
    ~InitialPointCommand();
    void Execute() override;
};

#endif
