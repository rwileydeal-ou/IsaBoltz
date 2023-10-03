#include <ModelIndependent/Densities/NumberDensity/Radiation.h>

NumberDensityRadiation::NumberDensityRadiation(ModelBase* model, double temp){
    _temperature = temp;
    model_ = model;
}
NumberDensityRadiation::~NumberDensityRadiation(){}

long double NumberDensityRadiation::Calculate(){
    long double n = 0.;
    double gstarEntropic = GStar::CalculateEntropic(*model_, _temperature);
    n = boost::math::zeta(3.) * gstarEntropic * pow(_temperature, 3.) / pow(M_PI, 2.);
    return n;
}
