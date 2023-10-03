#include <ParticleDefs/Axino/AxinoFactory.h>

AxinoBranchingRatioFactory::AxinoBranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles) : 
    BranchingRatioFactory(logger, parent, model, particles)
{}
AxinoBranchingRatioFactory::~AxinoBranchingRatioFactory(){}

std::shared_ptr< IBranchingRatio > AxinoBranchingRatioFactory::create_branching_ratio(){ return std::make_shared< AxinoBr >(logger_, parent_, model_, particles_); }
std::shared_ptr< ISigmaV > AxinoCrossSectionFactory::create_cross_section(){ return std::make_shared< AxinoSigV >(); }
