#include <io/Input/Params/KeyVals/NonUniversalSugra.h>

NonUniversalSugra::NonUniversalSugra(Logger& logger) : 
    Modify(logger)
{}
NonUniversalSugra::~NonUniversalSugra(){}

using namespace std;

map<string, double&> NonUniversalSugra::doubleMap(ModelBase& modelParams){
    map<string, double&> pqOpts;
    pqOpts.insert(pair<string, double&> ("ulgut", modelParams.Options.NonUniversalSugra.GenOneTwo.m_uL_GUT));
    pqOpts.insert(pair<string, double&> ("drgut", modelParams.Options.NonUniversalSugra.GenOneTwo.m_dR_GUT));
    pqOpts.insert(pair<string, double&> ("urgut", modelParams.Options.NonUniversalSugra.GenOneTwo.m_uR_GUT));
    pqOpts.insert(pair<string, double&> ("elgut", modelParams.Options.NonUniversalSugra.GenOneTwo.m_eL_GUT));
    pqOpts.insert(pair<string, double&> ("ergut", modelParams.Options.NonUniversalSugra.GenOneTwo.m_eR_GUT));
    pqOpts.insert(pair<string, double&> ("llgut", modelParams.Options.NonUniversalSugra.GenThree.m_lL_GUT));
    pqOpts.insert(pair<string, double&> ("lrgut", modelParams.Options.NonUniversalSugra.GenThree.m_lR_GUT));
    pqOpts.insert(pair<string, double&> ("brgut", modelParams.Options.NonUniversalSugra.GenThree.m_bR_GUT));
    pqOpts.insert(pair<string, double&> ("tlgut", modelParams.Options.NonUniversalSugra.GenThree.m_tL_GUT));
    pqOpts.insert(pair<string, double&> ("trgut", modelParams.Options.NonUniversalSugra.GenThree.m_tR_GUT));
    pqOpts.insert(pair<string, double&> ("muweak", modelParams.Options.NonUniversalSugra.WeakInput.weakMu));
    pqOpts.insert(pair<string, double&> ("maweak", modelParams.Options.NonUniversalSugra.WeakInput.weakMA));
    pqOpts.insert(pair<string, double&> ("lambdarpv", modelParams.Options.NonUniversalSugra.LambdaRPV));
    return pqOpts;
}

map<string, int&> NonUniversalSugra::intMap(ModelBase& modelParams){
    map<string, int&> pqOpts;
    return pqOpts;
}

map<string, bool&> NonUniversalSugra::boolMap(ModelBase& modelParams){
    map<string, bool&> pqOpts;
    pqOpts.insert(pair<string, bool&> ("gaugino", modelParams.Options.NonUniversalSugra.Gaugino));
    pqOpts.insert(pair<string, bool&> ("trilinear", modelParams.Options.NonUniversalSugra.Trilinear));
    pqOpts.insert(pair<string, bool&> ("higgs", modelParams.Options.NonUniversalSugra.Higgs));
    pqOpts.insert(pair<string, bool&> ("genonetwo", modelParams.Options.NonUniversalSugra.GenOneTwo.GenOneTwo));
    pqOpts.insert(pair<string, bool&> ("genthree", modelParams.Options.NonUniversalSugra.GenThree.GenThree));
    pqOpts.insert(pair<string, bool&> ("weakinput", modelParams.Options.NonUniversalSugra.WeakInput.WeakInput));
    pqOpts.insert(pair<string, bool&> ("rightneutrino", modelParams.Options.NonUniversalSugra.RightNeutrino));
    return pqOpts;
}

