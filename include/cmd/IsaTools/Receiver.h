#ifndef IsaToolsReceiver_h
#define IsaToolsReceiver_h

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <cmd/IReceiver.h>
#include <Models/ModelBase.h>
#include <Models/Connection.h>
#include <DataRelay.h>
#include <io/Exceptions/NotImplemented.h>

enum IsaTool{
    IsaAmu,
    IsaBsg,
    IsaBtn,
    IsaRes,
    IsaRed
};

class IsaToolsReceiver : public IReceiver
{
private: 
    Connection& connection_;
    std::shared_ptr< DataRelay > fortranInterface_;
    IsaTool tool_;
    int isaVal_;
    int iModel_;
    void runIsaAmu();
    void runIsaBsg();
    void runIsaBtn();
    void runIsaRes();
    void runIsaRed();
    bool squashLogs_;
public:
    ~IsaToolsReceiver();
    IsaToolsReceiver(IsaTool tool, Connection& connection, std::shared_ptr< DataRelay > fortranInterface, bool squashLogs = false);
    void Calculate() override;
};

#endif
