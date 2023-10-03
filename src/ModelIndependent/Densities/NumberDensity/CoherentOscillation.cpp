#include <ModelIndependent/Densities/NumberDensity/CoherentOscillation.h>

NumberDensityCohOsc::NumberDensityCohOsc(double mass, double amplitude){
    _mass = mass;
    _amplitude = amplitude;
}
NumberDensityCohOsc::~NumberDensityCohOsc(){}

long double NumberDensityCohOsc::Calculate(){
    long double n = 1./2. * _mass * pow( _amplitude, 2. );
    return n;
}
