#include <ParticleDefs/Modulus/ModulusFactory.h>

ModulusBranchingRatioFactory::ModulusBranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle >& particles) 
    : BranchingRatioFactory(logger, parent, model, particles)
{
}
ModulusBranchingRatioFactory::~ModulusBranchingRatioFactory(){}

std::shared_ptr< IBranchingRatio > ModulusBranchingRatioFactory::create_branching_ratio(){ return std::make_shared< ModulusBr >(logger_, parent_, model_, particles_); }
