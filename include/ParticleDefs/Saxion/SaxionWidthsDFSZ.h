#include <ParticleDefs/Saxion/SaxionWidths.h>
#include <Models/PartialWidth.h>

struct SfermionCouplings{
    double sLsL;
    double sRsR;
    double sLsR;
};

struct SfermionMixedCouplings{
    double LL;
    double LR;
    double RR;
};

class SaxionWidthsDFSZ : public SaxionWidths
{
private:
    double _epsilonhl;
    double _epsilonHH;
    
    double ephl(double cH, double vEW);
    double epHH(double cH, double vEW);
    double psLamb(double x, double y, double z);
    SfermionCouplings AhlUp(double T3L, double YL, double T3R, double YR, double AU, double mUpFermion, double alpha, double beta, double gSU2, double mW, double STW2, double CTW2, double mu);
    SfermionCouplings AHUp(double T3L, double YL, double T3R, double YR, double AU, double mUpFermion, double alpha, double beta, double gSU2, double mW, double STW2, double CTW2, double mu);
    SfermionCouplings AhlDown(double T3L, double YL, double T3R, double YR, double AD, double mDownFermion, double alpha, double beta, double gSU2, double mW, double STW2, double CTW2, double mu);
    SfermionCouplings AHDown(double T3L, double YL, double T3R, double YR, double AD, double mDownFermion, double alpha, double beta, double gSU2, double mW, double STW2, double CTW2, double mu);
    SfermionMixedCouplings couplingMatrixSfermions(double sLsLCoupling, double sRsRCoupling, double sLsRCoupling, double mixAngle);
    double widthSfermionSfermion(double mSfermion1, double mSfermion2, double mSaxion, double AhlSFCoupling, double AHHSFCoupling, double nCF);
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widthSneutrinoSneutrino();
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widthSleptonSlepton();
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widthSquarkSquark();
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widthDiHiggs(double cH);
    double widthDiHiggs(double coupling, double mHiggs1, double mHiggs2, double mSaxion);
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widthVBosonVBoson();
    Models::PartialWidth widthWBosonWBoson(Models::Particle const& wBoson, double ggW, double gsVV);
    Models::PartialWidth widthZBosonZBoson(Models::Particle const& zBoson, double ggZ, double gsVV);
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widthVBosonHiggs();
    Models::PartialWidth widthWBosonHiggs(Models::Particle const& wBoson, Models::Particle const& higgsCharged, double ggW, double gsVh);
    Models::PartialWidth widthZBosonHiggs(Models::Particle const& zBoson, Models::Particle const& higgsPseudo, double ggZ, double gsVh);
    double widthFermionFermion(double mSaxion, double mFermion, double vEW, double gCoupling, double nC);
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widthFermionFermion();
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widthCharginoCharginoSameGen(double cH);
    double widthCharginoCharginoSameGen(double mSaxion, double mChargino, double gSU2, double sigCoupling);
    Models::PartialWidth widthCharginoCharginoDifferentGen(double cH);
    double widthCharginoCharginoDifferentGen(double mSaxion, double mChargino1, double mChargino2, double gSU2, double sigCoupling, double piCoupling);
    double widthNeutralinoNeutralino(double mNeutralino1, double mNeutralino2, double mSaxion, double thz1, double thz2, double zetsIJ, double zetsJI);
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widthNeutralinoNeutralino(double cH);
public:
    SaxionWidthsDFSZ(const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles);
    ~SaxionWidthsDFSZ();
};
