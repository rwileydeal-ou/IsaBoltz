#ifndef ConfigParser_h
#define ConfigParser_h

#include <Models/ModelBase.h>
#include <io/FileIO.h>
#include <io/Input/FileParser.h>
#include <io/Logging/Logger.h>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

class ConfigParser
{
private:
    Logger& logger_;
    Models::ParticleStatistics parseStats(std::string spin);
    void readParticleConfig(boost::property_tree::ptree &pt, ModelBase &model);
    void readConstantConfig(boost::property_tree::ptree &pt, ModelBase &model);
public:
    void Parse(std::string configCard, ModelBase &model);
    ConfigParser(Logger& logger);
    ~ConfigParser();
};

#endif
