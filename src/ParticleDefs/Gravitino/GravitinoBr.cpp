#include <ParticleDefs/Gravitino/GravitinoBr.h>

GravitinoBr::GravitinoBr(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle >& particles) 
    : IBranchingRatio(logger, parent, model, particles)
{
}
GravitinoBr::~GravitinoBr(){}

BranchingFraction GravitinoBr::Calculate(){
    BranchingFraction br;
    GravitinoWidths widths(model_, particles_);
    br.PartialWidths = widths.Decays;
    br.TotalWidth = calculateTotalWidth(widths.Decays);
    return br; 
}
