#include <sql/Filters/TempDecay.h>

using namespace Filters;

TempDecay::TempDecay(boost::uuids::uuid tempDecayId){
    AddFilterStatement( "ID", boost::lexical_cast<std::string>(tempDecayId), SqlDataType::TEXT, SqlComparator::Equals );
}
TempDecay::TempDecay(boost::uuids::uuid inputId, boost::uuids::uuid particleId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParticleID", boost::lexical_cast<std::string>(particleId), SqlDataType::TEXT, SqlComparator::Equals );
}
TempDecay::TempDecay(boost::uuids::uuid inputId, boost::uuids::uuid particleId, boost::uuids::uuid scaleFactorId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParticleID", boost::lexical_cast<std::string>(particleId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );
}

   
TempDecay::~TempDecay()
{
}
