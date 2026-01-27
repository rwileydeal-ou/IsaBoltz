#ifndef RelicDensity_h
#define RelicDensity_h

#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/ScaleFactor.h>
#include <Models/Particle.h>
#include <Models/ParticleEvolution.h>

namespace Models{
    struct RelicDensity
    {
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        boost::uuids::uuid ParticleId;
        boost::uuids::uuid ParticleEvolutionId;
        boost::uuids::uuid ScaleFactorId;
        double OmegaHSqr = 0.;

        RelicDensity(double omegaHSqr, const ParticleEvolution& pEvo){
            Id = boost::uuids::random_generator()();
            InputId = pEvo.InputId;
            ParticleId = pEvo.ParticleId;
            ParticleEvolutionId = pEvo.Id;
            ScaleFactorId = pEvo.ScaleFactorId;
            OmegaHSqr = omegaHSqr;
        }

        RelicDensity(double omegaHSqr, const boost::uuids::uuid& inputId, const boost::uuids::uuid& particleId, const boost::uuids::uuid& particleEvoId, const boost::uuids::uuid& scaleFactorId){
            Id = boost::uuids::random_generator()();
            InputId = inputId;
            ParticleId = particleId;
            ParticleEvolutionId = particleEvoId;
            ScaleFactorId = scaleFactorId;
            OmegaHSqr = omegaHSqr;
        }

        RelicDensity(){
            Id = boost::uuids::random_generator()();
            InputId = boost::uuids::nil_generator()();
            ParticleId = boost::uuids::nil_generator()();
            ParticleEvolutionId = boost::uuids::nil_generator()();
            ScaleFactorId = boost::uuids::nil_generator()();
        }
    };
}

#endif
