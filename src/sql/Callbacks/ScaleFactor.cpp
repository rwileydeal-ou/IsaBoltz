#include <sql/Callbacks/ScaleFactor.h>

using namespace Callbacks;
using namespace std;

ScaleFactor::ScaleFactor(/* args */)
{
}    
ScaleFactor::~ScaleFactor()
{
}

int ScaleFactor::Callback(void *data, int argc, char **argv, char **colName){
    CallbackReturnImplementation *cb = (static_cast< CallbackReturnImplementation* > (data) );

    Models::ScaleFactorPoint scaleFactor;
    scaleFactor.Id = boost::lexical_cast<boost::uuids::uuid>(argv[0]);
    scaleFactor.InputId = boost::lexical_cast<boost::uuids::uuid>(argv[1]);
    scaleFactor.Temperature = boost::lexical_cast<double>(argv[2]);
    scaleFactor.ScaleFactor = boost::lexical_cast<double>(argv[3]);
    if ( argv[4] != nullptr ){
        scaleFactor.Entropy = boost::lexical_cast<double>(argv[4]);
    } else{
        scaleFactor.Entropy = INFINITY;
    }
    if ( argv[5] != nullptr ){
        scaleFactor.Hubble = boost::lexical_cast<double>(argv[5]);
    } else{
        scaleFactor.Hubble = INFINITY;
    }
    scaleFactor.GStar = boost::lexical_cast<double>(argv[6]);
    scaleFactor.GStarEntropic = boost::lexical_cast<double>(argv[7]);
    scaleFactor.Ordinal = boost::lexical_cast<int>(argv[8]);

    cb->ScaleFactors.push_back(scaleFactor);
    return 0;
}
