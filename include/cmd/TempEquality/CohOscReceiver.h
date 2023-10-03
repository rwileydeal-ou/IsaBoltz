#ifndef CohOscTempEqualityReceiver_h
#define CohOscTempEqualityReceiver_h

#include <algorithm>
#include <iostream>
#include <cmd/IReceiver.h>
#include <cmd/TempEquality/Receiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <io/Exceptions/NotImplemented.h>
#include <ModelIndependent/GStar.h>
#include <sql/DbManager.h>
#include <sql/Statements/TempOsc.h>
#include <sql/Callbacks/TempOsc.h>
#include <sql/Filters/TempOsc.h>

class CohOscTempEqualityReceiver : public TempEqualityReceiver
{
private: 
    Connection& connection_;
    Models::Particle& particle_;
    Models::TempOscillation pullTempOsc();
    double tempEquality() override;
    double tempEq_greaterThan_tempOsc(double initialGuess);
    double tempEq_lessThan_tempOsc_lessThan_TR(double tempOsc);
    double tempEq_TR_lessThan_Te_lessThan_tempOsc(double initialGuess);
public:
    ~CohOscTempEqualityReceiver();
    CohOscTempEqualityReceiver(Connection& connection, Models::Particle& particle, double tempReheat);
};

#endif
