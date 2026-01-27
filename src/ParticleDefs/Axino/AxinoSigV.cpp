#include <ParticleDefs/Axino/AxinoSigV.h>

AxinoSigV::AxinoSigV(/* args */){}
AxinoSigV::~AxinoSigV(){ }

SigmaV AxinoSigV::Calculate(
    const Models::Particle& axino, 
    const ModelBase& model, 
    double T, 
    std::shared_ptr< DataRelay > fortInterface
){
    double crossSec = 0;
    switch (model.PQSector.AxionModel){
        case DFSZ:
            crossSec = SigVDFSZ::Calculate(T, axino.Mass, model.StandardModel.Couplings.gSU2, 2., model.Susy.Couplings.mu, model.PQSector.Potential.vPQ);
            break;
        default:
            handleInvalidModel();
    }
    return SigmaV( axino, T, crossSec);
}
