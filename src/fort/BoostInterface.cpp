#include <boost/math/special_functions.hpp>

using namespace std;
extern"C" double boostbesselk1_(double *arg){
    double bessel = boost::math::cyl_bessel_k(1, *arg);
    return bessel;
}
extern"C" double boostbesselk2_(double *arg){
    double bessel = boost::math::cyl_bessel_k(2, *arg);
    return bessel;
}
