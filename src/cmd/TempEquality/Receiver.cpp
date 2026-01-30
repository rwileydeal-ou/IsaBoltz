#include <cmd/TempEquality/Receiver.h>

TempEqualityReceiver::TempEqualityReceiver(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle, 
    double tempReheat
) :
    connection_(connection),
    db_(db),
    particle_(particle)
{
    tempReheat_ = tempReheat;
    tempEquality_.InputId = connection_.InputId;
    tempEquality_.ParticleId = particle_.Id;
}
TempEqualityReceiver::~TempEqualityReceiver(){
}

void TempEqualityReceiver::Calculate(){
    tempEquality_.Temperature = tempEquality();
    tempEquality_.GStar = GStar::Calculate(db_, connection_, tempEquality_.Temperature);
    tempEquality_.GStarEntropic = GStar::CalculateEntropic(db_, connection_, tempEquality_.Temperature);
}

Models::TempEquality TempEqualityReceiver::getTempEquality(){
    return tempEquality_;
}
