#include <sql/Filters/PartialWidth.h>

using namespace Filters;

PartialWidth::PartialWidth(boost::uuids::uuid partialWidthId){
    AddFilterStatement( "ID", boost::lexical_cast<std::string>(partialWidthId), SqlDataType::TEXT, SqlComparator::Equals );
}
PartialWidth::PartialWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParentID", boost::lexical_cast<std::string>(parentId), SqlDataType::TEXT, SqlComparator::Equals );
}
PartialWidth::PartialWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId, boost::uuids::uuid scaleFactorId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParentID", boost::lexical_cast<std::string>(parentId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );
}
PartialWidth::PartialWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId, std::vector< boost::uuids::uuid > childrenIds){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParentID", boost::lexical_cast<std::string>(parentId), SqlDataType::TEXT, SqlComparator::Equals );

    std::string childIdsFormatted;
    for( auto c = childrenIds.begin(); c != childrenIds.end(); ++c ){
        childIdsFormatted.append( 
            boost::lexical_cast<std::string>( *c )
        );
        if (std::next(c) != childrenIds.end()){
            childIdsFormatted.append(", ");
        }
    }
    AddFilterStatement( "ChildrenIDs", childIdsFormatted, SqlDataType::TEXT, SqlComparator::Equals);
}
PartialWidth::PartialWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId, std::vector< boost::uuids::uuid > childrenIds, boost::uuids::uuid scaleFactorId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParentID", boost::lexical_cast<std::string>(parentId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );

    std::string childIdsFormatted;
    for( auto c = childrenIds.begin(); c != childrenIds.end(); ++c ){
        childIdsFormatted.append( 
            boost::lexical_cast<std::string>( *c )
        );
        if (std::next(c) != childrenIds.end()){
            childIdsFormatted.append(", ");
        }
    }
    AddFilterStatement( "ChildrenIDs", childIdsFormatted, SqlDataType::TEXT, SqlComparator::Equals);
}


PartialWidth::~PartialWidth()
{
}
