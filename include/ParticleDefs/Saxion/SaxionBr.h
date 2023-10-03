#ifndef SaxionBr_h
#define SaxionBr_h

#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>
#include <ParticleDefs/Saxion/SaxionWidthsDFSZ.h>

class SaxionBr : public IBranchingRatio
{
private:
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> handleDFSZ();
public:
    SaxionBr(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles);
    ~SaxionBr();
    BranchingFraction Calculate() override;
};

#endif
