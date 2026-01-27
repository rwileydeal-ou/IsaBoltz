#ifndef Coupling_h
#define Coupling_h

#include <vector>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/Particle.h>

namespace Models{
    struct Coupling
    {
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        std::string Key;
        double Value = 0.;

        Coupling(){
            Id = boost::uuids::random_generator()();
        }
        Coupling(std::string key, double const& value, boost::uuids::uuid inputId){
            Id = boost::uuids::random_generator()();
            InputId = inputId;
            Value = value;
            Key = key;
        }
    };
}

#endif
