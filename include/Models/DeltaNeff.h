#ifndef DeltaNeff_h
#define DeltaNeff_h

#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/Particle.h>
#include <Models/ParticleEvolution.h>

namespace Models{
    struct DeltaNeff
    {
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        boost::uuids::uuid ParentId;
        boost::uuids::uuid ParticleId;
        boost::uuids::uuid ParticleEvolutionId;
        boost::uuids::uuid ScaleFactorId;
        std::vector< std::vector< boost::uuids::uuid > > ChildrenIds;
        double Delta_Neff = 0.;

        DeltaNeff(){
            Id = boost::uuids::random_generator()();
        }
        DeltaNeff(boost::uuids::uuid const& parentId, std::vector< std::vector< boost::uuids::uuid > > const& childrenIds, double const& delta_Neff, boost::uuids::uuid inputId){
            for (auto &c : childrenIds){
                ChildrenIds.push_back( c );
            }
            Id = boost::uuids::random_generator()();
            ParentId = parentId;
            InputId = inputId;
            ScaleFactorId = boost::uuids::nil_generator()();
            ParticleId = boost::uuids::nil_generator()();
            ParticleEvolutionId = boost::uuids::nil_generator()();
            Delta_Neff = delta_Neff;
        }
        DeltaNeff(double deltaNeff, const Models::ParticleEvolution& particleEvo){
            Id = boost::uuids::random_generator()();
            InputId = particleEvo.InputId;
            ScaleFactorId = particleEvo.ScaleFactorId;
            ParticleId = particleEvo.ParticleId;
            ParticleEvolutionId = particleEvo.Id;
            Delta_Neff = deltaNeff;
            ParentId = boost::uuids::nil_generator()();
        }
    };
}

#endif
