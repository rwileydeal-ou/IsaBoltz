#ifndef TempDecay_h
#define TempDecay_h

#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/ScaleFactor.h>
#include <Models/Particle.h>

namespace Models{
    struct TempDecay
    {
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        boost::uuids::uuid ParticleId;
        boost::uuids::uuid ScaleFactorId;
        double Temperature = 0.;
        double GStar = 0.;
        double GStarEntropic = 0.;

        TempDecay(const TempDecay &temp){
            Id = temp.Id;
            InputId = temp.InputId;
            ParticleId = temp.ParticleId;
            ScaleFactorId = temp.ScaleFactorId;
            Temperature = temp.Temperature;
            GStar = temp.GStar;
            GStarEntropic = temp.GStarEntropic;
        }

        TempDecay( const Models::ScaleFactorPoint& point, const Particle& particle ){
            Id = boost::uuids::random_generator()();
            InputId = point.InputId;
            ParticleId = particle.Id;
            ScaleFactorId = point.Id;
            Temperature = point.Temperature;
            GStar = point.GStar;
            GStarEntropic = point.GStarEntropic;
        }

        TempDecay( const Models::ScaleFactorPoint& point, const boost::uuids::uuid& particleId ){
            Id = boost::uuids::random_generator()();
            InputId = point.InputId;
            ParticleId = particleId;
            ScaleFactorId = point.Id;
            Temperature = point.Temperature;
            GStar = point.GStar;
            GStarEntropic = point.GStarEntropic;
        }

        TempDecay& operator = (const TempDecay &temp){
            Id = temp.Id;
            InputId = temp.InputId;
            ParticleId = temp.ParticleId;
            ScaleFactorId = temp.ScaleFactorId;
            Temperature = temp.Temperature;
            GStar = temp.GStar;
            GStarEntropic = temp.GStarEntropic;
            return *this;
        }

        TempDecay(){
            Id = boost::uuids::random_generator()();
            InputId = boost::uuids::nil_generator()();
            ParticleId = boost::uuids::nil_generator()();
            ScaleFactorId = boost::uuids::nil_generator()();
        }
    };
}

#endif
