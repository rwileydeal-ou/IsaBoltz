#include <ParticleDefs/Common/DFSZ/SigVDFSZ.h>

SigVDFSZ::SigVDFSZ(){}
SigVDFSZ::~SigVDFSZ(){}

double SigVDFSZ::K2(double x){
    return boost::math::cyl_bessel_k(2, x);
}

double SigVDFSZ::Calculate(double T, double M, double gSU2, double cH, double mu, double vPQ){
    double sigV = 100. * pow(gSU2, 2.) * pow(cH, 2.) * (2./3.) * pow(mu * M, 2.) / (32. * M_PI * pow(vPQ, 2.) * pow(boost::math::zeta(3.), 2.) * pow(T, 4.)) * K2(M/T);
    return sigV;
}
