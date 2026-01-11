#ifndef CrossSectionCommand_h
#define CrossSectionCommand_h

#include <memory>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cmd/ICommand.h>
#include <cmd/CrossSections/Receiver.h>
#include <Models/Particle.h>
#include <DataRelay.h>
#include <io/Exceptions/NotImplemented.h>
#include <Models/Connection.h>
#include <sql/DbManager.h>
#include <sql/Statements/CrossSection.h>
#include <sql/Statements/Particle.h>
#include <sql/Filters/Particle.h>
#include <sql/Callbacks/Particle.h>

class CrossSectionCommand : public ICommand {
private:
    Connection& connection_;
    boost::uuids::uuid scaleFactorId_;
    std::shared_ptr< CrossSectionReceiver > receiver_;
    Models::Particle particle_;
    std::shared_ptr< DataRelay > fortLib_;
    std::shared_ptr< double > temperature_;
    bool squashLogs_;
    SigmaV result_;
    bool postResult_;
    void postResult();
public:
    CrossSectionCommand(
        Connection& connection, 
        const Models::Particle& particle, 
        ::shared_ptr< double > temperature, 
        std::shared_ptr< DataRelay > fortranInterface, 
        bool squashLogs = false
    );
    CrossSectionCommand(
        Connection& connection, 
        const Models::Particle& particle, 
        std::shared_ptr< double > temperature, 
        std::shared_ptr< DataRelay > fortranInterface, 
        boost::uuids::uuid scaleFactorId, 
        bool squashLogs = false
    );
    CrossSectionCommand(
        Connection& connection, 
        boost::uuids::uuid particleId, 
        std::shared_ptr< double > temperature, 
        std::shared_ptr< DataRelay > fortranInterface, 
        boost::uuids::uuid scaleFactorId, 
        bool squashLogs = false
    );
    ~CrossSectionCommand();
    void Execute() override;
    void PostResult(bool post=true);
    void UpdateInputs( 
        std::shared_ptr< double > temperature,
        Models::Particle& particle
    );
    SigmaV getResult();
};

#endif
