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
    enum GaugeGroup{ Electromagnetic, Color, RParity };
    enum ParticleStatistics { Boson, Fermion };

    struct Charge{
        GaugeGroup Group;
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
        ParticleStatistics Statistics;
        CoherentOsc CohOscComponents;
        double Spin = 0.;
        boost::uuids::uuid ScaleFactorId;
        Particle(const Particle &p){
            Id = p.Id;
            InputId = p.InputId;
            PdgCode = p.PdgCode;
            Mass = p.Mass;
            Width = p.Width;
            Name = p.Name;
            Key = p.Key;
            for (auto &charge : p.Charges){
                Charges.push_back(charge);
            }
            Statistics = p.Statistics;
            Spin = p.Spin;
            CohOscComponents = p.CohOscComponents;
            ScaleFactorId = p.ScaleFactorId;
        }
        Particle& operator = (const Particle &p){
            Id = p.Id;
            InputId = p.InputId;
            PdgCode = p.PdgCode;
            Mass = p.Mass;
            Width = p.Width;
            Name = p.Name;
            Key = p.Key;
            for (auto charge : p.Charges){
                Charges.push_back(charge);
            }
            Statistics = p.Statistics;
            Spin = p.Spin;
            CohOscComponents = p.CohOscComponents;
            ScaleFactorId = p.ScaleFactorId;
            return *this;
        }
        Particle(){
            Id = boost::uuids::random_generator()();
            InputId = boost::uuids::nil_generator()();
            ScaleFactorId = boost::uuids::nil_generator()();
        }
    };
}

#endif