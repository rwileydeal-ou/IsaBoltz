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
#include <Models/Connection.h>
#include <DataRelay.h>
#include <sql/Statements/Particle.h>
#include <sql/Statements/Inputs.h>
#include <sql/DbManager.h>

class MssmSpectrumReceiver : public IReceiver
{
private: 
    Connection& connection_;
    std::shared_ptr< DataRelay > fortranInterface_;
public:
    ~MssmSpectrumReceiver();
    MssmSpectrumReceiver(Connection& connection, std::shared_ptr< DataRelay > fortranInterface);
    void Calculate() override;
};

#endif
