#include <sql/Callbacks/TempEquality.h>

using namespace Callbacks;
using namespace std;

TempEquality::TempEquality(/* args */)
{
}    
TempEquality::~TempEquality()
{
}

int TempEquality::Callback(void *data, int argc, char **argv, char **colName){
    CallbackReturnImplementation *cb = (static_cast< CallbackReturnImplementation* > (data) );

    Models::TempEquality tempEquality;
    tempEquality.Id = boost::lexical_cast<boost::uuids::uuid>(argv[0]);
    tempEquality.InputId = boost::lexical_cast<boost::uuids::uuid>(argv[1]);
    tempEquality.ParticleId = boost::lexical_cast<boost::uuids::uuid>(argv[2]);
    tempEquality.ProductionMechanism = static_cast<ParticleProductionMechanism>( std::stoi( argv[3] ) );
    tempEquality.Temperature = boost::lexical_cast<double>(argv[4]);
    tempEquality.GStar = boost::lexical_cast<double>(argv[5]);
    tempEquality.GStarEntropic = boost::lexical_cast<double>(argv[6]);

    if (argv[7] != nullptr){
        tempEquality.ScaleFactorId = boost::lexical_cast<boost::uuids::uuid>(argv[7]);
    }

    cb->TempEqualities.push_back(tempEquality);
    return 0;
}
