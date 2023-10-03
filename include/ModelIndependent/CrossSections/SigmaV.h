#ifndef SigmaV_h
#define SigmaV_h

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/Particle.h>

struct SigmaV{
    boost::uuids::uuid Id;
    double Temperature = 0.;
    double CrossSection = 0.;
    boost::uuids::uuid ParticleId;
    boost::uuids::uuid ScaleFactorId;
    boost::uuids::uuid InputId;

    SigmaV(Models::Particle childProcess, double temperature, double crossSection){
        Temperature = temperature;
        ParticleId = childProcess.Id;
        CrossSection = crossSection;
        if (std::isnan(crossSection)){
            CrossSection = 0.;
        }
        Id = boost::uuids::random_generator()();
    }
    SigmaV(Models::Particle childProcess, double temperature, double crossSection, boost::uuids::uuid scaleFactorId){
        Temperature = temperature;
        ParticleId = childProcess.Id;
        CrossSection = crossSection;
        if (std::isnan(crossSection)){
            CrossSection = 0.;
        }
        Id = boost::uuids::random_generator()();
        ScaleFactorId = scaleFactorId;
    }
    SigmaV(){
        Id = boost::uuids::random_generator()();
    }
};

#endif
