#ifndef Connection_h
#define Connection_h

#include <boost/uuid/uuid.hpp>
#include <boost/property_tree/ptree.hpp>
#include <io/Logging/Logger.h>
#include <Models/ModelBase.h>

struct Connection{
    std::string SqlConnectionString;
    ModelBase Model;
    Logger Log;
    boost::uuids::uuid InputId;
    boost::property_tree::ptree OutputTree;

};

#endif
