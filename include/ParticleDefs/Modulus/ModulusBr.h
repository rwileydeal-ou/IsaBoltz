#ifndef ModulusBr_h
#define ModulusBr_h

#include <ParticleDefs/Modulus/ModulusWidths.h>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>

class ModulusBr : public IBranchingRatio
{
public:
    ModulusBr(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles);
    ~ModulusBr();
    BranchingFraction Calculate() override;
};

#endif
