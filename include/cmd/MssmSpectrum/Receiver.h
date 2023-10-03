#ifndef MssmSpectrumReceiver_h
#define MssmSpectrumReceiver_h

#include <iostream>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <cmd/IReceiver.h>
#include <Models/ModelBase.h>
#include <DataRelay.h>
#include <sql/Statements/Particle.h>
#include <sql/DbManager.h>

class MssmSpectrumReceiver : public IReceiver
{
private: 
    Logger& logger_;
    boost::uuids::uuid inputId_;
    std::string sqlConnectionString_;
    ModelBase& model_;
    boost::property_tree::ptree outputTree_;
    std::shared_ptr< DataRelay > fortranInterface_;
public:
    ~MssmSpectrumReceiver();
    MssmSpectrumReceiver(ModelBase& model, std::shared_ptr< DataRelay > fortranInterface, boost::property_tree::ptree model_tree, Logger& logger, std::string sqlConnectionString, boost::uuids::uuid inputId);
    void Calculate() override;
};

#endif
