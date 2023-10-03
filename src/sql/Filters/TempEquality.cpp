#include <sql/Filters/TempEquality.h>

using namespace Filters;

TempEquality::TempEquality(boost::uuids::uuid tempEqualityId){
    AddFilterStatement( "ID", boost::lexical_cast<std::string>(tempEqualityId), SqlDataType::TEXT, SqlComparator::Equals );
}
TempEquality::TempEquality(boost::uuids::uuid inputId, boost::uuids::uuid particleId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParticleID", boost::lexical_cast<std::string>(particleId), SqlDataType::TEXT, SqlComparator::Equals );
}
TempEquality::TempEquality(boost::uuids::uuid inputId, boost::uuids::uuid particleId, boost::uuids::uuid scaleFactorId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParticleID", boost::lexical_cast<std::string>(particleId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );
}

   
TempEquality::~TempEquality()
{
}
