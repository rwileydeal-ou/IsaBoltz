#include <ParticleDefs/Neutralino/NeutralinoSigV.h>

using namespace std;

NeutralinoSigV::NeutralinoSigV(){ }
NeutralinoSigV::~NeutralinoSigV(){ }

SigmaV NeutralinoSigV::Calculate(
    const Models::Particle& neutralino, 
    const ModelBase& model, 
    double T, 
    std::shared_ptr< DataRelay > fortInterface
){
    double xF = T / abs(neutralino.Mass);
    // THIS IS DUE TO MACHINE LIMITS, AT LEAST FOR NOW
    // BELOW T~1 GEV, <SIG.V> DROPS TO ~0, SO JUST EXTRACT FROZEN VALUE AS APPROXIMATION
    // THIS HAPPENS BECAUSE THE FORTRAN SUBROUTINE HAS A K1(X)/K2(X) FACTOR AS X->INF
    if (xF < 0.003){
        xF = 0.003;
    }

    auto intResult = fortInterface->CalculateFunctionalIntegral(xF);
    double crossSec = intResult * 2.5682e-9; // convert pb to 1/GeV^2
    return SigmaV( neutralino, T, crossSec);
}
