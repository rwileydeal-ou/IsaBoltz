#include <cmath>
#include <ModelIndependent/Densities/NumberDensity/NumberDensityFactory.h>
#include <Models/ModelBase.h>

class NumberDensityCohOsc : public NumberDensityFactory
{
private:
    double _mass;
    double _amplitude;
public:
    long double Calculate() override;
    NumberDensityCohOsc(double mass, double amplitude);
    ~NumberDensityCohOsc();
};
