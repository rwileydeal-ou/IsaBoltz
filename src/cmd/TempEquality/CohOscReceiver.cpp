#include <cmd/TempEquality/CohOscReceiver.h>

CohOscTempEqualityReceiver::CohOscTempEqualityReceiver(Connection& connection, Models::Particle& particle, double tempReheat) :
    connection_(connection),
    particle_(particle),
    TempEqualityReceiver(connection, particle, tempReheat)
{
    tempEquality_.ProductionMechanism = ParticleProductionMechanism::COHERENT_OSCILLATION;
}
CohOscTempEqualityReceiver::~CohOscTempEqualityReceiver(){
}

// This method calculates Temp_eq for coh. osc. fields if T_e < T_osc < T_R
double CohOscTempEqualityReceiver::tempEq_lessThan_tempOsc_lessThan_TR(double tempOsc){
    double gstr = GStar::Calculate( connection_, tempOsc );
    // here we're assuming that the field has started to oscillate
    double tempEq = 1.5 * pow(
        particle_.CohOscComponents.Amplitude / connection_.Model.Constants.mPlanck, 2.)
        * pow( connection_.Model.Constants.mPlanck * particle_.Mass, 1./2.)
        * pow( 10. / ( pow(M_PI, 2.) * gstr ), 1./4. );
    return tempEq;
}

double CohOscTempEqualityReceiver::tempEq_TR_lessThan_Te_lessThan_tempOsc(double initialGuess){
    double T1 = 0.;
    double dT = 0.;
    double gstrTR = GStar::Calculate(connection_, tempReheat_);
    
    auto a = (initialGuess-T1) / initialGuess;
    while( (initialGuess-T1) / initialGuess > 0.01){
        double gstrTe = GStar::Calculate(connection_, initialGuess);

        T1 = pow( 
            2. * pow(connection_.Model.Constants.mPlanck, 2.) * gstrTR 
            / ( 3. * gstrTe ), 1./4.
        ) * tempReheat_ / sqrt( particle_.CohOscComponents.Amplitude );
        
        if( abs(initialGuess-T1) == dT ){
            initialGuess = (initialGuess + T1) / 2.;
        }
        else{
            dT = abs( initialGuess - T1);
            initialGuess = T1;
        }
    }
    return initialGuess;
}

// This method calculates Temp_eq for coh. osc. fields if T_osc < T_e < T_R
double CohOscTempEqualityReceiver::tempEq_greaterThan_tempOsc(double initialGuess){
    double T1 = 0.;
    double dT = 0.;

    auto a = (initialGuess-T1) / initialGuess;
    while( (initialGuess-T1) / initialGuess > 0.01){
        double gstr = GStar::Calculate(connection_, initialGuess);

        T1 = sqrt( 
            particle_.CohOscComponents.Amplitude * particle_.Mass 
        ) * pow( 15. / ( gstr * pow(M_PI, 2.) ), 1./4.);

        if( abs(initialGuess-T1) == dT ){
            initialGuess = (initialGuess + T1) / 2.;
        }
        else{
            dT = abs( initialGuess - T1);
            initialGuess = T1;
        }
    }
    return initialGuess;
}

Models::TempOscillation CohOscTempEqualityReceiver::pullTempOsc(){
    Models::TempOscillation tempOsc;
 
    DbManager db(connection_);
    db.Open();
    auto statement = Statements::TempOsc(tempOsc, Statements::StatementType::Read);
    auto filter = Filters::TempOsc(connection_.InputId, particle_.Id);
    statement.AddFilter( filter );
    auto cb = Callbacks::TempOsc();
    db.Execute( statement, cb.Callback, cb.CallbackReturn );
    db.Close();

    if (cb.CallbackReturn.TempOscs.size() != 1){
        throw_with_trace( std::logic_error("Could not find unique TempOscillate") );
    }

    tempOsc = cb.CallbackReturn.TempOscs.front();
    return tempOsc;
}

double CohOscTempEqualityReceiver::tempEquality(){
    double tempOsc = pullTempOsc().Temperature;
    
    double tempEq_Te_gtr_Tosc = tempEq_greaterThan_tempOsc(tempOsc);

    double tempEq_Te_less_Tr_less_Tosc = 1.5 * pow( 
            particle_.CohOscComponents.Amplitude / connection_.Model.Constants.mPlanck, 2.)
            * tempReheat_;
    double tempEq_Te_less_Tosc_less_Tr = tempEq_lessThan_tempOsc_lessThan_TR(tempOsc);
    double tempEq_TR_less_Te_less_Tosc = tempEq_TR_lessThan_Te_lessThan_tempOsc(tempOsc);

    // depending on specific conditions, may be multiple Te
    // this happens due to constant energy density pre-oscillation, then scaling as R^-3 vs radiation scaling as R^{-3/2} (during reheat)
    // in the event of multiple Te, want to take the smallest consistent one - and should be <= TR or else we're contradicting reheating -> radiation dom. assumption!
    bool cond1 = (tempEq_Te_less_Tr_less_Tosc < tempOsc && tempOsc > tempReheat_ && tempEq_Te_less_Tr_less_Tosc < tempReheat_ );
    bool cond2 = (tempEq_Te_less_Tosc_less_Tr < tempOsc && tempOsc < tempReheat_);
    bool cond3 = tempEq_Te_gtr_Tosc > tempOsc;
    bool cond4 = (tempEq_TR_less_Te_less_Tosc < tempOsc && tempOsc > tempReheat_ && tempEq_TR_less_Te_less_Tosc > tempReheat_);

    // Effectively, we're taking the first one that's true, these are ordered from lowest->highest temp
    if ( cond1 ){
        return tempEq_Te_less_Tr_less_Tosc;
    } else if ( cond2 ){
        return tempEq_Te_less_Tosc_less_Tr;
    } else if (cond3){
        return tempEq_Te_gtr_Tosc;
    } else if ( cond4 ){
        return tempEq_TR_less_Te_less_Tosc;
    } else{
        connection_.Log.Warn("No cases met for T_Equality! Continuing using the minimal value...");
        return std::min( { tempEq_Te_less_Tr_less_Tosc, tempEq_Te_less_Tosc_less_Tr, tempEq_Te_gtr_Tosc, tempEq_TR_less_Te_less_Tosc } );
    }
}
