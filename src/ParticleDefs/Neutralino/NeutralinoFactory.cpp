#include <ParticleDefs/Neutralino/NeutralinoFactory.h>

NeutralinoBranchingRatioFactory::NeutralinoBranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle >& particles) : 
    BranchingRatioFactory(logger, parent, model, particles)
{}
NeutralinoBranchingRatioFactory::~NeutralinoBranchingRatioFactory(){}

std::shared_ptr< IBranchingRatio > NeutralinoBranchingRatioFactory::create_branching_ratio(){ return std::make_shared< NeutralinoBr >(logger_, parent_, model_, particles_); }
std::shared_ptr< ISigmaV > NeutralinoCrossSectionFactory::create_cross_section(){ return std::make_shared< NeutralinoSigV >(); }
