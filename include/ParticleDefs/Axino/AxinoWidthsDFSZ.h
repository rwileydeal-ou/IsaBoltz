#ifndef AxinoWidthsDFSZ_h
#define AxinoWidthsDFSZ_h

#include <complex>
#include <io/util/math/Pauli.h>
#include <io/util/math/Rotations.h>
#include <boost/numeric/ublas/vector.hpp>
#include <ParticleDefs/Axino/AxinoWidths.h>
#include <Models/PartialWidth.h>

class AxinoWidthsDFSZ : public AxinoWidths
{
private:
    // some helper calculations
    double V0(int neutralino, double cH, double vEW);
    double V0i[4];
    double psLamb(double x, double y, double z);
    double widthNeutralinoZBosonDFSZ(double mNeutralino, double mAxino, double mZ, double gU1, double gSU2, double zMix1, double zMix2);
    double widthUpFermionSfermionDFSZ(double mStop, double mTop, double mAxino, double NC, std::complex<double> au, std::complex<double> bu);
    double widthDownFermionSfermionDFSZ(double mSbottom, double mBottom, double mAxino, double NC, std::complex<double> ad, std::complex<double> bd);
    double widthNeutralinoHiggsHLDFSZ(double mNeutralino, double mAxino, double mHL, double alpha, double zMix0, double zMix1, double zMix2, double zMix3, double gU1, double gSU2, double vPQ, double mu, double cH);
    double widthNeutralinoHiggsHHDFSZ(double mNeutralino, double mAxino, double mHH, double alpha, double zMix0, double zMix1, double zMix2, double zMix3, double gU1, double gSU2, double vPQ, double mu, double cH);
    double widthNeutralinoHiggsHADFSZ(double mNeutralino, double mAxino, double mHA, double alpha, double beta, double zMix0, double zMix1, double zMix2, double zMix3, double gU1, double gSU2, double vPQ, double mu, double cH);
    double widthCharginoWBosonDFSZ(double mChargino, double mW, double mAxino, double gSU2, double x0, double y0);
    boost::numeric::ublas::vector<double> TAxinoNeutralinoScalar(double zMix0, double zMix1, double alpha, double mAxino, double mNeutralino);
    std::deque< Models::PartialWidth > widthNeutralinoHiggsDFSZ(double cH);
    std::deque< Models::PartialWidth > widthNeutralinoZBosonDFSZ();
    Models::PartialWidth widthLightCharginoHiggsDFSZ(double cH);
    Models::PartialWidth widthHeavyCharginoHiggsDFSZ(double cH);
    std::deque< Models::PartialWidth > widthCharginoWBosonDFSZ();
    std::deque< Models::PartialWidth > widthFermionSfermionDFSZ();
public:
    AxinoWidthsDFSZ(const ModelBase& model, const std::deque< Models::Particle >& particles);
    ~AxinoWidthsDFSZ();
};

#endif
