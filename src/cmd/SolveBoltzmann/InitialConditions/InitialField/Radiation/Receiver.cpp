#include <cmd/SolveBoltzmann/InitialConditions/InitialField/Radiation/Receiver.h>

using namespace std;

RadiationReceiver::RadiationReceiver(Connection& connection, std::shared_ptr< Models::ScaleFactorPoint > initialPoint, const Models::Particle& particle) :
    connection_(connection)
{
    particle_ = particle;
    initialPoint_ = initialPoint;
    boltzmannParticleEvolution_.InputId = connection_.InputId;
    boltzmannParticleEvolution_.ParticleId = particle_.Id;
    boltzmannParticleEvolution_.ParticleKey = particle_.Key;
    boltzmannParticleEvolution_.ProductionMechanism = ParticleProductionMechanism::RADIATION;
    boltzmannParticleEvolution_.ScaleFactorId = initialPoint_ -> Id;
}

RadiationReceiver::~RadiationReceiver(){}

void RadiationReceiver::Calculate(){
    // start particle in active state
    boltzmannParticleEvolution_.IsActive = true;
    boltzmannParticleEvolution_.IsDecaying = false;
    
    boltzmannParticleEvolution_.Y1 = 0.;
    boltzmannParticleEvolution_.Y2 = 0.;
}

Models::ParticleEvolution RadiationReceiver::getBoltzmannParticleEvolution(){
    return boltzmannParticleEvolution_;
}

