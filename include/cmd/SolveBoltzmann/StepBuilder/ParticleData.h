#ifndef ParticleData_h
#define ParticleData_h

#include <string>
#include <boost/uuid/uuid.hpp>
#include <Models/TotalWidth.h>
#include <Models/ProductionMechanism.h>
#include <Models/Particle.h>
#include <Models/ParticleEvolution.h>

struct ParticleData
{
    // Universal data
    std::string ParticleKey;
    boost::uuids::uuid ParticleId;
    boost::uuids::uuid ParticleEvolutionId;
    boost::uuids::uuid InputId;
    boost::uuids::uuid ScaleFactorId;

    // Evolution object
    ParticleProductionMechanism ProductionMechanism;
    long double Y1 = 0.;
    long double Y2 = 0.;
    long double NumberDensity = 0.;
    long double EnergyDensity = 0.;
    long double EquilibriumNumberDensity = 0.;

    // Particle object
    int PdgCode = 0;
    Models::ParticleStatistics Statistics;
    std::vector<Models::Charge> Charges;
    double Mass = 0.;
    bool TempDependentMass = false;
    boost::uuids::uuid PreviousTempParticleId;
    double Spin = 0.;

    // Utility values needed
    double TotalWidth = 0.;
    double AnnihilationCrossSection = 0.;
    int EqnIndex = -1;
    double Amplitude = 0.;

    // Switches
    bool IsActive;
    bool IsOscillating;
    bool IsDecaying;
    bool IsLSP;
    bool TurnOff = false;

    ParticleData(){
    }

    ParticleData( 
        const ParticleData& particleData 
    ) :
        ParticleKey( particleData.ParticleKey ),
        ParticleId( particleData.ParticleId ),
        ParticleEvolutionId( particleData.ParticleEvolutionId ),
        InputId( particleData.InputId ),
        ScaleFactorId( particleData.ScaleFactorId ),
        ProductionMechanism( particleData.ProductionMechanism ),
        Y1( particleData.Y1 ),
        Y2( particleData.Y2 ),
        NumberDensity( particleData.NumberDensity ),
        EnergyDensity( particleData.EnergyDensity ),
        EquilibriumNumberDensity( particleData.EquilibriumNumberDensity ),
        PdgCode( particleData.PdgCode ),
        Statistics( particleData.Statistics ),
        Charges( particleData.Charges ),
        Mass( particleData.Mass ),
        TempDependentMass( particleData.TempDependentMass ),
        Spin( particleData.Spin ),
        TotalWidth( particleData.TotalWidth ),
        AnnihilationCrossSection( particleData.AnnihilationCrossSection ),
        EqnIndex( particleData.EqnIndex ),
        IsActive( particleData.IsActive ),
        IsOscillating( particleData.IsOscillating ),
        IsDecaying( particleData.IsDecaying ),
        IsLSP( particleData.IsLSP ),
        TurnOff( particleData.TurnOff ),
        PreviousTempParticleId( particleData.PreviousTempParticleId ),
        Amplitude( particleData.Amplitude )
    {
    }

    ParticleData( const Models::ParticleEvolution& particleEvo ){
        ParticleKey = particleEvo.ParticleKey;

        ParticleId = particleEvo.ParticleId;
        ParticleEvolutionId = particleEvo.Id;
        InputId = particleEvo.InputId;
        ScaleFactorId = particleEvo.ScaleFactorId;

        ProductionMechanism = particleEvo.ProductionMechanism;
        Y1 = particleEvo.Y1;
        Y2 = particleEvo.Y2;
        NumberDensity = particleEvo.NumberDensity;
        EnergyDensity = particleEvo.EnergyDensity;
        EquilibriumNumberDensity = particleEvo.EquilibriumNumberDensity;

        IsActive = particleEvo.IsActive;
        IsOscillating = particleEvo.IsOscillating;
        IsDecaying = particleEvo.IsDecaying;
    }
};


#endif
