#include <iostream>
#include <complex>
#include <cmath>
#include <Models/ModelBase.h>
#include <Models/PartialWidth.h>
#include <algorithm>
#include <deque>

class GravitinoWidths
{
private:
    const ModelBase& model_;
    const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles_;
    Models::Particle parent_;
    Models::PartialWidth widthGluino();
    Models::PartialWidth widthAxinoAxion();
    Models::PartialWidth widthAxinoSaxion();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthLightSquarks();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthSbottom();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthStop();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthStau();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthSneutrinos();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthLightLeptons();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthNeutralinoZBoson();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthNeutralinoPhoton();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthCharginoWBoson();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthNeutralinoHiggs();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthCharginoHiggs();
    double gPrime_;
    double g_;
    double gZ_;
    double widthNeutralinoHH(double mNeutralino, double mHH, double alpha, double zMix0, double zMix1);
    double widthNeutralinoHl(double mNeutralino, double mHl, double alpha, double zMix0, double zMix1);
    double widthNeutralinoHA(double mNeutralino, double mHA, double beta, double zMix0, double zMix1);
    double widthNeutralinoZBoson(double mNeutralino, double mZ, double zMix0, double zMix1, double zMix2, double zMix3, double vuq, double vdq);
    double widthNeutralinoPhoton(double mNeutralino, double mPhoton, double zMix0, double zMix1, double zMix2, double zMix3, double vuq, double vdq);
    double gscl(double mFermion, double mScalar, double cLLRR, double cLRRL);
    double gvec(double mFermion, double mVector, double cgg1, double cgg2, double cgh1, double cgh2, double chh1, double chh2);
public:
    GravitinoWidths(const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles);
    ~GravitinoWidths();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > Decays;
};
