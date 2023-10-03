#include <ParticleDefs/Axino/AxinoBr.h>

using namespace std;

AxinoBr::AxinoBr(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles) :
    IBranchingRatio(logger, parent, model, particles)
{}
AxinoBr::~AxinoBr(){}

BranchingFraction AxinoBr::Calculate(){
    BranchingFraction br;
    switch (model_.PQSector.AxionModel){
        case DFSZ:
            br.PartialWidths = handleDFSZ();
            break;
        default:
            handleInvalidModel();
    }
    br.TotalWidth = calculateTotalWidth(br.PartialWidths);
    return br;
}

deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > AxinoBr::handleDFSZ(){
    AxinoWidthsDFSZ dfszWidths(model_, particles_);
    return dfszWidths.Decays;
}
