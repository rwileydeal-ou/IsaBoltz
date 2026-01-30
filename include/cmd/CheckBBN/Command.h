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
    DbManager& db_;
    boost::uuids::uuid scaleFactorId_;
    std::shared_ptr< CheckBBNReceiver > receiver_;
    Models::Particle particle_;
    Models::ScaleFactorPoint findScaleFactor( 
        std::vector<Models::ScaleFactorPoint>& scaleFactors,  
        boost::uuids::uuid scaleFactorId
    );
    Models::TotalWidth findTotalWidth( 
        std::vector<Models::TotalWidth>& totalWidths,  
        boost::uuids::uuid parentId
    );
    Models::Particle findParticle( 
        std::vector<Models::Particle>& particles,  
        boost::uuids::uuid particleId
    );
public:
    CheckBBNCommand(
        Connection& connection, 
        DbManager& db,
        std::string enabledKey
    );
    ~CheckBBNCommand();
    void Execute() override;
};

#endif
