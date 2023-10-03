#ifndef SigVDFSZ_h
#define SigVDFSZ_h

#include <cmath>
#include <boost/math/special_functions/zeta.hpp>
#include <boost/math/special_functions/bessel.hpp>
#include <io/Logging/Logger.h>

using namespace std;

class SigVDFSZ
{
private:
    static double K2(double x);
public:
    static double Calculate(double T, double M, double gSU2, double cH, double mu, double vPQ);
    SigVDFSZ();
    ~SigVDFSZ();
};

#endif
