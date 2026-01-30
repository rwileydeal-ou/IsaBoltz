#include <cmd/DeltaNeff/Receiver.h>

DeltaNeffReceiver::DeltaNeffReceiver( 
    Connection& connection, 
    const Models::ParticleEvolution& particleEvo 
) :
    connection_(connection),
    particleEvo_(particleEvo)
{

}

DeltaNeffReceiver::~DeltaNeffReceiver(){

}

void DeltaNeffReceiver::Calculate(){
    double dneff = calculateDeltaNeff();
    deltaNeff_ = Models::DeltaNeff(dneff, particleEvo_);
}

Models::DeltaNeff DeltaNeffReceiver::getDeltaNeff(){
    return deltaNeff_;
}
