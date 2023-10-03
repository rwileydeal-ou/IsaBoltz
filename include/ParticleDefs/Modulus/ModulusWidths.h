#include <iostream>
#include <cmath>
#include <Models/ModelBase.h>
#include <Models/PartialWidth.h>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>

class ModulusWidths
{
private:
    const ModelBase& model_;
    Models::Particle parent_;
    const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles_;
    Models::PartialWidth widthDiNeutralino(double lambdaEff, const Models::Particle& neutralino, double mPlanck);
    Models::PartialWidth widthNeutralinoINeutralinoJ(double lambdaEff, const Models::Particle& neutralinoI, const Models::Particle& neutralinoJ, double mPlanck);
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthNeutralinos();

    Models::PartialWidth widthDiChargino(double lambdaSU2, double gV, double gA, const Models::Particle& chargino, double mPlanck);
    Models::PartialWidth widthChargino1Chargino2(double lambdaSU2, double gV, double gA, const Models::Particle& chargino1, const Models::Particle& chargino2, double mPlanck);
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthCharginos();

    Models::PartialWidth widthGluinoGluino(double lambdaSU2, const Models::Particle& gluino, double mPlanck);
    Models::PartialWidth widthGluinoGluino();

    Models::PartialWidth widthWBosonWBoson(double lambdaSU2, const Models::Particle& wBoson, double mPlanck);
    Models::PartialWidth widthZBosonZBoson(double lambdaU1, double lambdaSU2, double sin2tw, const Models::Particle& zBoson, double mPlanck);
    Models::PartialWidth widthPhotonPhoton(double lambdaU1, double lambdaSU2, double sin2tw, const Models::Particle& photon, double mPlanck);
    Models::PartialWidth widthZBosonPhoton(double lambdaU1, double lambdaSU2, double sin2tw, const Models::Particle& zBoson, const Models::Particle& photon, double mPlanck);
    Models::PartialWidth widthGluonGluon(double lambdaSU3, const Models::Particle& gluon, double mPlanck);
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthVBosonVBoson();

    Models::PartialWidth widthLightHiggsLightHiggs(double lambdaEff, const Models::Particle& lightHiggs, double mPlanck);
    Models::PartialWidth widthHeavyHiggsHeavyHiggs(double lambdaEff, const Models::Particle& heavyHiggs, double mPlanck);
    Models::PartialWidth widthLightHiggsHeavyHiggs(double lambdaEff, const Models::Particle& lightHiggs, const Models::Particle& heavyHiggs, double mPlanck);
    Models::PartialWidth widthPseudoHiggsPseudoHiggs(double lambdaEff, const Models::Particle& pseudoHiggs, double mPlanck);
    Models::PartialWidth widthChargedHiggsChargedHiggs(double lambdaEff, const Models::Particle& chargedHiggs, double mPlanck);
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthHiggsHiggs();

    Models::PartialWidth widthSquarkLSquarkL(double lambdaQ, const Models::Particle& squarkL, double mPlanck);
    Models::PartialWidth widthSquarkRSquarkR(double lambdaUD, const Models::Particle& squarkR, double mPlanck);
    Models::PartialWidth widthSquark1Squark1(double lambdaUD, double lambdaQ3, double thetaMix, const Models::Particle& squark1, double mPlanck);
    Models::PartialWidth widthSquark2Squark2(double lambdaUD, double lambdaQ3, double thetaMix, const Models::Particle& squark2, double mPlanck);
    Models::PartialWidth widthSquark1Squark2(double lambdaUD, double lambdaQ3, double thetaMix, const Models::Particle& squark1, const Models::Particle& squark2, double mPlanck);
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthSquarkSquark();

    Models::PartialWidth widthSleptonLRSleptonLR(double lambdaLE, const Models::Particle& sleptonLR, double mPlanck);
    Models::PartialWidth widthSlepton1Slepton1(double lambdaL, double lambdaE3, double thetaMix, const Models::Particle& slepton1, double mPlanck);
    Models::PartialWidth widthSlepton2Slepton2(double lambdaL, double lambdaE3, double thetaMix, const Models::Particle& slepton2, double mPlanck);
    Models::PartialWidth widthSlepton1Slepton2(double lambdaL, double lambdaE3, double thetaMix, const Models::Particle& slepton1, const Models::Particle& slepton2, double mPlanck);
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthSleptonSlepton();

    Models::PartialWidth widthQuarkQuark(double lambdaUD, double lambdaQ, const Models::Particle& quark, double mPlanck);
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthQuarkQuark();

    Models::PartialWidth widthChargedDiLepton(double lambdaL, double lambdaE, const Models::Particle& lepton, double mPlanck);
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthLeptonLepton();

    Models::PartialWidth widthDiGravitino(double lambdaG, const Models::Particle& gravitino, double mPlanck);
    Models::PartialWidth widthDiGravitino();

    Models::PartialWidth widthDiAxion(double lambdaAx, const Models::Particle& axion, double mPlanck);
    Models::PartialWidth widthDiSaxion(double lambdaAx, const Models::Particle& saxion, double mPlanck);
    Models::PartialWidth widthDiAxino(double lambdaAx, const Models::Particle& axino, double mPlanck);
    Models::PartialWidth widthDiAlp(double lambdaAlp, const Models::Particle& alp, double mPlanck);
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > widthDiAxionic();
public:
    ModulusWidths(const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles);
    ~ModulusWidths();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > Decays;
}; 
