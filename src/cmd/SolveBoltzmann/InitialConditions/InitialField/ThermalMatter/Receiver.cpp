#include <cmd/SolveBoltzmann/InitialConditions/InitialField/ThermalMatter/Receiver.h>

using namespace std;

// This receiver class calculates the initial conditions (at T_RH) for thermally produced matter
// This requires the cross section to have been previously calculated for the particle at T_RH (using the appropriate command)
ThermalMatterReceiver::ThermalMatterReceiver(
    Connection& connection, 
    DbManager& db,
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint, 
    const Models::Particle& particle
) :
    connection_(connection),
    db_(db)
{
    particle_ = particle;
    initialPoint_ = initialPoint;
    boltzmannParticleEvolution_.InputId = connection_.InputId;
    boltzmannParticleEvolution_.ParticleId = particle_.Id;
    boltzmannParticleEvolution_.ParticleKey = particle_.Key;
    boltzmannParticleEvolution_.ProductionMechanism = ParticleProductionMechanism::THERMAL;
    boltzmannParticleEvolution_.ScaleFactorId = initialPoint_ -> Id;
}

ThermalMatterReceiver::~ThermalMatterReceiver(){}

// TODO: document/check this!
double ThermalMatterReceiver::y1Initial(double tempDecouple, double crossSection, double nEq, double hubble, double entropyInitial){
    double y1 = 0.;
    if (tempDecouple != 0.){
        double expy1 = (crossSection * pow(nEq, 2.) * 0.1) / (hubble * entropyInitial);
        if (expy1 == 0.){ 
            y1 = log( 2. * pow(10., -9.) );
        } else{
            y1 = log( min( expy1, 2. * pow(10., -9.) ) );
        }
    } else{
        y1 = log( nEq / entropyInitial );
    }
    return y1;
}

// This comes from e.g. arXiv 1110.2491, Eq. A8 
// This is a corollary of assuming initial conditions for n, then plugging in T_Reheat
double ThermalMatterReceiver::y2Initial(double mass, double tempReheat, Models::ParticleStatistics stats){
    double y2 = 0.;

    if (tempReheat > (mass * 1.675)){
        double nF = 1.;
        if (stats == Models::ParticleStatistics::Fermion){ nF = 7. / 6.; }
        y2 = log( nF * pow(M_PI, 4.) / (30. * boost::math::zeta(3.)) );
    } else{
        double k1 = boost::math::cyl_bessel_k(1., mass / tempReheat);
        double k2 = boost::math::cyl_bessel_k(2., mass / tempReheat);
        // K1 -> 0 and K2 -> 0 as x -> inf, so if both/either is 0 (since same arg), replace with limit of K1/K2 -> 1
        if ( k1 == 0. || k2 == 0. ){
            y2 = log( mass / ( tempReheat ) + 3.);
        } else{
            y2 = log( mass * k1 / ( k2 * tempReheat ) + 3.);
        }
    }
    return y2;
}

double ThermalMatterReceiver::tempDecouple(double crossSection, double nEq, double tempReheat, double hubble){
    if (crossSection * nEq / hubble < 2.){
        return tempReheat;
    }
    return 0.;
}

SigmaV ThermalMatterReceiver::getCrossSection(){
    SigmaV sigV;
    auto statement = Statements::CrossSection(sigV, Statements::Read);
    auto filter = Filters::CrossSection(connection_.InputId, particle_.Id, initialPoint_->Id);
    statement.AddFilter( filter );
    auto cb = Callbacks::CrossSection();
    db_.Execute( statement, cb.Callback, cb.CallbackReturn );

    if ( cb.CallbackReturn.crossSections.size() <= 0 ) {
        connection_.Log.Warn("Could not find cross section!");
        throw_with_trace( logic_error("SQL error") );
    } else if ( cb.CallbackReturn.crossSections.size() > 1 ){
        connection_.Log.Warn("Multiple cross sections found!" );
        throw_with_trace( logic_error("SQL error") );
    }
    sigV = cb.CallbackReturn.crossSections.front();
    return sigV;
}

void ThermalMatterReceiver::Calculate(){
    SigmaV sigV = getCrossSection();

    // start particle in active state
    boltzmannParticleEvolution_.IsActive = true;
    boltzmannParticleEvolution_.IsDecaying = false;
    
    // quickly calculate the degrees of freedom
    int degF =  ( (int)std::round(2. * particle_.Spin) ) + 1;

    double nEq = NumberDensityEq::Calculate(
        abs(particle_.Mass), 
        initialPoint_ -> Temperature, 
        degF, 
        particle_.Statistics
    );
    
    boltzmannParticleEvolution_.EquilibriumNumberDensity = nEq;

    double tDec = tempDecouple(
        sigV.CrossSection, 
        nEq, 
        initialPoint_ -> Temperature, 
        initialPoint_ -> Hubble
    );

    boltzmannParticleEvolution_.Y1 = y1Initial(
        tDec, 
        sigV.CrossSection, 
        nEq, 
        initialPoint_ -> Hubble, 
        initialPoint_ -> Entropy
    );
    boltzmannParticleEvolution_.Y2 = y2Initial(
        abs( particle_.Mass ), 
        initialPoint_ -> Temperature, 
        particle_.Statistics
    );
}

Models::ParticleEvolution ThermalMatterReceiver::getBoltzmannParticleEvolution(){
    return boltzmannParticleEvolution_;
}

