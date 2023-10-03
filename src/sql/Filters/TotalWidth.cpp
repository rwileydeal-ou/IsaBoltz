#include <sql/Filters/TotalWidth.h>

using namespace Filters;

TotalWidth::TotalWidth(boost::uuids::uuid totalWidthId){
    AddFilterStatement( "ID", boost::lexical_cast<std::string>(totalWidthId), SqlDataType::TEXT, SqlComparator::Equals );
}
TotalWidth::TotalWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParentID", boost::lexical_cast<std::string>(parentId), SqlDataType::TEXT, SqlComparator::Equals );
}
TotalWidth::TotalWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId, boost::uuids::uuid scaleFactorId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParentID", boost::lexical_cast<std::string>(parentId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );
}

   
TotalWidth::~TotalWidth()
{
}
