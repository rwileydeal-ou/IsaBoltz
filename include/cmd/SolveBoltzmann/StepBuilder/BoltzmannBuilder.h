#ifndef BoltzmannBuilder_h
#define BoltzmannBuilder_h

#include <deque>
#include <unordered_map>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <Models/ScaleFactor.h>
#include <Models/Connection.h>
#include <Models/ParticleEvolution.h>
#include <Models/PartialWidth.h>

#include <cmd/SolveBoltzmann/StepBuilder/Data.h>
#include <cmd/SolveBoltzmann/StepBuilder/ParticleData.h>

#include <sql/DbManager.h>
#include <sql/Statements/Particle.h>
#include <sql/Filters/Particle.h>
#include <sql/Callbacks/Particle.h>

struct ComponentBuilder{
    boost::uuids::uuid EvolutionId = boost::uuids::nil_generator()();
    long double NumberDensityEquation = 0.;
    std::deque<long double, boost::pool_allocator<long double>> NumberDensityJacobian;
    long double EnergyDensityEquation = 0.;
    std::deque<long double, boost::pool_allocator<long double>> EnergyDensityJacobian;
    long double EntropyEquation = 0.;
    std::deque<long double, boost::pool_allocator<long double>> EntropyJacobian;

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
    DbManager db_;

    long double nRegularizer_ = 1e-100L;
    mutable std::unordered_map<
        boost::uuids::uuid,
        Models::Particle,
        boost::hash<boost::uuids::uuid>
    > particleCache_;

    template <typename T>
    static T handleOverflow(T input, Logger& logger);
    double getPN(double r, double m);

    double Br(const ParticleData& parent, const ParticleData& daughter);
    double Br_Rodd_to_LSP( const ParticleData& parent, const ParticleData& daughter );
    double Br_Rodd_to_Rodd( const ParticleData& parent, const ParticleData& daughter );
    double Br_Rodd_to_Reven( const ParticleData& parent, const ParticleData& daughter );
    double Br_Rodd_to_radiation( const ParticleData& parent, const ParticleData& daughter );
    double Br_Reven_to_Reven( const ParticleData& parent, const ParticleData& daughter );
    double Br_Reven_to_radiation( const ParticleData& parent, const ParticleData& daughter );
    double Br_Reven_to_LSP( const ParticleData& parent, const ParticleData& daughter );
    double Br_Reven_to_Rodd( const ParticleData& parent, const ParticleData& daughter );

    ComponentBuilder calculate_injection_contribution(const ParticleData& parent, const ParticleData& daughter, const long double& hubble);
    ComponentBuilder calculate_hubble_contribution(const ParticleData &particle);

    // These are all helper functions for the decay/inverse decay terms
    ComponentBuilder calculate_inverse_decay(const ParticleData& parent, const ParticleData& daughter, const long double& hubble);
    ComponentBuilder calculate_thermal_particle_decays(const ParticleData& parent, const long double& hubble );
    ComponentBuilder calculate_cohOsc_field_decay(const ParticleData& parent, const long double& hubble );
    ComponentBuilder calculate_decay_terms(const ParticleData& parent);

    ComponentBuilder calculate_annihilation_term(const ParticleData& parent, const ParticleData& rad );

    ComponentBuilder calculate_injection_terms(const ParticleData& parent);

    ComponentBuilder calculate_pressure_term(const ParticleData& parent);

    ComponentBuilder calculate_particle_entropy(const double& t, const ParticleData& particle, const ParticleData& rad);

    const Models::Particle& pullParticle( const boost::uuids::uuid& particleId );
public:
    ComponentBuilder Build_Particle_Boltzmann_Eqs(const double& t, const ParticleData& particle, const ParticleData &rad);
    BoltzmannBuilder( Connection& connection, const BoltzmannData& data );
    ~BoltzmannBuilder();
};

#endif
