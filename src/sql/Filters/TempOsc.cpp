#include <sql/Filters/TempOsc.h>

using namespace Filters;

TempOsc::TempOsc(boost::uuids::uuid tempOscId){
    AddFilterStatement( "ID", boost::lexical_cast<std::string>(tempOscId), SqlDataType::TEXT, SqlComparator::Equals );
}
TempOsc::TempOsc(boost::uuids::uuid inputId, boost::uuids::uuid particleId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParticleID", boost::lexical_cast<std::string>(particleId), SqlDataType::TEXT, SqlComparator::Equals );
}
TempOsc::TempOsc(boost::uuids::uuid inputId, boost::uuids::uuid particleId, boost::uuids::uuid scaleFactorId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParticleID", boost::lexical_cast<std::string>(particleId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );
}

   
TempOsc::~TempOsc()
{
}
