#ifndef FiltersParticleEvolution_h
#define FiltersParticleEvolution_h

#include <sql/IFilter.h>
#include <Models/Particle.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace Filters{
    class ParticleEvolution : public IFilter
    {
    private:
        /* data */
    public:
        ParticleEvolution(boost::uuids::uuid particleEvolutionId);
        ParticleEvolution(boost::uuids::uuid particleId, ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId);
        ParticleEvolution(std::string particleKey, ParticleProductionMechanism productionMechanism, boost::uuids::uuid uuid, WhereUUID whereUUID);
        ParticleEvolution( ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId );
        ~ParticleEvolution();
    };
}

#endif
