#include <ParticleDefs/Saxion/SaxionSigV.h>

SaxionSigV::SaxionSigV(/* args */){}
SaxionSigV::~SaxionSigV(){ }

SigmaV SaxionSigV::Calculate(const Models::Particle& saxion, const ModelBase& model, double T, std::shared_ptr< DataRelay > fortInterface){
    double crossSec = 0;
    switch (model.PQSector.AxionModel){
        case DFSZ:
            crossSec = SigVDFSZ::Calculate(T, saxion.Mass, model.StandardModel.Couplings.gSU2, 2., model.Susy.Couplings.mu, model.PQSector.Potential.vPQ);
            break;
        default:
            handleInvalidModel();
    }
    return SigmaV( saxion, T, crossSec);
}
