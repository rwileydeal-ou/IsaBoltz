#ifndef BranchingRatioCommand_h
#define BranchingRatioCommand_h

#include <deque>
#include <memory>
#include <boost/pool/pool_alloc.hpp>
#include <cmd/ICommand.h>
#include <cmd/BranchingRatios/Receiver.h>
#include <Models/Particle.h>
#include <Models/Connection.h>

class BranchingRatioCommand : public ICommand {
private:
    Connection& connection_;
    std::shared_ptr< BranchingRatioReceiver > receiver_;
    Models::Particle particle_;
    const std::deque< Models::Particle >& particles_;
    bool postResult_;
    bool squashLogs_;
    void postSqlTotalWidth(
        DbManager& db, 
        BranchingFraction& result
    );
    void postSqlPartialWidths(
        DbManager& db, 
        BranchingFraction& result
    );
    void postResult();
public:
    BranchingRatioCommand(
        Connection& connection, 
        Models::Particle& particle, 
        const std::deque< Models::Particle >& particles, 
        bool squashLogs = false
    );
    ~BranchingRatioCommand();
    void Execute() override;
    void PostResult(bool post);
    BranchingFraction getResult();
};

#endif
