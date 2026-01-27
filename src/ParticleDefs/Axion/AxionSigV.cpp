#include <ParticleDefs/Axion/AxionSigV.h>

AxionSigV::AxionSigV(/* args */){}
AxionSigV::~AxionSigV(){ }

SigmaV AxionSigV::Calculate(
    const Models::Particle& axion, 
    const ModelBase& model, 
    double T, 
    std::shared_ptr< DataRelay > fortInterface
){
    double crossSec = 0;
    switch (model.PQSector.AxionModel){
        case DFSZ:
            // mass for axion case is mu, as per arXiv: 1406.4138
            crossSec = SigVDFSZ::Calculate(T, model.Susy.Couplings.mu, model.StandardModel.Couplings.gSU2, 2., model.Susy.Couplings.mu, model.PQSector.Potential.vPQ);
            break;
        default:
            handleInvalidModel();
    }
    return SigmaV( axion, T, crossSec);
}
