#include <ParticleDefs/Saxion/SaxionFactory.h>

SaxionBranchingRatioFactory::SaxionBranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles) : 
    BranchingRatioFactory(logger, parent, model, particles)
{
}
SaxionBranchingRatioFactory::~SaxionBranchingRatioFactory(){}

std::shared_ptr< IBranchingRatio > SaxionBranchingRatioFactory::create_branching_ratio(){ return std::make_shared< SaxionBr >(logger_, parent_, model_, particles_); }
std::shared_ptr< ISigmaV > SaxionCrossSectionFactory::create_cross_section(){ return std::make_shared< SaxionSigV >(); }
