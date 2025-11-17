#include <ParticleDefs/Gravitino/GravitinoFactory.h>

GravitinoBranchingRatioFactory::GravitinoBranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle >& particles) :
    BranchingRatioFactory(logger, parent, model, particles)
{
}
GravitinoBranchingRatioFactory::~GravitinoBranchingRatioFactory(){}

std::shared_ptr< IBranchingRatio > GravitinoBranchingRatioFactory::create_branching_ratio(){ return std::make_shared< GravitinoBr >(logger_, parent_, model_, particles_); }
std::shared_ptr< ISigmaV > GravitinoCrossSectionFactory::create_cross_section(){ return std::make_shared< GravitinoSigV >(); }
