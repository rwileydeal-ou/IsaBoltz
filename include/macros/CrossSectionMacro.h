#ifndef CrossSectionMacro_h
#define CrossSectionMacro_h

#include <memory>
#include <client/Sender.h>
#include <cmd/Commands.h>
#include <cmd/MssmSpectrum/Command.h>
#include <cmd/IsaTools/Command.h>
#include <cmd/CrossSections/Command.h>
#include <macros/Macro.h>
#include <io/FileIO.h>
#include <Models/Connection.h>

class CrossSectionMacro : public Macro {
private:
    Connection& connection_;
    DbManager& db_;
    CommandWithPayload cmd_;
    std::shared_ptr< MssmSpectrumCommand > spectraCmd_;
    std::shared_ptr< Sender > invoker_;
    bool interactiveMode_;

    // data
    std::shared_ptr< double > temp_;
public:
    CrossSectionMacro(
        CommandWithPayload cmd, 
        std::shared_ptr< Sender > invoker, 
        std::shared_ptr< MssmSpectrumCommand >& spectraCmd, 
        Connection& connection, 
        DbManager& db,
        bool interactiveMode
    );
    ~CrossSectionMacro();
    void Execute() override;
};

#endif
