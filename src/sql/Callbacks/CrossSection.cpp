#include <sql/Callbacks/CrossSection.h>

using namespace Callbacks;

CrossSection::CrossSection(/* args */)
{
}    
CrossSection::~CrossSection()
{
}

int CrossSection::Callback(void *data, int argc, char **argv, char **colName){
    CallbackReturnImplementation *cb = (static_cast< CallbackReturnImplementation* > (data) );

    SigmaV crossSection;
    crossSection.Id = boost::lexical_cast<boost::uuids::uuid>(argv[0]);
    crossSection.InputId = boost::lexical_cast<boost::uuids::uuid>(argv[1]);
    crossSection.ParticleId = boost::lexical_cast<boost::uuids::uuid>(argv[2]);
    crossSection.CrossSection = boost::lexical_cast<double>(argv[3]);
    crossSection.Temperature = boost::lexical_cast<double>(argv[4]);
    crossSection.ScaleFactorId = boost::lexical_cast<boost::uuids::uuid>(argv[5]);

    cb->crossSections.push_back(crossSection);
    return 0;
}
