#include <cmd/RelicDensity/Receiver.h>

RelicDensityReceiver::RelicDensityReceiver( Connection& connection, const Models::ParticleEvolution& particleEvo, const Models::Particle& particle, const Models::ScaleFactorPoint& finalScaleFactorPoint ) :
    connection_(connection),
    particleEvo_(particleEvo),
    particle_(particle),
    finalScaleFactorPoint_(finalScaleFactorPoint)
{

}

RelicDensityReceiver::~RelicDensityReceiver(){

}


void RelicDensityReceiver::Calculate(){
    double omegaHSqr = calculateRelicDensity();
    relicDensity_ = Models::RelicDensity(omegaHSqr, particleEvo_);
}

Models::RelicDensity RelicDensityReceiver::getRelicDensity(){
    return relicDensity_;
}

