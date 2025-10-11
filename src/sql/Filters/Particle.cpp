#include <sql/Filters/Particle.h>

using namespace Filters;

Particle::Particle(boost::uuids::uuid id, WhereUUID whereUUID){
    switch (whereUUID)
    {
    case WhereUUID::InputId:
        AddFilterStatement( "InputID", boost::lexical_cast<std::string>(id), SqlDataType::TEXT, SqlComparator::Equals );
        break;
    case WhereUUID::ParticleId:
        AddFilterStatement( "ID", boost::lexical_cast<std::string>(id), SqlDataType::TEXT, SqlComparator::Equals );
        break;
    case WhereUUID::Id:
        AddFilterStatement( "ID", boost::lexical_cast<std::string>(id), SqlDataType::TEXT, SqlComparator::Equals );
        break;
    default:
        throw NotImplementedException();
    }
}
Particle::Particle(std::string particleKey, boost::uuids::uuid inputId, SqlComparator comparison){
    AddFilterStatement( "ParticleKey", particleKey, SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "InputID", boost::lexical_cast<std::string>(inputId), SqlDataType::TEXT, comparison );
}

Particle::Particle(std::string particleKey, boost::uuids::uuid particleId, boost::uuids::uuid inputId){
    AddFilterStatement( "ParticleKey", particleKey, SqlDataType::TEXT, SqlComparator::Equals );
    AddFilterStatement( "ID", boost::lexical_cast<std::string>(particleId), SqlDataType::TEXT, SqlComparator::Equals );
}
   
Particle::~Particle()
{
}
