#include <sql/Filters/BoltzmannParticleEvolution.h>

using namespace Filters;

ParticleEvolution::ParticleEvolution(boost::uuids::uuid particleEvolutionId){
    AddFilterStatement( "ID", boost::lexical_cast<std::string>(particleEvolutionId), SqlDataType::TEXT, SqlComparator::Equals );
}

ParticleEvolution::ParticleEvolution(boost::uuids::uuid particleId, ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId){
    AddFilterStatement( "ParticleID", boost::lexical_cast<std::string>(particleId), SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "Production", boost::lexical_cast<std::string>(productionMechanism), SqlDataType::INTEGER, SqlComparator::Equals );
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );
}

ParticleEvolution::ParticleEvolution(std::string particleKey, ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId){
    AddFilterStatement( "ParticleKey", particleKey, SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "Production", boost::lexical_cast<std::string>(productionMechanism), SqlDataType::INTEGER, SqlComparator::Equals );
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );
}

ParticleEvolution::ParticleEvolution(ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId){
    if (productionMechanism != ParticleProductionMechanism::ANY){
        AddFilterStatement( "Production", boost::lexical_cast<std::string>(productionMechanism), SqlDataType::INTEGER, SqlComparator::Equals );
    }
    AddFilterStatement( "ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId), SqlDataType::TEXT, SqlComparator::Equals );
}

ParticleEvolution::~ParticleEvolution()
{
}
