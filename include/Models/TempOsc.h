#ifndef TempOscillate_h
#define TempOscillate_h

#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/ScaleFactor.h>
#include <Models/ParticleEvolution.h>

namespace Models{
    struct TempOscillation
    {
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        boost::uuids::uuid ParticleId;
        boost::uuids::uuid ScaleFactorId;
        double Temperature = 0.;
        double GStar = 0.;
        double GStarEntropic = 0.;

        TempOscillation(const Models::ScaleFactorPoint& point, const ParticleEvolution& pEvo){
            Id = boost::uuids::random_generator()();
            InputId = pEvo.InputId;
            ParticleId = pEvo.ParticleId;
            ScaleFactorId = pEvo.ScaleFactorId;
            Temperature = point.Temperature;
            GStar = point.GStar;
            GStarEntropic = point.GStarEntropic;
        }

        TempOscillation(const Models::ScaleFactorPoint& point, const boost::uuids::uuid& particleId){
            Id = boost::uuids::random_generator()();
            InputId = point.InputId;
            ParticleId = particleId;
            ScaleFactorId = point.Id;
            Temperature = point.Temperature;
            GStar = point.GStar;
            GStarEntropic = point.GStarEntropic;
        }

        TempOscillation(){
            Id = boost::uuids::random_generator()();
            InputId = boost::uuids::nil_generator()();
            ParticleId = boost::uuids::nil_generator()();
            ScaleFactorId = boost::uuids::nil_generator()();
        }
    };
}

#endif
