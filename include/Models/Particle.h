#ifndef ParticleProperties_h
#define ParticleProperties_h

#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/CoherentOsc.h>
#include <Models/ProductionMechanism.h>

namespace Models{
    enum class GaugeGroup{ None, Electromagnetic, Color, RParity };
    enum class ParticleStatistics { None, Boson, Fermion };

    struct Charge{
        GaugeGroup Group = GaugeGroup::None;
        double Value = 0.;
    };

    struct Particle
    {
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        int PdgCode = 0.;
        double Mass = 0.;
        double Width = 0.;
        std::string Name = "";
        std::string Key = "";
        std::vector<Charge> Charges;
        ParticleStatistics Statistics = ParticleStatistics::None;
        CoherentOsc CohOscComponents;
        double Spin = 0.;
        boost::uuids::uuid ScaleFactorId;

        Particle()
        : 
            Id(boost::uuids::random_generator()()),
            InputId(boost::uuids::nil_generator()()),
            ScaleFactorId(boost::uuids::nil_generator()())
        {
        }
    };
}

#endif