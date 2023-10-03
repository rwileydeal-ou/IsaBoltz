#include <io/Input/Params/KeyVals/PqSector.h>

PqSector::PqSector(Logger& logger) : 
    Modify(logger)    
{}
PqSector::~PqSector(){}

using namespace std;

map<string, double&> PqSector::doubleMap(ModelBase& modelParams){
    map<string, double&> pqOpts;
    pqOpts.insert(pair<string, double&> ("gcck", modelParams.PQSector.Potential.g));
    pqOpts.insert(pair<string, double&> ("thetai", modelParams.PQSector.Theta_I));
    pqOpts.insert(pair<string, double&> ("fa", modelParams.PQSector.Fa));
    pqOpts.insert(pair<string, double&> ("xi", modelParams.PQSector.Xi));
    return pqOpts;
}

map<string, int&> PqSector::intMap(ModelBase& modelParams){
    map<string, int&> pqOpts;
    return pqOpts;
}

map<string, bool&> PqSector::boolMap(ModelBase& modelParams){
    map<string, bool&> pqOpts;
    return pqOpts;
}
