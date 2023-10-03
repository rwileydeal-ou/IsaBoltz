#ifndef BranchingRatioFactory_h
#define BranchingRatioFactory_h

#include <memory>
#include <boost/pool/pool_alloc.hpp>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>

class BranchingRatioFactory{
protected:
    Logger& logger_;
    const Models::Particle& parent_;
    const ModelBase& model_;
    const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles_;
public:
    BranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles);
    ~BranchingRatioFactory();
    virtual std::shared_ptr< IBranchingRatio > create_branching_ratio() = 0;
};

class DefaultBranchingRatioFactory : public BranchingRatioFactory {
public:
    DefaultBranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles);
    ~DefaultBranchingRatioFactory();
    std::shared_ptr< IBranchingRatio > create_branching_ratio() override;
};

#endif
