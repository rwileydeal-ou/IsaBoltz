#include <ParticleDefs/Neutralino/NeutralinoBr.h>

NeutralinoBr::NeutralinoBr(
    Logger& logger, 
    const Models::Particle& parent, 
    const ModelBase& model, 
    const std::vector< Models::Particle >& particles
) :
    IBranchingRatio(logger, parent, model, particles)
{

}

NeutralinoBr::~NeutralinoBr(){}

BranchingFraction NeutralinoBr::Calculate(){
    BranchingFraction br;
    NeutralinoWidths widths(model_, particles_);
    br.PartialWidths = widths.Decays;
    br.TotalWidth = calculateTotalWidth(widths.Decays);
    return br;
}
