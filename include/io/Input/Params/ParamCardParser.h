#ifndef ParamCardParser_h
#define ParamCardParser_h

#include <io/FileIO.h>
#include <io/Input/FileParser.h>
#include <io/Logging/Logger.h>
#include <map>
#include <io/Input/Params/KeyVals/Mass.h>
#include <io/Input/Params/KeyVals/PqSector.h>
#include <io/Input/Params/KeyVals/NonUniversalSugra.h>
#include <io/Input/Params/KeyVals/Sugra.h>
#include <io/Input/Params/KeyVals/Modulus.h>

class ParamCardParser : public FileParser
{
    private:
        Logger& logger_;
        EAxionModel parseAxionModel(std::string string);
        std::map<std::string, double&> modelDoubleMap(ModelBase& modelParams);
        std::map<std::string, int&> modelIntMap(ModelBase& modelParams);
    public:
        ParamCardParser(Logger& logger);
        ~ParamCardParser();
        void Parse(std::string paramCard, ModelBase &model);
        void ReadLine(std::string line, ModelBase &model);
        void MatchKey(std::string key, std::string keyVal, ModelBase &model, std::map<std::string, double&> &doubleMap, std::map<std::string, int&> &intMap);
};

#endif
