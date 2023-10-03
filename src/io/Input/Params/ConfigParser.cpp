#include <io/Input/Params/ConfigParser.h>

using namespace std;

ConfigParser::ConfigParser(Logger& logger) :
    logger_(logger)
{}
ConfigParser::~ConfigParser(){}

Models::ParticleStatistics ConfigParser::parseStats(std::string spin){
    std::vector<std::string> splitLine = FileIO::Split(spin, "/");
    if (splitLine.size() == 1){
        return Models::ParticleStatistics::Boson;
    }
    if (splitLine[1] == "2"){
        return Models::ParticleStatistics::Fermion;
    } else{
        return Models::ParticleStatistics::Boson;
    }
}
void ConfigParser::readConstantConfig(boost::property_tree::ptree &pt, ModelBase &model){
    BOOST_FOREACH( boost::property_tree::ptree::value_type &v, pt.get_child("Config.ConstantsConfig") ) {
        if( v.first == "Constant" ) {
            auto key = v.second.get<std::string>("Key");
            auto name = v.second.get<std::string>("Name");
            auto val = v.second.get<double>("Value");
            if (key == "lambdaqcd"){
                model.StandardModel.LambdaQCD = val;
            } else if (key == "planckmass"){
                model.Constants.mPlanck = val;
            } else if (key == "hbar"){
                model.Constants.hBar = val;  
            } else if (key == "ncolors"){
                model.StandardModel.NC = val;
            } else{
                logger_.Warn("Could not find key " + key);
            }
        }
    }
}

void ConfigParser::readParticleConfig(boost::property_tree::ptree &pt, ModelBase &model){
    BOOST_FOREACH( boost::property_tree::ptree::value_type &v, pt.get_child("Config.ParticleConfig") ) {
        if( v.first == "Particle" ) {
            Models::Particle particle;
            particle.Key = v.second.get<std::string>("Key");
            particle.Name = v.second.get<std::string>("Name");
            particle.PdgCode = v.second.get<int>("PdgCode");
            particle.Width = v.second.get<double>("Width", 0.);

            auto spin = v.second.get<std::string>("Spin");
            particle.Spin = FileParser::ParseFrac( spin );
            particle.Statistics = parseStats( spin );

            Models::Charge emCharge;
            emCharge.Group = Models::GaugeGroup::Electromagnetic;
            emCharge.Value = FileParser::ParseFrac( v.second.get<string>("Charge.Electromagnetic") );
            particle.Charges.push_back( emCharge );

            Models::Charge colorCharge;
            colorCharge.Group = Models::GaugeGroup::Color;
            colorCharge.Value = FileParser::ParseFrac( v.second.get<string>("Charge.Color") );
            particle.Charges.push_back( colorCharge );

            Models::Charge rparityCharge;
            rparityCharge.Group = Models::GaugeGroup::RParity;
            rparityCharge.Value = FileParser::ParseFrac( v.second.get<string>("Charge.RParity", "0.") );
            particle.Charges.push_back( rparityCharge );

            model.Particles.push_back(particle);
            logger_.Debug("Read config for " + particle.Name);
        }
    }
}

void ConfigParser::Parse(string configCard, ModelBase &model){
    boost::property_tree::ptree pt;
    try{
        read_xml(configCard, pt);
        readParticleConfig(pt, model);
        readConstantConfig(pt, model);
    } catch (exception& e){
        logger_.Error(e.what());
        throw;
    }
}
