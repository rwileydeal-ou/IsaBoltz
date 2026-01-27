#ifndef GravitinoBr_h
#define GravitinoBr_h

#include <ParticleDefs/Gravitino/GravitinoWidths.h>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>

class GravitinoBr : public IBranchingRatio
{
public:
    GravitinoBr(
        Logger& logger, 
        const Models::Particle& parent, 
        const ModelBase& model, 
        const std::vector< Models::Particle >& particles
    );
    ~GravitinoBr();
    BranchingFraction Calculate() override;
};

#endif
