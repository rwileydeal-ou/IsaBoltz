#ifndef NumberDensityEq_h
#define NumberDensityEq_h

#include <cmath>
#include <boost/math/special_functions/zeta.hpp>
#include <boost/math/special_functions/bessel.hpp>
#include <io/Logging/Logger.h>
#include <Models/Particle.h>

class NumberDensityEq
{
private:
    /* data */
public:
    static double Calculate(double mass, double T, double degF, Models::ParticleStatistics stats);
    NumberDensityEq(/* args */);
    ~NumberDensityEq();
};

#endif
