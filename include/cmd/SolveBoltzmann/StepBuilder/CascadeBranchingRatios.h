#ifndef CascadeBranchingRatios_h
#define CascadeBranchingRatios_h

#include <unordered_map>
#include <Models/Connection.h>
#include <cmd/SolveBoltzmann/StepBuilder/Data.h>
#include <cmd/SolveBoltzmann/StepBuilder/ParticleData.h>
#include <sql/DbManager.h>
#include <sql/Statements/Particle.h>
#include <sql/Filters/Particle.h>
#include <sql/Callbacks/Particle.h>

class CascadeBranchingRatios
{
private:
    Connection& connection_;
    const BoltzmannData& data_;
    DbManager& db_;

    std::unordered_map<
        boost::uuids::uuid,
        Models::Particle,
        boost::hash<boost::uuids::uuid>
    >& particleCache_;

    double Br_Rodd_to_LSP( 
        const ParticleData& parent, 
        const ParticleData& daughter 
    );
    double Br_Rodd_to_Rodd( 
        const ParticleData& parent, 
        const ParticleData& daughter 
    );
    double Br_Rodd_to_Reven( 
        const ParticleData& parent, 
        const ParticleData& daughter 
    );
    double Br_Rodd_to_radiation( 
        const ParticleData& parent, 
        const ParticleData& daughter 
    );
    double Br_Reven_to_Reven( 
        const ParticleData& parent, 
        const ParticleData& daughter 
    );
    double Br_Reven_to_radiation( 
        const ParticleData& parent, 
        const ParticleData& daughter 
    );
    double Br_Reven_to_LSP( 
        const ParticleData& parent, 
        const ParticleData& daughter 
    );
    double Br_Reven_to_Rodd( 
        const ParticleData& parent, 
        const ParticleData& daughter 
    );
    const Models::Particle& pullParticle( 
        const boost::uuids::uuid& particleId 
    );
public:
    double Calculate(
        const ParticleData& parent, 
        const ParticleData& daughter
    );
    CascadeBranchingRatios( 
        Connection& connection, 
        DbManager& db,
        const BoltzmannData& data, 
        std::unordered_map< 
            boost::uuids::uuid, 
            Models::Particle, 
            boost::hash<boost::uuids::uuid> 
        >& particleCache 
    );
    ~CascadeBranchingRatios();
};

#endif
