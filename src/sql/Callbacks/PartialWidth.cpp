#include <sql/Callbacks/PartialWidth.h>

using namespace Callbacks;
using namespace std;

PartialWidth::PartialWidth(/* args */)
{
}    
PartialWidth::~PartialWidth()
{
}

int PartialWidth::Callback(void *data, int argc, char **argv, char **colName){
    CallbackReturnImplementation *cb = (static_cast< CallbackReturnImplementation* > (data) );

    Models::PartialWidth partialWidth;
    partialWidth.Id = boost::lexical_cast<boost::uuids::uuid>(argv[0]);
    partialWidth.InputId = boost::lexical_cast<boost::uuids::uuid>(argv[1]);
    partialWidth.TotalWidthId = boost::lexical_cast<boost::uuids::uuid>(argv[2]);
    partialWidth.ParentId = boost::lexical_cast<boost::uuids::uuid>(argv[3]);

    auto childIdsRaw = boost::lexical_cast<std::string>(argv[4]);
    vector<string> childIds;
    boost::split(childIds, childIdsRaw, boost::is_any_of(", "), boost::token_compress_on);
    for (auto& childId : childIds){
        partialWidth.ChildrenIds.push_back( boost::lexical_cast<boost::uuids::uuid>(childId) );
    }

    partialWidth.Width = boost::lexical_cast<double>(argv[5]);
    if (argv[6] != nullptr){
        partialWidth.ScaleFactorId = boost::lexical_cast<boost::uuids::uuid>(argv[6]);
    }

    cb->PartialWidths.push_back(partialWidth);
    return 0;
}
