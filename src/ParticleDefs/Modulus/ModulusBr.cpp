#include <ParticleDefs/Modulus/ModulusBr.h>

ModulusBr::ModulusBr(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle >& particles) :
    IBranchingRatio(logger, parent, model, particles)
{

}

ModulusBr::~ModulusBr(){}

BranchingFraction ModulusBr::Calculate(){
    BranchingFraction br;
    ModulusWidths widths(model_, particles_);
    br.PartialWidths = widths.Decays; 
    br.TotalWidth = calculateTotalWidth(widths.Decays);
    return br;
}
