#ifndef TempEquality_h
#define TempEquality_h

#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/ProductionMechanism.h>

namespace Models{
    struct TempEquality
    {
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        boost::uuids::uuid ParticleId;
        boost::uuids::uuid ScaleFactorId;
        ParticleProductionMechanism ProductionMechanism;
        double Temperature = 0.;
        double GStar = 0.;
        double GStarEntropic = 0.;

        TempEquality(const TempEquality &temp){
            Id = temp.Id;
            InputId = temp.InputId;
            ParticleId = temp.ParticleId;
            ScaleFactorId = temp.ScaleFactorId;
            ProductionMechanism = temp.ProductionMechanism;
            Temperature = temp.Temperature;
            GStar = temp.GStar;
            GStarEntropic = temp.GStarEntropic;
        }

        TempEquality& operator = (const TempEquality &temp){
            Id = temp.Id;
            InputId = temp.InputId;
            ParticleId = temp.ParticleId;
            ScaleFactorId = temp.ScaleFactorId;
            ProductionMechanism = temp.ProductionMechanism;
            Temperature = temp.Temperature;
            GStar = temp.GStar;
            GStarEntropic = temp.GStarEntropic;
            return *this;
        }

        TempEquality(){
            Id = boost::uuids::random_generator()();
            InputId = boost::uuids::nil_generator()();
            ParticleId = boost::uuids::nil_generator()();
            ScaleFactorId = boost::uuids::nil_generator()();
        }
    };
}

#endif
