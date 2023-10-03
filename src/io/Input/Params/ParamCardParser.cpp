#include <io/Input/Params/ParamCardParser.h>

using namespace std;

ParamCardParser::ParamCardParser(Logger& logger) : 
    FileParser(),
    logger_(logger)
{

}
ParamCardParser::~ParamCardParser(){}

EAxionModel ParamCardParser::parseAxionModel(string string){
    std::string lowString = boost::algorithm::to_lower_copy(string);
    if (lowString.find("dfsz") != string::npos)
        return DFSZ;
    else if (lowString.find("ksvz") != string::npos)
        return KSVZ;
    else 
        throw logic_error("Could not parse Axion Model");
}

map<string, double&> ParamCardParser::modelDoubleMap(ModelBase& modelParams){
    map<string, double&> modOpts;
    // SM params
    modOpts.insert(pair<string, double&> ("vew", modelParams.StandardModel.vEW));
    modOpts.insert(pair<string, double&> ("gsu2", modelParams.StandardModel.Couplings.gSU2));
    modOpts.insert(pair<string, double&> ("gu1", modelParams.StandardModel.Couplings.gU1));
    modOpts.insert(pair<string, double&> ("alphas", modelParams.StandardModel.Couplings.alphaS));
    modOpts.insert(pair<string, double&> ("alphaem", modelParams.StandardModel.Couplings.alphaEM));
    modOpts.insert(pair<string, double&> ("sin2tw", modelParams.StandardModel.MixingAngles.SinSqrThetaW));
    
    // Cosmology params
    modOpts.insert(pair<string, double&> ("treheat", modelParams.Cosmology.Temperatures.Reheat));
    modOpts.insert(pair<string, double&> ("tneutdecouple", modelParams.Cosmology.Temperatures.NeutrinoDecouple));
    modOpts.insert(pair<string, double&> ("thadronization", modelParams.Cosmology.Temperatures.Hadronization));
    modOpts.insert(pair<string, double&> ("tevolvefinal", modelParams.Cosmology.Temperatures.Final));

    return modOpts;
}

map<string, int&> ParamCardParser::modelIntMap(ModelBase& modelParams){
    map<string, int&> modInt;
    modInt.insert(pair<string, int&> ("nw", modelParams.NW));
    modInt.insert(pair<string, int&> ("susymodel", modelParams.Options.ModelInt));
    return modInt;
}

void ParamCardParser::ReadLine(string line, ModelBase &model){
    auto split = FileIO::Split(line, " =\t\0");
    if (split.size() > 1){
        string key = split[0];
        if (key == "" || key.at(0) == '#'){
            return;
        }
        string keyVal = split[1];

        auto keySeparation = FileIO::Split(key, ".");
        string key1 = keySeparation[0];
        boost::to_lower(key1);
        string key2 = "";

        if (keySeparation.size() > 1){
            key2 = keySeparation[1];
            boost::to_lower(key2);
        }

        if (key2 == "amplitude"){
            ModelBaseOps::Find(model.Particles, key1).CohOscComponents.Amplitude = FileParser::ParseDouble(keyVal);
        } else if(key2 == "mass"){
            Mass m(logger_);
            m.setKeyVal(key1, keyVal, model);
        } else if (key1 == "pq"){
            PqSector pq(logger_);
            pq.SetKeyVal(key2, keyVal, model);
        } else if (key1 == "sug"){
            Sugra sug(logger_);
            sug.SetKeyVal(key2, keyVal, model);
        } else if (key1 == "nusug"){
            NonUniversalSugra nusug(logger_);
            nusug.SetKeyVal(key2, keyVal, model);
        } else if (key1 == "moduli"){
            boost::to_lower(key2);
                Modulus mod(logger_);
                mod.SetKeyVal(key2, keyVal, model);
        } else{
            auto mapDouble = modelDoubleMap(model);
            auto mapInt = modelIntMap(model);
            MatchKey(key, keyVal, model, mapDouble, mapInt);
        }
    }
}

void ParamCardParser::MatchKey(string key, string keyVal, ModelBase &model, map<string, double&> &mapDouble, map<string, int&> &mapInt){
    try{
        boost::to_lower(key);
        boost::to_lower(keyVal);
        if (key[0] != '#' && key[0] != ' ' && key[0] != '\0'){
            logger_.Debug( "Setting " + key + ": " + keyVal );
            auto posDouble = mapDouble.find(key);
            auto posInt = mapInt.find(key);
            if (posDouble != mapDouble.end()){
                posDouble->second = FileParser::ParseDouble(keyVal);
            } else if(posInt != mapInt.end()){
                posInt->second = FileParser::ParseInt(keyVal);
            } else if(key == "axionmodel"){
                model.PQSector.AxionModel = parseAxionModel(keyVal);
            } else{
                logger_.Warn( "Could not find model key=" + key );
            }
        }
    } catch(exception& e){
        logger_.Error(e.what());
        throw;
    }
}

void ParamCardParser::Parse(string infile, ModelBase &model){
    try{
        logger_.Info("Parsing param_card: " + infile);
        FileIO::ValidateFilePath(infile, logger_);

        ifstream file;
        file.open(infile);
        string line;
        while(getline(file, line)){
            ReadLine(line, model);
        }
    } catch(FileNotFoundException& e){ 
        logger_.Error( "Could not find param_card: " + infile);
        throw;
    }
}
