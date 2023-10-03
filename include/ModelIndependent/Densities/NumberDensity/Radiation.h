#include <cmath>
#define BOOST_ALLOW_DEPRECATED_HEADERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/math/special_functions.hpp>
#include <ModelIndependent/Densities/NumberDensity/NumberDensityFactory.h>
#include <ModelIndependent/GStar.h>
#include <Models/ModelBase.h>

class NumberDensityRadiation : public NumberDensityFactory
{
private:
    double _temperature;
    ModelBase* model_;
public:
    long double Calculate() override;
    NumberDensityRadiation(ModelBase* model, double temp);
    ~NumberDensityRadiation();
};
