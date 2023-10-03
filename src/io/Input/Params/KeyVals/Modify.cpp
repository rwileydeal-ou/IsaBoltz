#include <io/Input/Params/KeyVals/Modify.h>

Modify::Modify(Logger& logger) :
    logger_(logger)
{}
Modify::~Modify(){}

using namespace std;

void Modify::SetKeyVal(string key, string keyVal, ModelBase& modelParams){
    try{
        boost::to_lower(key);
        boost::to_lower(keyVal);

        auto mapDouble = doubleMap(modelParams);
        auto mapInt = intMap(modelParams);
        auto mapBool = boolMap(modelParams);

        if (key[0] != '#' && key[0] != ' ' && key[0] != '\0'){
            logger_.Debug( "Setting " + key + ": " + keyVal );
            auto posDouble = mapDouble.find(key);
            auto posInt = mapInt.find(key);
            auto posBool = mapBool.find(key);

            if (posDouble != mapDouble.end()){
                posDouble->second = FileParser::ParseDouble(keyVal);
            } else if(posInt != mapInt.end()){
                posInt->second = FileParser::ParseInt(keyVal);
            } else if(posBool != mapBool.end()){
                posBool->second = FileParser::ParseBool(keyVal);
            } else{
                logger_.Warn( "Could not find model key=" + key );
            }
        }
    } catch(exception& e){
        logger_.Error(e.what());
        throw;
    }
}
