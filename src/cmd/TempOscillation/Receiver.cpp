#include <cmd/TempOscillation/Receiver.h>

using namespace std;

TempOscillationReceiver::TempOscillationReceiver(Connection& connection, Models::Particle& particle, double tempReheat) :
    connection_(connection),
    particle_(particle)
{
    tempReheat_ = tempReheat;
    gstar_ = 0.;
    
    tempOscillation_.ParticleId = particle_.Id;
    tempOscillation_.InputId = connection_.InputId;
}
TempOscillationReceiver::~TempOscillationReceiver(){
}

// This method calculates the oscillation temperature of a coh.osc. field for radiation dominated universe (after reheating process)
// from Andre thesis, sec 5.2
// Currently ignores additional instanton contributions if T < lambda_QCD (only relevant for axions)
double TempOscillationReceiver::tempOsc_lessThan_tempReheat(double initialGuess, double& gstar){
    double T1 = 0.;
    double dT = 0.;

    auto a = (initialGuess-T1) / initialGuess;
    while( (initialGuess-T1) / initialGuess > 0.01){
        gstar = GStar::Calculate(connection_, initialGuess);

        T1 = sqrt( 
            // need an extra sqrt(8 pi) since reduced Planck mass code default, while expression uses non-reduced mP
            connection_.Model.Constants.mPlanck * sqrt( 8. * M_PI ) * particle_.Mass 
        ) * pow( 5. / ( 4. * gstar * pow(M_PI, 3.) ), 1./4.);

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

// This method calculates the oscillation temperature of a coh.osc. field for inflaton dominated universe (during reheating process)
// from Andre thesis, sec 5.2
// Currently ignores additional instanton contributions if T < lambda_QCD (only relevant for axions)
double TempOscillationReceiver::tempOsc_greaterThan_tempReheat(double initialGuess, double& gstar){
    double T1 = 0.;
    double dT = 0.;

    double gstr_TReheat = GStar::Calculate(connection_, tempReheat_);

    auto a = (initialGuess-T1) / initialGuess;
    while( (initialGuess-T1) / initialGuess > 0.01){
        // calculate gstar at oscillation temp
        gstar = GStar::Calculate(connection_, initialGuess);
        
        T1 = pow(
            // need an extra sqrt(8 pi) since reduced Planck mass code default, while expression uses non-reduced mP
            pow( tempReheat_, 2. ) * connection_.Model.Constants.mPlanck * sqrt( 8. * M_PI ) * particle_.Mass, 1./4.
        ) * pow( 
            5. * gstr_TReheat / ( 4. * pow( gstar, 2. ) * pow(M_PI, 3.) ), 1./8.
        );

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

void TempOscillationReceiver::Calculate(){
    // see e.g. Andre thesis Eqn 5.20 -> "denominator" is O(1) so guess the "numerator" for fast convergence
    double initialGuess = sqrt( particle_.Mass * connection_.Model.Constants.mPlanck * sqrt( 8. * M_PI ) );
    // we'll do some hacky magic here. 
    // if TReheat > sqrt(mpl * mass), TOsc will always be less than TReheat (extra factor will only make TOsc smaller)
    if (tempReheat_ > initialGuess){
        double gstr = 0.;
        tempOscillation_.Temperature = tempOsc_lessThan_tempReheat(initialGuess, gstr);
        gstar_ = gstr;
    } else{
        // otherwise, we may need to calculate both to ensure we get the right one
        double gstra = 0.;
        double a = tempOsc_lessThan_tempReheat(initialGuess, gstra);
        double gstrb = 0.;
        double b = tempOsc_greaterThan_tempReheat(initialGuess, gstrb);
        // now if the Tosc < TReheat expression has Tosc > TReheat, we know immediately it's invalid, so we take the other solution
        // the converse of the above is also true, and we'll throw in the = TReheat case as well (unlikely to ever pop up, but will choose TOsc < TReheat soln in the case of equality)
        if (a > tempReheat_ && b > tempReheat_){
            tempOscillation_.Temperature = b;
            gstar_ = gstrb;
        } else if ( a <= tempReheat_ && b <= tempReheat_ ){
            tempOscillation_.Temperature = a;
            gstar_ = gstra;
        } else{
            // in the event that a > tempReheat and b < tempReheat, something weird happened and shouldn't go on as neither is valid
            // in the event that a < tempReheat and b > tempReheat, both seem valid and something else weird happened
            // either way, should probably terminate here:
            throw_with_trace( logic_error("No valid expressions for oscillation temperature!") );
        }
    }
    tempOscillation_.GStar = GStar::Calculate(connection_, tempOscillation_.Temperature);
    tempOscillation_.GStarEntropic = GStar::CalculateEntropic(connection_, tempOscillation_.Temperature);
}

Models::TempOscillation TempOscillationReceiver::getTempOscillation(){
    return tempOscillation_;
}
