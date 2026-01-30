#ifndef BoltzmannBuilder_h
#define BoltzmannBuilder_h

#include <vector>
#include <iostream>
#include <unordered_map>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <Models/ScaleFactor.h>
#include <Models/Connection.h>
#include <Models/ParticleEvolution.h>
#include <Models/PartialWidth.h>

#include <cmd/SolveBoltzmann/StepBuilder/Data.h>
#include <cmd/SolveBoltzmann/StepBuilder/ParticleData.h>
#include <cmd/SolveBoltzmann/StepBuilder/CascadeBranchingRatios.h>

#include <sql/DbManager.h>
#include <sql/Statements/Particle.h>
#include <sql/Filters/Particle.h>
#include <sql/Callbacks/Particle.h>

struct ComponentBuilder{
    boost::uuids::uuid EvolutionId = boost::uuids::nil_generator()();
    long double NumberDensityEquation = 0.;
    std::vector<long double, boost::pool_allocator<long double>> NumberDensityJacobian;
    long double EnergyDensityEquation = 0.;
    std::vector<long double, boost::pool_allocator<long double>> EnergyDensityJacobian;
    long double EntropyEquation = 0.;
    std::vector<long double, boost::pool_allocator<long double>> EntropyJacobian;

    ComponentBuilder(int size){
        NumberDensityJacobian.resize( size );        
        EnergyDensityJacobian.resize( size );        
        EntropyJacobian.resize( size );        
    }
};

class BoltzmannBuilder
{
private:
    Connection& connection_;
    const BoltzmannData& data_;
    DbManager& db_;

    long double nRegularizer_ = 1e-100L;
    std::unordered_map<
        boost::uuids::uuid,
        Models::Particle,
        boost::hash<boost::uuids::uuid>
    >& particleCache_;

    template <typename T>
    static T handleOverflow(T input, Logger& logger);
    double getPN(double r, double m);

    void calculate_injection_contribution(
        const ParticleData& parent, 
        const ParticleData& daughter, 
        const long double& hubble,
        ComponentBuilder& builder
    );
    void calculate_hubble_contribution(
        const ParticleData &particle,
        ComponentBuilder& builder
    );

    // These are all helper functions for the decay/inverse decay terms
    void calculate_inverse_decay(
        const ParticleData& parent, 
        const ParticleData& daughter, 
        const long double& hubble,
        ComponentBuilder& builder
    );
    void calculate_thermal_particle_decays(
        const ParticleData& parent, 
        const long double& hubble,
        ComponentBuilder& builder
    );
    void calculate_cohOsc_field_decay(
        const ParticleData& parent, 
        const long double& hubble, 
        ComponentBuilder& builder
    );
    void calculate_decay_terms(
        const ParticleData& parent,
        ComponentBuilder& builder
    );

    void calculate_annihilation_term(
        const ParticleData& parent, 
        const ParticleData& rad,
        ComponentBuilder& builder
    );

    void calculate_injection_terms(
        const ParticleData& parent,
        ComponentBuilder& builder
    );

    void calculate_pressure_term(
        const ParticleData& parent,
        ComponentBuilder& builder
    );

    void calculate_particle_entropy(
        const double& t, 
        const ParticleData& particle, 
        const ParticleData& rad,
        ComponentBuilder& builder
    );
public:
    ComponentBuilder Build_Particle_Boltzmann_Eqs(
        const double& t, 
        const ParticleData& particle, 
        const ParticleData &rad
    );
    BoltzmannBuilder( 
        Connection& connection,
        DbManager& db,
        const BoltzmannData& data, 
        std::unordered_map< 
            boost::uuids::uuid, 
            Models::Particle, 
            boost::hash<boost::uuids::uuid> 
        >& particleCache 
    );
    ~BoltzmannBuilder();
};

#endif
