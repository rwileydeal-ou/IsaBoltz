#include <ModelIndependent/BranchingRatios/BranchingRatioFactory.h>

BranchingRatioFactory::BranchingRatioFactory(
    Logger& logger, 
    const Models::Particle& parent, 
    const ModelBase& model, 
    const std::vector< Models::Particle >& particles
) :
    logger_(logger),
    parent_(parent),
    model_(model),
    particles_(particles)
{
}
BranchingRatioFactory::~BranchingRatioFactory(){}

DefaultBranchingRatioFactory::DefaultBranchingRatioFactory(
    Logger& logger, 
    const Models::Particle& parent, 
    const ModelBase& model, 
    const std::vector< Models::Particle >& particles
) :
    BranchingRatioFactory(logger, parent, model, particles)
{}
DefaultBranchingRatioFactory::~DefaultBranchingRatioFactory(){}

std::shared_ptr< IBranchingRatio > DefaultBranchingRatioFactory::create_branching_ratio(){ 
    return std::make_shared< DefaultBranchingRatio >(logger_, parent_, model_, particles_); 
}
