#include <ParticleDefs/Saxion/SaxionBr.h>

using namespace std;

SaxionBr::SaxionBr(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles) : 
    IBranchingRatio(logger, parent, model, particles)
{}
SaxionBr::~SaxionBr(){}

BranchingFraction SaxionBr::Calculate(){
    BranchingFraction br;
    switch(model_.PQSector.AxionModel){
        case DFSZ:
            br.PartialWidths = handleDFSZ();
            break;
        default:
            handleInvalidModel();
    }
    br.TotalWidth = calculateTotalWidth(br.PartialWidths);
    return br;
}

deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> SaxionBr::handleDFSZ(){
    SaxionWidthsDFSZ dfszWidths(model_, particles_);
    return dfszWidths.Decays;
}
