#ifndef ModelBase_h
#define ModelBase_h

#include <deque>
#include <algorithm>
#include <boost/pool/pool_alloc.hpp>
#include <Models/Particle.h>
#include <Models/Sugra/NonUniversalSugra.h>

// TODO: Move this to better place/redo how calculated (?)
struct TrhPars{
    double trhm1v[100000], trhm2v[100000], trhm3v[100000], trhg1v[100000], trhg2v[100000], trhg3v[100000], tqv[100000];
};

struct StandardModelCouplings{
    double gSU2;
    double gU1;
    double alphaS;
    double alphaEM;
};

struct StandardModelMixingAngles{
    double SinSqrThetaW;
};

struct StandardModelParams{
    StandardModelCouplings Couplings;
    StandardModelMixingAngles MixingAngles;
    double vEW;
    int NC;
    double LambdaQCD;
};

struct SusyMixingAngles{
    double zMix[4][4];      // neutralino mixing matrix
    double gammaL;          // left chargino
    double gammaR;          // right chargino
    double alfah;           // Higgs mixing 
    double thetaT;          // stop
    double thetaB;          // sbottom
    double thetaL;          // stau
};

struct MSusyQuarkMasses{
    double topMSusy;        // top mass at MSUSY
    double botMSusy;        // bottom mass at MSUSY
    double tauMSusy;        // tau mass at MSUSY
};

struct ParticleMasses{
    double M0;
    double MHF;
    MSusyQuarkMasses MSusy;
};

struct SusyCouplings{
    double g_1;
    double g_2;
    double g_3;
    double y_tau;
    double y_b;
    double y_t;
    double M_1;
    double M_2;
    double M_3;
    double A_tau;
    double A_b;
    double A_t;
    double M_hdSqr;
    double M_huSqr;
    double M_erSqr;
    double M_elSqr;
    double M_dnrSqr;
    double M_uprSqr;
    double M_uplSqr;
    double M_taurSqr;
    double M_taulSqr;
    double M_btrSqr;
    double M_tprSqr;
    double M_tplSqr;
    double mu;
    double B;
    double Y_N;
    double M_nr;
    double A_n;
    double vdq;
    double vuq;
    double g_GUT;
    double m_GUT;
    double alpha_GUT;
};

struct SusyParams{
    SusyMixingAngles MixingAngles;
    SusyCouplings Couplings;
    double TanBeta;
};

struct PecceiQuinnPotential{
    double Af;
    double mX;
    double mY;
    double vX;
    double vY;
    double vPQ;
    double f;
    double g;
};

enum EAxionModel{
    DFSZ,
    KSVZ
};

struct PecceiQuinnParams{
    double Theta_I;
    double Xi;
    EAxionModel AxionModel;
    int nDW;
    double Fa;
    PecceiQuinnPotential Potential;
};

struct CosmologicalTemperatures{
    double NeutrinoDecouple;
    double Reheat;
    double Hadronization;
    double Final;
};

struct StandardCosmologicalParams{
    CosmologicalTemperatures Temperatures;
};

struct PhysicalConstants{
    double hBar;
    double mPlanck;
};

struct ModelOptions{
    int ModelInt;
    NonUniversalSugraOptions NonUniversalSugra;
};

struct ModulusGaugeCouplings{
    double LambdaU1;        // coupling to U(1) superfield
    double LambdaSU2;       // coupling to SU(2) superfield
    double LambdaSU3;       // coupling to SU(3) superfield
};

struct ModulusAxionicCouplings{
    double LambdaALP;
    double LambdaAx;
};

struct ModulusGravitinoCouplings{
    double LambdaG;     // coupling to Gravitino
};

struct ModulusHiggsCouplings{
    double LambdaGM;     // coupling to Higgs superfields (neutral and charged)
    double LambdaHu;
    double LambdaHd;
};

struct ModulusQuarkSquarkCouplings{
    double LambdaUu;      // coupling to 1st generation up-type left-hand quark/squark components
    double LambdaUc;      // coupling to 2nd generation up-type left-hand quark/squark components
    double LambdaUt;      // coupling to 3rd generation up-type left-hand quark/squark components

    double LambdaDd;      // coupling to 1st generation down-type left-hand quark/squark components
    double LambdaDs;      // coupling to 2nd generation down-type left-hand quark/squark components
    double LambdaDb;      // coupling to 3rd generation down-type left-hand quark/squark components

    double LambdaQ1;      // coupling to 1st generation right-hand quark/squark components
    double LambdaQ2;      // coupling to 2nd generation right-hand quark/squark components
    double LambdaQ3;      // coupling to 3rd generation right-hand quark/squark components
};

struct ModulusLeptonSleptonCouplings{
    double LambdaLe;      // coupling to 1st generation left-hand lepton/slepton components
    double LambdaLm;      // coupling to 2nd generation left-hand lepton/slepton components
    double LambdaLt;      // coupling to 3rd generation left-hand lepton/slepton components

    double LambdaE1;      // coupling to 1st generation right-hand lepton/slepton components
    double LambdaE2;      // coupling to 2nd generation right-hand lepton/slepton components
    double LambdaE3;      // coupling to 3rd generation right-hand lepton/slepton components
};

struct ModulusParams{
    ModulusGaugeCouplings GaugeCouplings;
    ModulusHiggsCouplings HiggsCouplings;
    ModulusQuarkSquarkCouplings QuarkSquarkCouplings;
    ModulusLeptonSleptonCouplings LeptonSleptonCouplings;
    ModulusGravitinoCouplings GravitinoCouplings;
    ModulusAxionicCouplings AxionicCouplings;
};

struct ModelBase{
    ParticleMasses Masses;
    ModelOptions Options;
    PhysicalConstants Constants;
    StandardModelParams StandardModel;
    SusyParams Susy;
    StandardCosmologicalParams Cosmology;
    PecceiQuinnParams PQSector;
    TrhPars RunningParameters;
    ModulusParams ModulusCouplings;
    double A0;
    double SgnMu;
    double DeltaEW;
    int NW;
    int IsajetError;
    std::deque< Models::Particle, boost::pool_allocator< Models::Particle> > Particles;
};

class ModelBaseOps{
public:
    static Models::Particle& Find (std::deque< Models::Particle, boost::pool_allocator<Models::Particle> > &vec, std::string key) {
        auto result = std::find_if( vec.begin(), vec.end(),
            [&] (Models::Particle const& p) { return p.Key == key; }
        );
        return (*result);
    }
    static const Models::Particle FindConst (std::deque< Models::Particle, boost::pool_allocator<Models::Particle> > vec, std::string key) {
        auto result = std::find_if( vec.begin(), vec.end(),
            [&] (Models::Particle const& p) { return p.Key == key; }
        );
        return (*result);
    }
};

#endif
