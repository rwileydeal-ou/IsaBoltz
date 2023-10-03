#include <io/Input/Params/KeyVals/Sugra.h>

Sugra::Sugra(Logger& logger) :
    Modify(logger)    
{}
Sugra::~Sugra(){}

using namespace std;

map<string, double&> Sugra::doubleMap(ModelBase& modelParams){
    map<string, double&> pqOpts;
    pqOpts.insert(pair<string, double&> ("m0", modelParams.Masses.M0));
    pqOpts.insert(pair<string, double&> ("mhf", modelParams.Masses.MHF));
    pqOpts.insert(pair<string, double&> ("a0", modelParams.A0));
    pqOpts.insert(pair<string, double&> ("tanb", modelParams.Susy.TanBeta));
    pqOpts.insert(pair<string, double&> ("sgnmu", modelParams.SgnMu));
    return pqOpts;
}

map<string, int&> Sugra::intMap(ModelBase& modelParams){
    map<string, int&> pqOpts;
    return pqOpts;
}

map<string, bool&> Sugra::boolMap(ModelBase& modelParams){
    map<string, bool&> pqOpts;
    return pqOpts;
}

