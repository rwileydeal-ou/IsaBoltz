#ifndef BranchingRatioCommand_h
#define BranchingRatioCommand_h

#include <vector>
#include <memory>
#include <boost/pool/pool_alloc.hpp>
#include <cmd/ICommand.h>
#include <cmd/BranchingRatios/Receiver.h>
#include <Models/Particle.h>
#include <Models/Connection.h>

class BranchingRatioCommand : public ICommand {
private:
    Connection& connection_;
    DbManager& db_;
    std::shared_ptr< BranchingRatioReceiver > receiver_;
    Models::Particle particle_;
    const std::vector< Models::Particle >& particles_;
    bool postResult_;
    bool squashLogs_;
    void postSqlTotalWidth(
        BranchingFraction& result
    );
    void postSqlPartialWidths(
        BranchingFraction& result
    );
    void postResult();
public:
    BranchingRatioCommand(
        Connection& connection, 
        DbManager& db,
        Models::Particle& particle, 
        const std::vector< Models::Particle >& particles, 
        bool squashLogs = false
    );
    ~BranchingRatioCommand();
    void Execute() override;
    void PostResult(bool post);
    BranchingFraction getResult();
};

#endif
