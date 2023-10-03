#ifndef ParticleEvolution_h
#define ParticleEvolution_h

#include <vector>
#include <ModelIndependent/BranchingRatios/BranchingFraction.h>
#include <ModelIndependent/CrossSections/SigmaV.h>
#include <ModelIndependent/Densities/ParticleDensity.h>
#include <Models/CoherentOsc.h>

namespace Models{
    struct ParticleEvolution{
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId = boost::uuids::nil_generator()();
        std::string ParticleKey;
        boost::uuids::uuid ParticleId = boost::uuids::nil_generator()();
        ParticleProductionMechanism ProductionMechanism; 
        bool IsActive;
        long double Y1 = 0.;      // this is Log(S/S0) for radiation, Log(n/s0) for all other particles
        long double Y2 = 0.;      // this is Log( rho / (n*T_R) ) for non-radiation particles
        long double NumberDensity = 0.; 
        long double EquilibriumNumberDensity = 0.;
        long double EnergyDensity = 0.;
        boost::uuids::uuid ScaleFactorId = boost::uuids::nil_generator()();
        bool IsOscillating;
        bool IsDecaying;

        ParticleEvolution(const ParticleEvolution &p){
            Y1 = p.Y1;
            Y2 = p.Y2;
            IsActive = p.IsActive;
            NumberDensity = p.NumberDensity;
            EquilibriumNumberDensity = p.EquilibriumNumberDensity;
            EnergyDensity = p.EnergyDensity;
            IsOscillating = p.IsOscillating;
            IsDecaying = p.IsDecaying;

            Id = p.Id;
            InputId = p.InputId;
            ParticleId = p.ParticleId;
            ScaleFactorId = p.ScaleFactorId;
            ParticleKey = p.ParticleKey;
            ProductionMechanism = p.ProductionMechanism;
        }
        ParticleEvolution& operator = (const ParticleEvolution &p){
            Y1 = p.Y1;
            Y2 = p.Y2;
            IsActive = p.IsActive;
            NumberDensity = p.NumberDensity;
            EquilibriumNumberDensity = p.EquilibriumNumberDensity;
            EnergyDensity = p.EnergyDensity;
            IsOscillating = p.IsOscillating;
            IsDecaying = p.IsDecaying;
            
            Id = p.Id;
            InputId = p.InputId;
            ParticleId = p.ParticleId;
            ScaleFactorId = p.ScaleFactorId;
            ParticleKey = p.ParticleKey;
            ProductionMechanism = p.ProductionMechanism;
            
            return *this;
        }
        ParticleEvolution(){
            Id = boost::uuids::random_generator()();
            InputId = boost::uuids::nil_generator()();
        }
    };
}

#endif