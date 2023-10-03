#ifndef NeutralinoBr_h
#define NeutralinoBr_h

#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>
#include <ParticleDefs/Neutralino/NeutralinoWidths.h>

class NeutralinoBr : public IBranchingRatio
{
private:
public:
    NeutralinoBr(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles);
    ~NeutralinoBr();
    BranchingFraction Calculate() override;
};

#endif
