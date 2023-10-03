#include <cmd/TempFreezeout/Receiver.h>

TempFreezeoutReceiver::TempFreezeoutReceiver(Connection& connection, Models::Particle& particle) :
    connection_(connection),
    particle_(particle)
{
}
TempFreezeoutReceiver::~TempFreezeoutReceiver(){
}

void TempFreezeoutReceiver::Calculate(){
    // TODO: calculate more precise value than this rough estimate
    tempFreezeout_ = abs( particle_.Mass ) / 20.;
}

double TempFreezeoutReceiver::getTempFreezeout(){
    return tempFreezeout_;
}
