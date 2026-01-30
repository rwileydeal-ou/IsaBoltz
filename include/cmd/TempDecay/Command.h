#ifndef TempDecayCommand_h
#define TempDecayCommand_h

#include <memory>
#include <cmd/ICommand.h>
#include <cmd/TempDecay/Receiver.h>
#include <Models/Particle.h>
#include <Models/Connection.h>
#include <sql/Statements/TempDecay.h>

class TempDecayCommand : public ICommand {
private:
    Connection& connection_;
    DbManager& db_;
    std::shared_ptr< TempDecayReceiver > receiver_;
    Models::Particle particle_;
    std::shared_ptr< double > tempDecay_;
public:
    TempDecayCommand(
        Connection& connection, 
        DbManager& db,
        Models::Particle& particle
    );
    TempDecayCommand(
        Connection& connection, 
        DbManager& db,
        Models::Particle& particle, 
        std::shared_ptr< double > tempDecay
    );
    ~TempDecayCommand();
    void Execute() override;
};

#endif
