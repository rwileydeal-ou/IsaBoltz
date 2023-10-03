#ifndef CoherentOsc_h
#define CoherentOsc_h

class CoherentOsc
{

public:
    bool IsCoherentOscComponent;
    double Amplitude = 0.;
    long double TempOsc = 0.;

    CoherentOsc(bool isOscComp){
        IsCoherentOscComponent = isOscComp;
    }
    CoherentOsc(bool isOscComp, double amplitude){
        IsCoherentOscComponent = isOscComp;
        Amplitude = amplitude;
    }
    CoherentOsc(){}
};


#endif
