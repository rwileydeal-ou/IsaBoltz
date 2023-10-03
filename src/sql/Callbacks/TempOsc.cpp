#include <sql/Callbacks/TempOsc.h>

using namespace Callbacks;
using namespace std;

TempOsc::TempOsc(/* args */)
{
}    
TempOsc::~TempOsc()
{
}

int TempOsc::Callback(void *data, int argc, char **argv, char **colName){
    CallbackReturnImplementation *cb = (static_cast< CallbackReturnImplementation* > (data) );

    Models::TempOscillation tempOsc;
    tempOsc.Id = boost::lexical_cast<boost::uuids::uuid>(argv[0]);
    tempOsc.InputId = boost::lexical_cast<boost::uuids::uuid>(argv[1]);
    tempOsc.ParticleId = boost::lexical_cast<boost::uuids::uuid>(argv[2]);
    tempOsc.Temperature = boost::lexical_cast<double>(argv[3]);
    tempOsc.GStar = boost::lexical_cast<double>(argv[4]);
    tempOsc.GStarEntropic = boost::lexical_cast<double>(argv[5]);

    if (argv[6] != nullptr){
        tempOsc.ScaleFactorId = boost::lexical_cast<boost::uuids::uuid>(argv[6]);
    }

    cb->TempOscs.push_back(tempOsc);
    return 0;
}
