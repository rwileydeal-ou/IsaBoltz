#include <sql/Callbacks/TempDecay.h>

using namespace Callbacks;
using namespace std;

TempDecay::TempDecay(/* args */)
{
}    
TempDecay::~TempDecay()
{
}

int TempDecay::Callback(void *data, int argc, char **argv, char **colName){
    CallbackReturnImplementation *cb = (static_cast< CallbackReturnImplementation* > (data) );

    Models::TempDecay tempDecay;
    tempDecay.Id = boost::lexical_cast<boost::uuids::uuid>(argv[0]);
    tempDecay.InputId = boost::lexical_cast<boost::uuids::uuid>(argv[1]);
    tempDecay.ParticleId = boost::lexical_cast<boost::uuids::uuid>(argv[2]);
    tempDecay.Temperature = boost::lexical_cast<double>(argv[3]);
    tempDecay.GStar = boost::lexical_cast<double>(argv[4]);
    tempDecay.GStarEntropic = boost::lexical_cast<double>(argv[5]);

    if (argv[6] != nullptr){
        tempDecay.ScaleFactorId = boost::lexical_cast<boost::uuids::uuid>(argv[6]);
    }

    cb->TempDecays.push_back(tempDecay);
    return 0;
}
