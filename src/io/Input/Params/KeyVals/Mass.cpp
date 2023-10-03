#include <io/Input/Params/KeyVals/Mass.h>

using namespace std;

Mass::Mass(Logger& logger) :
    logger_(logger)
{}
Mass::~Mass(){}

void Mass::setKeyVal(string key, string keyVal, ModelBase& modelParams){
    boost::to_lower(key);
    boost::to_lower(keyVal);
    double mass = FileParser::ParseDouble(keyVal);
    auto foundKey = find_if(modelParams.Particles.begin(), modelParams.Particles.end(), 
        [&key](const Models::Particle& particle){ return particle.Key == key; }
    );
    if (foundKey != modelParams.Particles.end()){
        (*foundKey).Mass = mass;
    } else{
        logger_.Warn("Could not find key " + key);
        logger_.Info("Adding particle with key " + key);
        auto newParticle = Models::Particle();
        newParticle.Key = key;
        newParticle.Mass = mass;
        modelParams.Particles.push_back( newParticle );
    }
}