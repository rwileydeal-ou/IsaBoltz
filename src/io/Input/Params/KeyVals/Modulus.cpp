#include <io/Input/Params/KeyVals/Modulus.h>

Modulus::Modulus(Logger& logger) :
    Modify(logger)
{}
Modulus::~Modulus(){}

using namespace std;

map<string, double&> Modulus::doubleMap(ModelBase& modelParams){
    map<string, double&> modOpts;
    modOpts.insert(pair<string, double&> ("lambdau1", modelParams.ModulusCouplings.GaugeCouplings.LambdaU1));
    modOpts.insert(pair<string, double&> ("lambdasu2", modelParams.ModulusCouplings.GaugeCouplings.LambdaSU2));
    modOpts.insert(pair<string, double&> ("lambdasu3", modelParams.ModulusCouplings.GaugeCouplings.LambdaSU3));
    modOpts.insert(pair<string, double&> ("lambdahgm", modelParams.ModulusCouplings.HiggsCouplings.LambdaGM));
    modOpts.insert(pair<string, double&> ("lambdahu", modelParams.ModulusCouplings.HiggsCouplings.LambdaHu));
    modOpts.insert(pair<string, double&> ("lambdahd", modelParams.ModulusCouplings.HiggsCouplings.LambdaHd));
    modOpts.insert(pair<string, double&> ("lambdauu", modelParams.ModulusCouplings.QuarkSquarkCouplings.LambdaUu));
    modOpts.insert(pair<string, double&> ("lambdauc", modelParams.ModulusCouplings.QuarkSquarkCouplings.LambdaUc));
    modOpts.insert(pair<string, double&> ("lambdaut", modelParams.ModulusCouplings.QuarkSquarkCouplings.LambdaUt));
    modOpts.insert(pair<string, double&> ("lambdadd", modelParams.ModulusCouplings.QuarkSquarkCouplings.LambdaDd));
    modOpts.insert(pair<string, double&> ("lambdads", modelParams.ModulusCouplings.QuarkSquarkCouplings.LambdaDs));
    modOpts.insert(pair<string, double&> ("lambdadb", modelParams.ModulusCouplings.QuarkSquarkCouplings.LambdaDb));
    modOpts.insert(pair<string, double&> ("lambdaq1", modelParams.ModulusCouplings.QuarkSquarkCouplings.LambdaQ1));
    modOpts.insert(pair<string, double&> ("lambdaq2", modelParams.ModulusCouplings.QuarkSquarkCouplings.LambdaQ2));
    modOpts.insert(pair<string, double&> ("lambdaq3", modelParams.ModulusCouplings.QuarkSquarkCouplings.LambdaQ3));
    modOpts.insert(pair<string, double&> ("lambdale", modelParams.ModulusCouplings.LeptonSleptonCouplings.LambdaLe));
    modOpts.insert(pair<string, double&> ("lambdalm", modelParams.ModulusCouplings.LeptonSleptonCouplings.LambdaLm));
    modOpts.insert(pair<string, double&> ("lambdalt", modelParams.ModulusCouplings.LeptonSleptonCouplings.LambdaLt));
    modOpts.insert(pair<string, double&> ("lambdae1", modelParams.ModulusCouplings.LeptonSleptonCouplings.LambdaE1));
    modOpts.insert(pair<string, double&> ("lambdae2", modelParams.ModulusCouplings.LeptonSleptonCouplings.LambdaE2));
    modOpts.insert(pair<string, double&> ("lambdae3", modelParams.ModulusCouplings.LeptonSleptonCouplings.LambdaE3));
    modOpts.insert(pair<string, double&> ("lambdag", modelParams.ModulusCouplings.GravitinoCouplings.LambdaG));
    modOpts.insert(pair<string, double&> ("lambdaaxion", modelParams.ModulusCouplings.AxionicCouplings.LambdaAx));
    modOpts.insert(pair<string, double&> ("lambdaalp", modelParams.ModulusCouplings.AxionicCouplings.LambdaALP));

    return modOpts;
}

map<string, int&> Modulus::intMap(ModelBase& modelParams){
    map<string, int&> modOpts;
    return modOpts;
}

map<string, bool&> Modulus::boolMap(ModelBase& modelParams){
    map<string, bool&> modOpts;
    return modOpts;
}
