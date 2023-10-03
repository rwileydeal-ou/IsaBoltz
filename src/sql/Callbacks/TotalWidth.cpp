#include <sql/Callbacks/TotalWidth.h>

using namespace Callbacks;
using namespace std;

TotalWidth::TotalWidth(/* args */)
{
}    
TotalWidth::~TotalWidth()
{
}

int TotalWidth::Callback(void *data, int argc, char **argv, char **colName){
    CallbackReturnImplementation *cb = (static_cast< CallbackReturnImplementation* > (data) );

    Models::TotalWidth totalWidth;
    totalWidth.Id = boost::lexical_cast<boost::uuids::uuid>(argv[0]);
    totalWidth.InputId = boost::lexical_cast<boost::uuids::uuid>(argv[1]);

    // if the particle is stable, there may not be any partial widths - so check for null before pulling
    if ( argv[2] != nullptr ){
        auto partialWidthIdsRaw = boost::lexical_cast<std::string>(argv[2]);
        vector<string> partialWidthIds;
        boost::split(partialWidthIds, partialWidthIdsRaw, boost::is_any_of(", "), boost::token_compress_on);
        for (auto& partialWidthId : partialWidthIds){
            totalWidth.PartialWidthIds.push_back( boost::lexical_cast<boost::uuids::uuid>(partialWidthId) );
        }
    }

    totalWidth.ParentId = boost::lexical_cast<boost::uuids::uuid>(argv[3]);
    totalWidth.Width = boost::lexical_cast<double>(argv[4]);

    if (argv[5] != nullptr){
        totalWidth.ScaleFactorId = boost::lexical_cast<boost::uuids::uuid>(argv[5]);
    }

    cb->TotalWidths.push_back(totalWidth);
    return 0;
}
