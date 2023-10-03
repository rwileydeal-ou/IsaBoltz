#include <cmd/SolveBoltzmann/InitialConditions/InitialField/CohOscMatter/Receiver.h>

using namespace std;

CohOscMatterReceiver::CohOscMatterReceiver(Connection& connection, std::shared_ptr< Models::ScaleFactorPoint > initialPoint, const Models::Particle& particle) :
    connection_(connection)
{
    particle_ = particle;
    initialPoint_ = initialPoint;
    boltzmannParticleEvolution_.InputId = connection_.InputId;
    boltzmannParticleEvolution_.ParticleId = particle_.Id;
    boltzmannParticleEvolution_.ParticleKey = particle_.Key;
    boltzmannParticleEvolution_.ProductionMechanism = ParticleProductionMechanism::COHERENT_OSCILLATION;
    boltzmannParticleEvolution_.ScaleFactorId = initialPoint_ -> Id;
}

CohOscMatterReceiver::~CohOscMatterReceiver(){}

double CohOscMatterReceiver::y1Initial(bool isOn, double mass, double entropyInitial, double rho0){
    double y1 = 0.;
    if (mass <= 0.){
        connection_.Log.Error("Unphysical mass detected in Y1! Freezing field " + particle_.Key + "...");
        return 0.;
    }
    if (entropyInitial <= 0.){
        connection_.Log.Error("Unphysical entropy detected in Y1! Freezing field " + particle_.Key + "...");
        return 0.;
    }
    // This utilizes n = rho / m, then using our Defn of Y1
    double arg = (rho0 / mass) / entropyInitial;
    if (arg < 0.){
        connection_.Log.Error("Unknown error while calculating Y1! Freezing field " + particle_.Key + "...");
        return 0.;
    }
    y1 = log(arg);
    return y1;
}

// This comes from e.g. arXiv 1110.2491, Eq. A8 
// This is a corollary of assuming initial conditions for n, then plugging in T_Reheat
double CohOscMatterReceiver::y2Initial(double mass, double tempReheat){
    double y2 = 0.;
    y2 = log(mass / tempReheat);
    return y2;
}

double CohOscMatterReceiver::tempOscillate(double mass, double tempReheat, double hubble){
    if (3. * hubble < mass){
        return tempReheat;
    }
    return 0.;
}

// TODO: we'll have to revise this to make modular, will work for now
double CohOscMatterReceiver::rho0(Models::Particle& particle, const ModelBase& model, std::shared_ptr< Models::ScaleFactorPoint > initialPoint){
    double amplitude = 0.;
    if (particle.Key == "axion"){
        double fTheta = pow( log( exp(1.) / (1. - pow(model.PQSector.Theta_I / M_PI, 2.) ) ), 7./6. );
        amplitude = 1.2 * model.PQSector.Fa / model.PQSector.nDW * model.PQSector.Theta_I * sqrt(fTheta);
    } else{
        amplitude = particle.CohOscComponents.Amplitude;
    }

    // This is the equivalent of testing 3H to m, if 3H > m, yields first expression (osc. during reheating)
    // otherwise, takes second expression (osc. after reheating)
    double rho0 = min( 
        0.05 * pow(M_PI * amplitude / model.Constants.mPlanck, 2.) * initialPoint -> GStarEntropic * pow(initialPoint -> Temperature, 4.), 
        (long double)pow(particle.Mass, 2.) * pow( amplitude, 2. ) / 2. 
    );
    
    return rho0;
}

void CohOscMatterReceiver::Calculate(){
    // start particle in active state
    boltzmannParticleEvolution_.IsActive = true;
    boltzmannParticleEvolution_.IsOscillating = true;
    boltzmannParticleEvolution_.IsDecaying = false;

    // now calculate the initial energy density for coh. osc. field (1/2 * m^2 phi_0^2)
    double rhoInitial = rho0(particle_, connection_.Model, initialPoint_);

    double tOsc = tempOscillate(particle_.Mass, initialPoint_ -> Temperature, initialPoint_ -> Hubble);

    // if the oscillation temperature isn't set (not yet oscillating), we'll flip the active state to "off" 
    // the initial energy density is then frozen - apart from temperature dependence of masses (e.g. axion)
    if (tOsc == 0.){ 
        boltzmannParticleEvolution_.IsActive = false; 
        boltzmannParticleEvolution_.IsOscillating = false;
    }

    boltzmannParticleEvolution_.Y1 = y1Initial(
        boltzmannParticleEvolution_.IsActive, 
        abs( particle_.Mass ), 
        initialPoint_ -> Entropy,
        rhoInitial
    );
    boltzmannParticleEvolution_.Y2 = y2Initial(
        abs( particle_.Mass ), 
        initialPoint_ -> Temperature
    );
}

Models::ParticleEvolution CohOscMatterReceiver::getBoltzmannParticleEvolution(){
    return boltzmannParticleEvolution_;
}

