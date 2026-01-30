#include <cmd/DeltaNeff/BoltzmannReceiver.h>

BoltzmannDeltaNeffReceiver::BoltzmannDeltaNeffReceiver( 
    Connection& connection, 
    const Models::ParticleEvolution& particleEvo, 
    const Models::Particle& particle, 
    const Models::ScaleFactorPoint& finalScaleFactorPoint 
) :
    DeltaNeffReceiver( connection, particleEvo ),
    particle_(particle),
    finalScaleFactorPoint_(finalScaleFactorPoint)
{

}

BoltzmannDeltaNeffReceiver::~BoltzmannDeltaNeffReceiver()
{
}

double BoltzmannDeltaNeffReceiver::calculateDeltaNeff(){
    double dnEff = 0.;
    if ( 
        particleEvo_.IsActive 
        && particleEvo_.EnergyDensity / ( particleEvo_.NumberDensity * particle_.Mass ) > 2. 
        && finalScaleFactorPoint_.Temperature < 1.e-3
    ){
        double rhoNeutrino = ( pow(M_PI, 2.) / 15.) * (7./8.) * pow( 4./11., 4./3. ) * pow( finalScaleFactorPoint_.Temperature, 4. );
        dnEff = particleEvo_.EnergyDensity / rhoNeutrino;
    }

    return dnEff;
}
