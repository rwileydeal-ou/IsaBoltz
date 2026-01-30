#ifndef MssmSpectrumCommand_h
#define MssmSpectrumCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/MssmSpectrum/Receiver.h>
#include <DataRelay.h>
#include <Models/Connection.h>

class MssmSpectrumCommand : public ICommand {
private:
    Connection& connection_;
    DbManager& db_;
    std::shared_ptr< MssmSpectrumReceiver > receiver_;
    std::shared_ptr< DataRelay > fortLib_;
public:
    std::shared_ptr< DataRelay > getFortranInterface();
    MssmSpectrumCommand(
        Connection& connection,
        DbManager& db
    );
    ~MssmSpectrumCommand();
    void Execute() override;
};

#endif
