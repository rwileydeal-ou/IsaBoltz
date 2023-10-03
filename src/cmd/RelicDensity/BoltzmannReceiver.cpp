#include <cmd/RelicDensity/BoltzmannReceiver.h>

BoltzmannRelicDensityReceiver::BoltzmannRelicDensityReceiver( Connection& connection, const Models::ParticleEvolution& particleEvo, const Models::Particle& particle, const Models::ScaleFactorPoint& finalScaleFactorPoint ) :
    RelicDensityReceiver(connection, particleEvo, particle, finalScaleFactorPoint)
{
    // TODO: don't hardcode this!!!
    currentTemperature_ = 2.34816e-13;
    currentGStarEntropic_ = GStar::CalculateEntropic(connection_, currentTemperature_);

    initialTemperature_ = finalScaleFactorPoint_.Temperature;
    initialGStarEntropic_ = GStar::CalculateEntropic( connection_, finalScaleFactorPoint_.Temperature );

    rhoCriticalh2_ = 8.0992e-47;
}

BoltzmannRelicDensityReceiver::~BoltzmannRelicDensityReceiver(){

}

double BoltzmannRelicDensityReceiver::cohosc_relicDensity(){
    double oh2 = particle_.Mass * particleEvo_.NumberDensity 
    * ( currentGStarEntropic_ / initialGStarEntropic_ ) 
    * pow(currentTemperature_ / initialTemperature_, 3.) / rhoCriticalh2_;
    return oh2;
}


// TODO: REVISE THIS.
// This is effectively identical to getPN in BoltzmannBuilder, which must also be revised
double BoltzmannRelicDensityReceiver::getRhoNf(double mass, double rhoN, double x0, double xF){
    double rT = 11.5 * mass;
    double x0b = x0;

    if (mass == 0. || rhoN * exp( x0b - xF ) > rT){
        return ( rhoN * exp( x0b - xF ) );
    } else{
        state_type y0(1);
        if ( rhoN > rT ){
            y0[0] = rT / mass;
            x0b += log( rhoN / rT );
        } else{
            y0[0] = rhoN / mass;
        }
        double step = ( xF - x0b ) / 1.e3;
        auto result = boost::numeric::odeint::integrate_adaptive(
            boost::numeric::odeint::make_controlled<error_stepper_type> (1.0e-06, 1.0e-4),
            [ ](const state_type &y, state_type &dydx, double x){
                std::vector<double> A;
                A.reserve(8);
                A.push_back(-0.345998);
                A.push_back(0.234319);
                A.push_back(-0.0953434);
                A.push_back(0.023657);
                A.push_back(-0.00360707);
                A.push_back(0.000329645);
                A.push_back(-0.0000165549);
                A.push_back(.000000351085);

                double rT = 11.5;
                double r = y[0];
                double F2 = -r;

                if (r > rT){
                    dydx[0] = F2;
                } else{
                    double F1 = -2. * (r - 1.);
                    for (int i =0; i<A.size(); ++i){
                        F1 += 3. * A[i] * pow(r - 1., i+2);
                    }
                    dydx[0] = std::max( F1, F2 );
                }
            },
            y0, x0, xF, step
        );
        return ( mass * y0[0] ); 
    }
}

double BoltzmannRelicDensityReceiver::thermal_relicDensity(){
    // initial N-efolds here is where Boltz code finished
    double x0 = log( finalScaleFactorPoint_.ScaleFactor );
    // final N-efolds is initial + Delta N, where Delta N is derived assuming conservation of entropy between x0 and xF
    double xF = x0 
    + log( 
        pow( initialGStarEntropic_ / currentGStarEntropic_, 1./3. )
        * ( initialTemperature_ / currentTemperature_ )
    );

    double rhoN = getRhoNf( 
        particle_.Mass, particleEvo_.EnergyDensity / particleEvo_.NumberDensity, x0, xF
    );

    double oh2 = particleEvo_.NumberDensity * rhoN 
    * ( currentGStarEntropic_ / initialGStarEntropic_ ) 
    * pow(currentTemperature_ / initialTemperature_, 3.) / rhoCriticalh2_;
    return oh2;
}

double BoltzmannRelicDensityReceiver::radiation_relicDensity(){
    double oh2 = pow( M_PI, 2. ) / 30. 
    * currentGStarEntropic_ 
    * pow( currentTemperature_, 4. ) / rhoCriticalh2_;

    return oh2;
}

double BoltzmannRelicDensityReceiver::calculateRelicDensity(){
    // an "or" clause *may* be more appropriate here, however keep as "and" for now so we don't obscure any potential weird bugs
    if (particleEvo_.EnergyDensity == 0. && particleEvo_.NumberDensity == 0.){
        return 0.;
    }
    switch (particleEvo_.ProductionMechanism){
        case ParticleProductionMechanism::COHERENT_OSCILLATION:
            return cohosc_relicDensity();
        case ParticleProductionMechanism::RADIATION:
            return radiation_relicDensity();
        case ParticleProductionMechanism::THERMAL:
            return thermal_relicDensity();        
        default:
            throw NotImplementedException();
    }
    return 0.;
}
