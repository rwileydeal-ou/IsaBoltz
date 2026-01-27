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
    std::vector<Models::PartialWidth> widthSneutrinoSneutrino();
    std::vector<Models::PartialWidth> widthSleptonSlepton();
    std::vector<Models::PartialWidth> widthSquarkSquark();
    std::vector<Models::PartialWidth> widthDiHiggs(double cH);
    double widthDiHiggs(double coupling, double mHiggs1, double mHiggs2, double mSaxion);
    std::vector<Models::PartialWidth> widthVBosonVBoson();
    Models::PartialWidth widthWBosonWBoson(Models::Particle const& wBoson, double ggW, double gsVV);
    Models::PartialWidth widthZBosonZBoson(Models::Particle const& zBoson, double ggZ, double gsVV);
    std::vector<Models::PartialWidth> widthVBosonHiggs();
    Models::PartialWidth widthWBosonHiggs(Models::Particle const& wBoson, Models::Particle const& higgsCharged, double ggW, double gsVh);
    Models::PartialWidth widthZBosonHiggs(Models::Particle const& zBoson, Models::Particle const& higgsPseudo, double ggZ, double gsVh);
    double widthFermionFermion(double mSaxion, double mFermion, double vEW, double gCoupling, double nC);
    std::vector<Models::PartialWidth> widthFermionFermion();
    std::vector<Models::PartialWidth> widthCharginoCharginoSameGen(double cH);
    double widthCharginoCharginoSameGen(double mSaxion, double mChargino, double gSU2, double sigCoupling);
    Models::PartialWidth widthCharginoCharginoDifferentGen(double cH);
    double widthCharginoCharginoDifferentGen(double mSaxion, double mChargino1, double mChargino2, double gSU2, double sigCoupling, double piCoupling);
    double widthNeutralinoNeutralino(double mNeutralino1, double mNeutralino2, double mSaxion, double thz1, double thz2, double zetsIJ, double zetsJI);
    std::vector<Models::PartialWidth> widthNeutralinoNeutralino(double cH);
public:
    SaxionWidthsDFSZ(
        const ModelBase& model, 
        const std::vector< Models::Particle >& particles
    );
    ~SaxionWidthsDFSZ();
};
