#ifndef TempEqualityMacro_h
#define TempEqualityMacro_h

#include <memory>
#include <client/Sender.h>
#include <cmd/Commands.h>
#include <cmd/MssmSpectrum/Command.h>
#include <cmd/TempOscillation/Command.h>
#include <cmd/TempEquality/Command.h>
#include <macros/Macro.h>
#include <io/FileIO.h>
#include <Models/Connection.h>

class TempEqualityMacro : public Macro {
private:
    Connection& connection_;
    DbManager& db_;
    CommandWithPayload cmd_;
    std::shared_ptr< Sender > invoker_;
    bool interactiveMode_;
public:
    TempEqualityMacro(
        CommandWithPayload cmd, 
        std::shared_ptr< Sender > invoker, 
        std::shared_ptr< MssmSpectrumCommand >& spectraCmd, 
        Connection& connection, 
        DbManager& db,
        bool interactiveMode
    );
    ~TempEqualityMacro();
    void Execute() override;
};

#endif
