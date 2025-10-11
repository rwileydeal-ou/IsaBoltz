#ifndef CheckBBN_h
#define CheckBBN_h

#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/ScaleFactor.h>
#include <Models/Particle.h>

namespace Models{
    struct CheckBBN
    {
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        boost::uuids::uuid ParticleId;
        boost::uuids::uuid ParticleEvolutionId;
        boost::uuids::uuid ScaleFactorId;
        double BrHadronic = 0.;
        double DensityConstraint = 0.;
        double DensityCalculated = 0.;
        bool ConstraintSatisfied = false;

        CheckBBN(const CheckBBN &checkBBN){
            Id = checkBBN.Id;
            InputId = checkBBN.InputId;
            ParticleId = checkBBN.ParticleId;
            ParticleEvolutionId = checkBBN.ParticleEvolutionId;
            ScaleFactorId = checkBBN.ScaleFactorId;
            BrHadronic = checkBBN.BrHadronic;
            DensityConstraint = checkBBN.DensityConstraint;
            DensityCalculated = checkBBN.DensityCalculated;
            ConstraintSatisfied = checkBBN.ConstraintSatisfied;
        }

        CheckBBN( const Models::ScaleFactorPoint& point, const Particle& particle, double densityConstraint, bool constraintSatisfied ){
            Id = boost::uuids::random_generator()();
            InputId = point.InputId;
            ParticleId = particle.Id;
            ScaleFactorId = point.Id;
            DensityConstraint = densityConstraint;
            ConstraintSatisfied = constraintSatisfied;
        }

        CheckBBN( const Models::ScaleFactorPoint& point, const boost::uuids::uuid& particleId, double densityConstraint, bool constraintSatisfied ){
            Id = boost::uuids::random_generator()();
            InputId = point.InputId;
            ParticleId = particleId;
            ScaleFactorId = point.Id;
            DensityConstraint = densityConstraint;
            ConstraintSatisfied = constraintSatisfied;
        }

        CheckBBN& operator = (const CheckBBN &checkBBN){
            Id = checkBBN.Id;
            InputId = checkBBN.InputId;
            ParticleId = checkBBN.ParticleId;
            ParticleEvolutionId = checkBBN.ParticleEvolutionId;
            ScaleFactorId = checkBBN.ScaleFactorId;
            BrHadronic = checkBBN.BrHadronic;
            DensityConstraint = checkBBN.DensityConstraint;
            DensityCalculated = checkBBN.DensityCalculated;
            ConstraintSatisfied = checkBBN.ConstraintSatisfied;
            return *this;
        }

        CheckBBN(){
            Id = boost::uuids::random_generator()();
            InputId = boost::uuids::nil_generator()();
            ParticleId = boost::uuids::nil_generator()();
            ParticleEvolutionId = boost::uuids::nil_generator()();
            ScaleFactorId = boost::uuids::nil_generator()();
        }
    };
}

#endif
