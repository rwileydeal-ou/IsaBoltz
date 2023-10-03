#include <sql/Filters/CrossSection.h>

using namespace Filters;

CrossSection::CrossSection(boost::uuids::uuid crossSectionId){
    AddFilterStatement( "ID", boost::lexical_cast<std::string>(crossSectionId), SqlDataType::TEXT, SqlComparator::Equals );
}
CrossSection::CrossSection(boost::uuids::uuid inputId, boost::uuids::uuid particleId, boost::uuids::uuid scaleFactorId){
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ParticleID", boost::lexical_cast<std::string>(particleId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );
}
CrossSection::CrossSection(ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId){
    if (productionMechanism == ParticleProductionMechanism::ANY){
    }
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );
}

   
CrossSection::~CrossSection()
{
}
