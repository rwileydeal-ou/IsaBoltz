#include <sql/Filters/ScaleFactor.h>

using namespace Filters;

ScaleFactor::ScaleFactor(boost::uuids::uuid uuid, WhereUUID whereUUID){
    switch (whereUUID)
    {
    case WhereUUID::Id:
        AddFilterStatement( "ID", boost::lexical_cast<std::string>(uuid), SqlDataType::TEXT, SqlComparator::Equals );
        break;
    case WhereUUID::InputId:
        AddFilterStatement( "InputID", boost::lexical_cast<std::string>(uuid), SqlDataType::TEXT, SqlComparator::Equals );
        break;
    default:
        throw NotImplementedException();
    }
}

ScaleFactor::ScaleFactor(int ordinal, boost::uuids::uuid inputId){
    AddFilterStatement( "Ordinal", boost::lexical_cast<std::string>(ordinal), SqlDataType::INTEGER, SqlComparator::Equals );
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
}

ScaleFactor::~ScaleFactor()
{
}
