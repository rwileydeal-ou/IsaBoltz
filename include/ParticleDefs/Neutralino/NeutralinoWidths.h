#include <iostream>
#include <cmath>
#include <future>
#include <deque>
#define BOOST_ALLOW_DEPRECATED_HEADERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>
#include <Models/ModelBase.h>
#include <Models/PartialWidth.h>

class NeutralinoWidths
{
private:
    const ModelBase& model_;
    const std::deque< Models::Particle >& particles_;
    typedef std::vector<double> state_type;
    typedef boost::numeric::odeint::runge_kutta_cash_karp54<state_type> error_stepper_type;
    double phaseSpaceIntegral(double mAxino, double mNeutralino, double mFermion, double mZ, double GammaZ, double sgz, double gv, double ga, double Gz, double Gg, double Qe);
    Models::PartialWidth widthNeutralinoAxinoUpQuark(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& quark, const Models::Particle& zBoson);
    Models::PartialWidth widthNeutralinoAxinoDownQuark(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& quark, const Models::Particle& zBoson);
    Models::PartialWidth widthNeutralinoAxinoMassiveDiLepton(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& lepton, const Models::Particle& zBoson);
    Models::PartialWidth widthNeutralinoAxinoMasslessDiLepton(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& lepton, const Models::Particle& zBoson);
    Models::PartialWidth widthNeutralinoAxinoPhoton(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& photon);
    Models::PartialWidth widthNeutralinoAxinoZ(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& zBoson);
    double gam0(double x1, double mAxino, double mNeutralino, double mFermion, double mZ, double GammaZ, double sgz, double gv, double ga, double Gz, double Gg, double Qe);
    double gam1(double x1, double mAxino, double mNeutralino, double mFermion, double Gg, double Qe, double L1, double L2);
    double gam2(double x1, double mAxino, double mNeutralino, double mFermion, double mZ, double sgz, double gv, double ga, double Gz, double GammaZ, double L3, double L4);
    double gam12(double x1, double mAxino, double mNeutralino, double mFermion, double mZ, double sgz, double gv, double Gz, double Gg, double Qe, double GammaZ, double L5, double L6, double L7, double L8);
    double arccot(double arg);
public:
    NeutralinoWidths(const ModelBase& model, const std::deque< Models::Particle >& particles);
    ~NeutralinoWidths();
    std::deque< Models::PartialWidth > Decays;
};
