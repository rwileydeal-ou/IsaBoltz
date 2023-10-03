#ifndef StepInfo_h
#define StepInfo_h

struct StepInfo
{
    double T0 = 0.;
    double TF = 0.;
    double OriginalDeltaT = 0.;
    double CurrentDeltaT = 0.;
    double AbsError = 0.;
    double RelError = 0.;

    StepInfo(){}
    StepInfo( double t0, double tF, double dt, double absError, double relError ){
        T0 = t0;
        TF = tF;
        OriginalDeltaT = dt;
        CurrentDeltaT = dt;
        AbsError = absError;
        RelError = relError;
    }
};


#endif
