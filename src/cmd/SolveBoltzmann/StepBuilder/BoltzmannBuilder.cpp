#include <cmd/SolveBoltzmann/StepBuilder/BoltzmannBuilder.h>

using namespace std;

BoltzmannBuilder::BoltzmannBuilder(
    Connection& connection, 
    const BoltzmannData& data
) : 
    connection_(connection),
    data_(data),
    db_(connection)
{
}
BoltzmannBuilder::~BoltzmannBuilder(){
}


template <typename T>
T BoltzmannBuilder::handleOverflow(T input, Logger& logger){
    if (std::isinf(input) || std::isnan(input)){
        logger.Warn("Overflow detected when building Boltzmann equations");
    }

    return input;
}

// TODO: REVISE THIS CRAP!!!
// From OG comments:
//...Compute pressure/number density given energy/number density ratio and particle's mass
//...Uses numerical fit (see notes)
double BoltzmannBuilder::getPN(double r, double m){
    static const double A[8] = {
        -0.345998, 0.234319, -0.0953434, 0.023657,
        -0.00360707, 0.000329645, -0.0000165549, 0.000000351085
    };
                            
    double rT = 11.5 * m;
    if (r < rT) {
        const double rm1 = (r / m) - 1.0;
        double F1 = (2.0 / 3.0) * rm1;
        double rm1pow = rm1 * rm1;
        for (int i = 0; i < 8; ++i) {
            F1 += A[i] * rm1pow;
            rm1pow *= rm1;
        }
        const double F2 = r / 3.0;
        const double val = m * F1;
        return val < F2 ? val : F2;
    }
    return r / 3.0;
}

// This method builds the contribution from a particle to the entropy Boltzmann equation, d(log(S/S0))/dx
// See e.g. arXiv 1406.4138 Eq. 2.8 (recast from dS/dt -> d(log(S/S0))/dx)
ComponentBuilder BoltzmannBuilder::calculate_particle_entropy(const double& t, const ParticleData& particle, const ParticleData& rad){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );
    
    long double widthXmass = Br(particle, rad) * particle.TotalWidth * particle.Mass;
    long double expTerm = expl( particle.Y1 + 3. * t - rad.Y1 );
    if ( particle.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION ){
        builder.EntropyEquation += widthXmass * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
        builder.EntropyJacobian[ 0 ] += - widthXmass * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
        builder.EntropyJacobian[ 2 * particle.EqnIndex ] += widthXmass * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
        builder.EntropyJacobian[ 2 * particle.EqnIndex + 1 ] += - widthXmass * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );

        // dh/drho etc
        builder.EntropyJacobian[ 2 * particle.EqnIndex ] += - widthXmass * particle.EnergyDensity * expTerm / ( 6. * pow(data_.CurrentPoint.Hubble, 3.) * 5.95e36 * data_.CurrentPoint.Temperature );
        builder.EntropyJacobian[ 2 * particle.EqnIndex + 1 ] += - widthXmass * particle.EnergyDensity * expTerm / ( 6. * pow(data_.CurrentPoint.Hubble, 3.) * 5.95e36 * data_.CurrentPoint.Temperature );
    } else{ 
        long double n1 = particle.NumberDensity;
        long double nEQ1 = particle.EquilibriumNumberDensity;
        auto nRatio = nEQ1/n1;
	if (std::isnan(nRatio) || std::isinf(nRatio)){
            connection_.Log.Warn("Overflow detected in entropy due to equilibrium density");
            nRatio = 0.;
        }

//        builder.EntropyEquation += widthXmass * ( n1 - nEQ1 ) * expl( 3. * t - rad.Y1 ) / ( data_.ReheatPoint.Entropy * data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );

        builder.EntropyEquation += widthXmass * ( 1. - nRatio ) * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
        builder.EntropyJacobian[ 0 ] += - widthXmass * ( 1. - nRatio ) * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
        builder.EntropyJacobian[ 2 * particle.EqnIndex ] += widthXmass * ( 1. - nRatio ) * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
        builder.EntropyJacobian[ 2 * particle.EqnIndex + 1 ] += - widthXmass * ( 1. - nRatio ) * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
    }
    return builder;
}

// This method computes the "hubble dilution" term in the number density equation
ComponentBuilder BoltzmannBuilder::calculate_hubble_contribution(const ParticleData &particle){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );
    // This is the Hubble expansion term (dividing by n_i H)
    builder.NumberDensityEquation = -3.; 
    return builder;
}

// This method computes the "pressure" term in the energy density equation
ComponentBuilder BoltzmannBuilder::calculate_pressure_term(const ParticleData& parent){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );
    long double rhoN1 = parent.EnergyDensity / parent.NumberDensity;

    // This is first term on RHS in e.g. Eq 2.6 from arXiv 1406.4138
    // Both sides were multiplied by n_i / (H * rho_i) = 1 / (H * Y2) with our defn of Y2
    // This makes diff eq. defined WRT log(rho_i / n_i))
    long double P1num = 3. * getPN(rhoN1, parent.Mass) / rhoN1;
    builder.EnergyDensityEquation += -P1num;
//    builder.EnergyDensityJacobian[  2 * parent.EqnIndex ] += P1num; // this is correct assuming P doesn't depend on rho/n
//    builder.EnergyDensityJacobian[  2 * parent.EqnIndex + 1 ] += P1num;
    return builder;
}

// This method builds the contribution from the annihilation terms from the specified particle
ComponentBuilder BoltzmannBuilder::calculate_annihilation_term( const ParticleData& parent, const ParticleData& rad ){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );
    long double n1 = parent.NumberDensity;
    long double nEQ1 = parent.EquilibriumNumberDensity;
    long double nRatio = n1 / (nEQ1 + nRegularizer_);

    long double eps_damp = 0L;//1e-7L;
    long double p = 8.0L;              // smoothness exponent
    long double sigma_damp = 0.15L;    // fractional width around equilibrium

    long double x = fabsl(nRatio - 1.0L) / sigma_damp;

    long double damp = eps_damp + (1.0L - eps_damp) * (1.0L - expl(-powl(x, p)));


//    long double sigma_damp = 20.L;  // tuning parameter for damping term
//    long double damp = 1.0L - expl(-pow( fabsl(logl(nRatio)) / sigma_damp, 2.0L));


    // We put this approximation in since this term is an attractor
    // Very noisy and lengthy if approximation isn't in, but final results are identical
    if ( parent.ParticleKey == "axion" || parent.ParticleKey == "saxion" || parent.ParticleKey == "axino" ){
        // for the PQ sector, have <sig.v>_{ij} dominated by i != j, and j can be treated as radiation 
        // we also divide by g_{*S} here, since the radiation number density involves all g_{*S} DOF, but the annihilation cross section only involves ~1 DOF
        long double annihilationTerm = parent.AnnihilationCrossSection * rad.NumberDensity * ( ( nEQ1 / ( n1 + nRegularizer_ ) ) - 1. ) / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.GStarEntropic );
        long double jacobianTerm = parent.AnnihilationCrossSection * rad.NumberDensity * ( ( nEQ1 / ( n1 + nRegularizer_ ) ) - 1. ) / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.GStarEntropic );
        builder.NumberDensityEquation += annihilationTerm * damp;
        builder.NumberDensityJacobian[ 2 * rad.EqnIndex ] += jacobianTerm * damp;
        builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += - jacobianTerm * damp;
    } else{
        // Regularized eqTerm
        long double eqTerm = (nEQ1 * nEQ1) / (n1 + nRegularizer_);

        // Main annihilation term
        long double annihilationTerm = parent.AnnihilationCrossSection * (eqTerm - n1) / (data_.CurrentPoint.Hubble);

        // Smooth transition between regimes
        long double equilCutoff = 0.5L;
        long double nonEquilCutoff = 0.1L;
        long double k_smooth = 20.0L;  // controls how sharp the transition is

        long double w_equil = 1./(1. + expl(-k_smooth) * ( nRatio - equilCutoff ));
        long double w_nonEquil = 1./(1. + expl(-k_smooth) * ( nRatio - nonEquilCutoff ));
        long double w = w_nonEquil * (1.0L - w_equil);  // middle transition window

        // Equilibrium and non-equilibrium limits
        long double equilLimit    = parent.AnnihilationCrossSection * ( eqTerm - n1 ) / ( data_.CurrentPoint.Hubble );
        long double nonEquilLimit = parent.AnnihilationCrossSection * ( -eqTerm - n1 ) / ( data_.CurrentPoint.Hubble );

        // Smoothly blend the Jacobian contribution
        long double jacobianTerm =
            (1.0L - w_nonEquil) * nonEquilLimit
            + w * (0.5L * (equilLimit + nonEquilLimit))
            + w_equil * equilLimit;

        // Build final contributions
        builder.NumberDensityEquation += annihilationTerm * damp;
        builder.NumberDensityJacobian[2 * parent.EqnIndex] += jacobianTerm * damp;
        
    }

    return builder;
}

// This method calculates the decay terms from the specified parent 
ComponentBuilder BoltzmannBuilder::calculate_decay_terms(const ParticleData& parent){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );
    if (parent.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION){
        builder = calculate_cohOsc_field_decay(parent, data_.CurrentPoint.Hubble);
    } else if (parent.ProductionMechanism == ParticleProductionMechanism::THERMAL){
        builder = calculate_thermal_particle_decays(parent, data_.CurrentPoint.Hubble);
    }
    return builder;
}

// This method is a helper function to calculate the decays of thermally produced particles
// called in the calculate_decay_terms method for the appropriate parent types
ComponentBuilder BoltzmannBuilder::calculate_thermal_particle_decays(const ParticleData& parent, const long double& hubble ){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );
    double relativisticFactor = parent.Mass * parent.NumberDensity / ( parent.EnergyDensity + nRegularizer_ );
    if ( !std::isfinite(relativisticFactor) || relativisticFactor > 1. ){
        relativisticFactor = 1.;
    }
    // Decay contribution
    // This is just the Gamma*mass*n/rho*H term
    builder.NumberDensityEquation += - parent.TotalWidth * relativisticFactor / hubble;
    
    double sigma = 200.;
    double dampFactor = expl( - sigma * pow( relativisticFactor, 2. ) );
//    builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += parent.TotalWidth * relativisticFactor * dampFactor / hubble;
//    builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += parent.TotalWidth * relativisticFactor * dampFactor / hubble;

//    builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += parent.TotalWidth * parent.Mass * parent.NumberDensity /  (6. * pow(hubble, 3.) * 5.95e36 ) ; 
//    builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += parent.TotalWidth * parent.Mass * parent.NumberDensity /  (6. * pow(hubble, 3.) * 5.95e36 ) * dampFactor; 
     
    // These are inverse decay contributions
    for ( auto& daughter : data_.ParticleDatas ){
        if ( !daughter.IsActive || daughter.ParticleEvolutionId == parent.ParticleEvolutionId || daughter.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION){
            continue;
        }
        if ( ( daughter.ProductionMechanism == ParticleProductionMechanism::THERMAL && daughter.EquilibriumNumberDensity > 0. ) || daughter.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
            ComponentBuilder inverseDecay( data_.ParticleDatas.size() );
            inverseDecay = calculate_inverse_decay(parent, daughter, hubble);
            builder.NumberDensityEquation += inverseDecay.NumberDensityEquation;
            builder.NumberDensityJacobian[ 2 * daughter.EqnIndex ] += inverseDecay.NumberDensityJacobian[ 2 * daughter.EqnIndex ];
            builder.NumberDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += inverseDecay.NumberDensityJacobian[ 2 * daughter.EqnIndex + 1 ];
        }
    }
    return builder;
}

ComponentBuilder BoltzmannBuilder::calculate_inverse_decay(const ParticleData& parent, const ParticleData& daughter, const long double& hubble){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );
    long double nRatioDaughters = 0.;
    
    if ( daughter.ProductionMechanism == ParticleProductionMechanism::THERMAL ){
        if ( (int)parent.Charges[0].Value == -1 && (int)daughter.Charges[0].Value == -1 ){
            // if parent is R-odd and child is R-odd, assume other child is effectively radiation in thermal equilibrium (R-even)
            nRatioDaughters = daughter.NumberDensity / daughter.EquilibriumNumberDensity;
        }
/*        else if ( (int)parent.Charges[0].Value == -1 && (int)daughter.Charges[0].Value == 1 ){
            // if parent is R-odd and child is R-even
            nRatioDaughters = 1.;
        }*/
        else if ( (int)parent.Charges[0].Value == 1 && (int)daughter.Charges[0].Value == 1 ){
            // if parent is R-even and child is R-even, take both daughters to be the same (don't assume thermal eq)
            nRatioDaughters = pow( daughter.NumberDensity / daughter.EquilibriumNumberDensity, 2. );
        }
        else if ( (int)parent.Charges[0].Value == 1 && (int)daughter.Charges[0].Value == -1 ){
            // if parent is R-even and child is R-odd, take both daughters to be the same (not thermal eq)
            nRatioDaughters = pow( daughter.NumberDensity / daughter.EquilibriumNumberDensity, 2. );
        }
        
        // can get overflows due to absurdly low NEQ - set to 0 manually since machine can't interpret inf*0
        if ( std::isinf(nRatioDaughters) || std::isnan(nRatioDaughters) ){
            nRatioDaughters = 0.;
        }
    } else if ( daughter.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
        // if the daughter is radiation, assume a = b = radiation, so n_a / neq_a = n_b / neq_b = 1
        nRatioDaughters = 1.;
    }


    // This term is the prefactor to the decay piece, Gamma_i * m_i / (rho_i * H), which then multiplies the rest of the decay piece
    // (see e.g. sum in the second term on RHS of Eq. 2.4 in arXiv 1406.4138, note EQ was divided by n_i * H for computation)
    long double preFactor = parent.TotalWidth * parent.Mass / ( hubble * parent.EnergyDensity );

    // We'll use this variable for the term in parentheses within e.g. the second term of RHS of Eq. 2.4 of arXiv 1406.4138
    long double nEff = nRatioDaughters * parent.EquilibriumNumberDensity;
    long double postFactor = Br(parent, daughter) * handleOverflow( nEff, connection_.Log );
    // if decays to R-odd pair, end up with 2 neutralinos in final state so need to multiply n and rho by 2
    if ( (int)parent.Charges[0].Value == 1 && (int)daughter.Charges[0].Value == -1 ){
        postFactor = 2. * postFactor;
    }

    // Now we'll assemble the pre-sum piece with the sum piece
    builder.NumberDensityEquation += preFactor * postFactor;
    builder.NumberDensityJacobian[ 2 * daughter.EqnIndex ] += preFactor * postFactor;
    builder.NumberDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += -2. * preFactor * postFactor;

    return builder;
}


// This method is a helper function to calculate the decays of coherently oscillating fields
// called in the calculate_decay_terms method for the appropriate parent types
ComponentBuilder BoltzmannBuilder::calculate_cohOsc_field_decay(const ParticleData& parent, const long double& hubble ){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );
    // For coh. osc. fields, have normal decay piece, but no inverse decays
    // This is then just the Gamma*mass*n / rho*H term, but rho=n*m
    builder.NumberDensityEquation += - parent.TotalWidth / hubble;
//    builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += parent.TotalWidth * parent.EnergyDensity /  (6. * pow(hubble, 3.) * 5.95e36 ) ; 
//    builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += parent.TotalWidth * parent.EnergyDensity /  (6. * pow(hubble, 3.) * 5.95e36 ) ; 

    return builder;
}

// This method computes the "injection" terms in number & energy density eqns 
// i.e. for species "i", decay of "a" produces "i" - applies only to thermal/radiation-type species "i"
ComponentBuilder BoltzmannBuilder::calculate_injection_terms(const ParticleData& parent){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );

    // injection contribution
    for (auto& daughter : data_.ParticleDatas){
        if ( daughter.ParticleEvolutionId == parent.ParticleEvolutionId 
            || !daughter.IsActive 
            || daughter.ProductionMechanism == ParticleProductionMechanism::RADIATION 
            || daughter.NumberDensity == 0.
            || daughter.EnergyDensity == 0.
        ){
            continue;
        }
        if ( daughter.TotalWidth > 0.){
            auto injections = calculate_injection_contribution(parent, daughter, data_.CurrentPoint.Hubble);
            builder.NumberDensityEquation += injections.NumberDensityEquation;
            builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += injections.NumberDensityJacobian[ 2 * parent.EqnIndex ];
            builder.NumberDensityJacobian[ 2 * daughter.EqnIndex ] += injections.NumberDensityJacobian[ 2 * daughter.EqnIndex ];
            builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += injections.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ];
            builder.NumberDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += injections.NumberDensityJacobian[ 2 * daughter.EqnIndex + 1 ];

            builder.EnergyDensityEquation += injections.EnergyDensityEquation;
            builder.EnergyDensityJacobian[ 2 * parent.EqnIndex ] += injections.EnergyDensityJacobian[ 2 * parent.EqnIndex ];
            builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex ] += injections.EnergyDensityJacobian[ 2 * daughter.EqnIndex ];
            builder.EnergyDensityJacobian[ 2 * parent.EqnIndex + 1 ] += injections.EnergyDensityJacobian[ 2 * parent.EqnIndex + 1 ];
            builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += injections.EnergyDensityJacobian[ 2 * daughter.EqnIndex + 1 ];
        }
    }

    return builder;
}

// This method is a helper function to calculate the injection terms from a specified "parent" to a specified "daughter"
// This method is called in the calculate_injection_terms method
// This is the last term in e.g. Eqs. 2.4, 2.5, 2.6 in 1406.4138
// Note that sum is taken care of by the loop in the calculate_injection_terms method
// THE CONVENTION IN THIS METHOD IS WEIRD, HAVE THE DAUGHTER DECAY INTO PARENT...
// TODO: SHOULD REDO THIS SO LESS CONFUSING...
ComponentBuilder BoltzmannBuilder::calculate_injection_contribution(const ParticleData& parent, const ParticleData& daughter, const long double& hubble){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );
    long double n1 = parent.NumberDensity;
    long double rhoN1 = parent.EnergyDensity / parent.NumberDensity;
    long double n2 = daughter.NumberDensity;
    long double rhoN2 = daughter.EnergyDensity / daughter.NumberDensity;

    if (parent.NumberDensity==0.) rhoN1 = parent.Mass;
    if (daughter.NumberDensity==0.) rhoN2 = daughter.Mass;

    long double widthXmass2 = Br( daughter, parent ) * daughter.TotalWidth * daughter.Mass;
    // if "parent" is R-even, assume 2 identical particles in final state so need to multiply n and rho by 2
    if ( (int)daughter.Charges[0].Value == 1 ){
        widthXmass2 = 2. * widthXmass2;
    }

    if (parent.EquilibriumNumberDensity > 0. 
        && daughter.EquilibriumNumberDensity > 0. 
        && daughter.ProductionMechanism == ParticleProductionMechanism::THERMAL
    ){
        long double nRatioDaughters = 0.;
        if ( (int)daughter.Charges[0].Value == -1 && (int)parent.Charges[0].Value == -1 ){
            // if parent is R-odd and child is R-odd, assume other child is effectively radiation in thermal equilibrium (R-even)
            nRatioDaughters = parent.NumberDensity / parent.EquilibriumNumberDensity;
        }
/*        else if ( (int)daughter.Charges[0].Value == -1 && (int)parent.Charges[0].Value == 1 ){
            // if parent is R-odd and child is R-even
            nRatioDaughters = 1.;
        }*/
        else if ( (int)daughter.Charges[0].Value == 1 && (int)parent.Charges[0].Value == 1 ){
            // if parent is R-even and child is R-even, take both daughters to be the same (don't assume thermal eq)
            nRatioDaughters = pow( parent.NumberDensity / parent.EquilibriumNumberDensity, 2. );
        }
        else if ( (int)daughter.Charges[0].Value == 1 && (int)parent.Charges[0].Value == -1 ){
            // if parent is R-even and child is R-odd, take both daughters to be the same (not thermal eq)
            nRatioDaughters = pow( parent.NumberDensity / parent.EquilibriumNumberDensity, 2. );
        }

        // can get overflows due to absurdly low NEQ - set to 0 manually since machine can't interpret inf*0
        if ( std::isinf(nRatioDaughters) || std::isnan(nRatioDaughters) ){
            nRatioDaughters = 0.;
        }

        // these equations have both the injection and the "inverse decay" injection terms
        builder.NumberDensityEquation += widthXmass2 * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * rhoN2 * n1 );
        builder.EnergyDensityEquation += widthXmass2 * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) * ( 0.5 - rhoN1 / rhoN2 ) / ( hubble * parent.EnergyDensity );

        builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += - widthXmass2 * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * rhoN2 * n1 );
        builder.NumberDensityJacobian[ 2 * daughter.EqnIndex ] += widthXmass2 * 2. * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * rhoN2 * n1 );
        builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += widthXmass2 * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * rhoN2 * n1 );
        builder.NumberDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += - widthXmass2 * 3. * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * rhoN2 * n1 );
        builder.NumberDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += - 1./3.* widthXmass2 * n2 * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( pow(hubble, 3.) * 2. * n1 * 5.95e36 );
//  dH/drho?

        builder.EnergyDensityJacobian[ 2 * parent.EqnIndex + 1 ] += - widthXmass2 * ( 0.5 + ( rhoN1 / rhoN2 ) ) * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * parent.EnergyDensity );
        builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += - widthXmass2 * ( 0.5 - 3. * ( rhoN1 / rhoN2 ) ) * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * parent.EnergyDensity );

        // dH / drho?
        builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += - 1./3. * daughter.EnergyDensity * widthXmass2 * ( 0.5 - (rhoN1/rhoN2)) * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / (pow(hubble, 3.) * 2. * parent.EnergyDensity * 5.95e36 );
        builder.EnergyDensityJacobian[ 2 * parent.EqnIndex + 1 ] += - 1./3. * widthXmass2 * ( 0.5 - (rhoN1/rhoN2)) * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( pow(hubble, 3.) * 2. * 5.95e36 );

        builder.EnergyDensityJacobian[ 2 * parent.EqnIndex ] += widthXmass2 *( rhoN1 / rhoN2 ) * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * parent.EnergyDensity );
        builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex ] += widthXmass2 * ( 0.5 - 2. * ( rhoN1 / rhoN2 ) ) * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * parent.EnergyDensity );
    } else{
        // these are the cases where neq = 0, so no "inverse decay" injections
        // applies to coh. osc. fields, and to fields with neq=0 -> effectively "frozen out" so inverse injection should be negligible

        double nEOM = widthXmass2 * n2 / ( hubble * rhoN2 * ( n1 + nRegularizer_ ) );
        double rhoEOM = widthXmass2 * n2 * ( 0.5 - rhoN1 / rhoN2 ) / ( hubble * ( parent.EnergyDensity + nRegularizer_ ) );
        builder.NumberDensityEquation += nEOM;
        builder.EnergyDensityEquation += rhoEOM;

        double daughterRelativisticFactor = daughter.Mass * daughter.NumberDensity / ( daughter.EnergyDensity + nRegularizer_ );
        double parentRelativisticFactor = parent.Mass * parent.NumberDensity / ( parent.EnergyDensity + nRegularizer_ );

        double sigma = 10.;
        double dampFactor = ( 1. - expl( - sigma * pow( 0.5 - rhoN1 / rhoN2, 2. ) ) ) / ( 1. - expl( - sigma * 0.25 ) );
        double dampFactor2 = ( expl( - sigma * pow( std::min(parentRelativisticFactor, 1.), 2. ) ) - expl(-sigma) ) / (1. - expl(-sigma) );
        double dampFactor3 = ( expl( - sigma * pow( std::min(daughterRelativisticFactor, 1.), 2. ) ) - expl(-sigma) ) / (1. - expl(-sigma) );
        if (daughter.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION){
            dampFactor3 = 0.;
        }

        builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += - nEOM;
        builder.NumberDensityJacobian[ 2 * daughter.EqnIndex ] += nEOM;
        // this term is only significant until the particle becomes non-relativistic, after which it goes to zero
//        builder.NumberDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += - 3. * nEOM * dampFactor3;

        double relativisticDGDN = rhoEOM;
        double relativisticDG2DN = - widthXmass2 * n2 * 0.5 / ( hubble * ( parent.EnergyDensity + nRegularizer_ ) );
        double relativisticDG3DN = widthXmass2 * n2 * ( rhoN1 / rhoN2 ) / ( hubble * ( parent.EnergyDensity + nRegularizer_ ) );
        
//        builder.EnergyDensityJacobian[ 2 * parent.EqnIndex ] += - relativisticDGDN * dampFactor;
        builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex ] += rhoEOM * dampFactor * dampFactor2;
        builder.EnergyDensityJacobian[ 2 * parent.EqnIndex + 1 ] += - rhoEOM * dampFactor;
//        builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += relativisticDG3DN * dampFactor * dampFactor2 * dampFactor3;
    }

    return builder;
}

const Models::Particle& BoltzmannBuilder::pullParticle(const boost::uuids::uuid& particleId) {
    // ---- 1. Check in-memory cache ----
    auto it = particleCache_.find(particleId);
    if (it != particleCache_.end()) {
        return it->second;
    }

    // ---- 2. Search in already-loaded particles ----
    auto r = std::find_if(
        data_.Particles.begin(), data_.Particles.end(),
        [&particleId](const Models::Particle& part) {
            return part.Id == particleId;
        });

    if (r != data_.Particles.end()) {
        const auto& inserted = particleCache_.emplace(particleId, *r).first->second;
        return inserted;
    }

    // ---- 3. Fallback to DB (should happen only once per unknown particle) ----
    db_.Open();
    Models::Particle p;
    auto statement = Statements::Particle(p, Statements::StatementType::Read);
    auto filter = Filters::Particle(particleId, Filters::WhereUUID::Id);
    statement.AddFilter(filter);
    auto cb = Callbacks::Particle();
    db_.Execute(statement, cb.Callback, cb.CallbackReturn);
    db_.Close();

    if (cb.CallbackReturn.Particles.size() != 1)
        throw std::logic_error("Could not pull unique particle");

    const auto& loaded = cb.CallbackReturn.Particles.front();
    const auto& inserted = particleCache_.emplace(particleId, loaded).first->second;
    return inserted;
}

// this helper function computes R-odd to the LSP, assuming 2-body cascade decay to R-odd+R-even ends in 1 LSP
// excludes species tracked separately
double BoltzmannBuilder::Br_Rodd_to_LSP( const ParticleData& parent, const ParticleData& daughter ){
    double br = 0.;
    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        // if not 2-body decay, continue 
        // TODO: handle higher body decays
        if ( width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure exactly one child is R-odd
        if ( 
            ( (int)child1.Charges[0].Value == -1 && (int)child2.Charges[0].Value == -1)
            || ( (int)child1.Charges[0].Value == 1 && (int)child2.Charges[0].Value == 1)
        ){
            continue;
        }            

        // don't include if 1 R-odd final state and IS tracked separately
        bool widthTrackedSeparately = false;
        for( auto& p : data_.ParticleDatas ){
            // only look at valid tracked R-odd decays...
            if ( p.ProductionMechanism != ParticleProductionMechanism::THERMAL || p.Charges[0].Value != -1 ){
                continue;
            }
            // TODO: FINAL CHECK HERE IS A BUG WORKAROUND WHERE OTHER PARTICLE HAS DECAYED AND IS TURNED OFF BEFORE PARENT HAS BEGUN TO DECAY
            // TREAT AS RADIATION IF HAS ALREADY DECAYED TO AT LEAST GET ENTROPY PRODUCTION IN CASCADE DECAYS
            // PROPER SOLUTION IS TO REWRITE EQUATIONS SO NON-SINGULAR WHEN SPECIES IS ENTIRELY DECAYED

            // make sure we don't accidentally discard direct parent->LSP decay
            if ( p.ParticleId != daughter.ParticleId 
                && (p.ParticleId == child1.Id || p.ParticleId == child2.Id)
//                && p.IsActive
            ){
                widthTrackedSeparately = true;
            }
        }
        if (!widthTrackedSeparately){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

// this helper function computes R-odd to R-odd where the daughter is NOT the LSP and is tracked separately
double BoltzmannBuilder::Br_Rodd_to_Rodd(const ParticleData& parent, const ParticleData& daughter){
    double br = 0.;
    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        // if not 2-body, or if NEITHER width ID corresponds to the daughter, keep going
        if ( width.ChildrenIds.size() != 2 || ( width.ChildrenIds[0] != daughter.ParticleId && width.ChildrenIds[1] != daughter.ParticleId ) ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure one child is R-odd
        if ( 
            ( (int)child1.Charges[0].Value == -1 && (int)child2.Charges[0].Value == -1)
            || ( (int)child1.Charges[0].Value == 1 && (int)child2.Charges[0].Value == 1)
        ){
            connection_.Log.Warn("Detected R-odd -> R-odd+R-odd/R-even+R-even width"); // this shouldn't ever happen, but should check to make sure
            continue;
        }            

        // take only the widths associated to child
        if ( child1.Key == daughter.ParticleKey || child2.Key == daughter.ParticleKey ){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

double BoltzmannBuilder::Br_Rodd_to_Reven(const ParticleData& parent, const ParticleData& daughter){
    double br = 0.;
    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        // if not 2-body, or if NEITHER width ID corresponds to the daughter, keep going
        if ( width.ChildrenIds.size() != 2 || ( width.ChildrenIds[0] != daughter.ParticleId && width.ChildrenIds[1] != daughter.ParticleId ) ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure one child is R-odd/one is R-even
        if ( 
            ( (int)child1.Charges[0].Value == -1 && (int)child2.Charges[0].Value == -1)
//            || ( (int)child1.Charges[0].Value == 1 && (int)child2.Charges[0].Value == 1)  // NOTE: we're temporarily removing this to support RPV models
        ){
            connection_.Log.Warn("Detected R-odd -> R-odd+R-odd/R-even+R-even width"); // this shouldn't ever happen, but should check to make sure
            continue;
        }            

        // take only the widths associated to child
        if ( child1.Key == daughter.ParticleKey || child2.Key == daughter.ParticleKey ){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

// this helper function computes R-odd to radiation, assuming any 2-body cascade decay ends in radiation release
// excludes species where both daughters are explicitly tracked separately
double BoltzmannBuilder::Br_Rodd_to_radiation(const ParticleData& parent, const ParticleData& daughter){
    double br = 1.;

    // check to make sure we don't include things tracked separately
    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        // only 2-body decays
        if ( width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure one child is R-odd
        if ( 
            ( (int)child1.Charges[0].Value == -1 && (int)child2.Charges[0].Value == -1)
//            || ( (int)child1.Charges[0].Value == 1 && (int)child2.Charges[0].Value == 1)  // NOTE: temporarily removing this to allow RPV models
        ){
            connection_.Log.Warn("Detected R-odd -> R-odd+R-odd/R-even+R-even width"); // this shouldn't ever happen, but should check to make sure
            continue;
        }            

        bool widthTrackedSeparately = false;
        for ( auto& p : data_.ParticleDatas ){
            // only look at valid R-odd decays
            if ( p.ProductionMechanism != ParticleProductionMechanism::THERMAL ){
                continue;
            }
            // if daughter is tracked separately and not radiation (already filtered), continue trying to match keys
            if ( p.ParticleKey == child1.Key ){
                // we've matched one key as tracked separately, but second key may not necessarily be same as first
                for ( auto& p2 : data_.ParticleDatas ){
                    // TODO: FINAL CHECK HERE IS A BUG WORKAROUND WHERE OTHER PARTICLE HAS DECAYED AND IS TURNED OFF BEFORE PARENT HAS BEGUN TO DECAY
                    // TREAT AS RADIATION IF HAS ALREADY DECAYED TO AT LEAST GET ENTROPY PRODUCTION IN CASCADE DECAYS
                    // PROPER SOLUTION IS TO REWRITE EQUATIONS SO NON-SINGULAR WHEN SPECIES IS ENTIRELY DECAYED
                    if ( p2.ParticleKey == child2.Key && p.ProductionMechanism != ParticleProductionMechanism::RADIATION 
//                        && p2.IsActive 
                    ){
                        // now we've verified both children in width are tracked separately and neither is radiation
                        widthTrackedSeparately = true;
                        break;
                    }
                }
            }
        }
        if ( widthTrackedSeparately ){
            br -= width.Width / parent.TotalWidth;
        }
    }
    return br;
}

double BoltzmannBuilder::Br_Reven_to_Reven(const ParticleData& parent, const ParticleData& daughter){
    double br = 0.;

    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        if ( width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure both children are R-even
        if ( (int)child1.Charges[0].Value != 1 || (int)child2.Charges[0].Value != 1 ){
            continue;
        }

        // if child is R-even and NOT radiation, take only the widths associated to child
        if ( child1.Key == daughter.ParticleKey || child2.Key == daughter.ParticleKey ){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

double BoltzmannBuilder::Br_Reven_to_radiation(const ParticleData& parent, const ParticleData& daughter){
    double br = 1.;

    // go through all partial widths of the parent
    // we assume that every cascade decay rapidly thermalizes so total decay can be treated as radiation
    // if a width matches a tracked key, don't include it since tracked separately
    // i.e. BR = 1 - species tracked separately
    // TODO: refactor this and make more efficient...
    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        if ( width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        bool widthTrackedSeparately = false;
        for ( auto& p : data_.ParticleDatas ){
            if ( p.ProductionMechanism != ParticleProductionMechanism::THERMAL ){
                continue;
            }
            if ( p.ParticleKey == child1.Key ){
                for ( auto& p2 : data_.ParticleDatas ){
                    // TODO: FINAL CHECK HERE IS A BUG WORKAROUND WHERE OTHER PARTICLE HAS DECAYED AND IS TURNED OFF BEFORE PARENT HAS BEGUN TO DECAY
                    // TREAT AS RADIATION IF HAS ALREADY DECAYED TO AT LEAST GET ENTROPY PRODUCTION IN CASCADE DECAYS
                    // PROPER SOLUTION IS TO REWRITE EQUATIONS SO NON-SINGULAR WHEN SPECIES IS ENTIRELY DECAYED
                    if ( p2.ParticleKey == child2.Key && p2.ProductionMechanism == ParticleProductionMechanism::THERMAL 
//                        && p2.IsActive 
                    ){
                        widthTrackedSeparately = true;
                        break;
                    }
                }
            }
        }
        if ( widthTrackedSeparately ){
            br -= width.Width / parent.TotalWidth;
        }
    }
    return br;
}

// this method computes R-even -> LSP + LSP, assuming 2-body cascade decay ends in 1 LSP
// excludes if BOTH children are tracked separately 
double BoltzmannBuilder::Br_Reven_to_LSP(const ParticleData& parent, const ParticleData& daughter){
    double br = 0.;

    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        // only continue if 2-body decay
        if ( width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // TODO: REMOVE THIS - HACK FOR DISS
//        if ( child1.Key.find("higgs") != std::string::npos && child2.Key.find("higgs") != std::string::npos){
//            br += 0.01 * width.Width / parent.TotalWidth;
//        }

        // check to make sure both children are R-odd
        if ( (int)child1.Charges[0].Value == 1 || (int)child2.Charges[0].Value == 1 ){
            continue;
        }

        bool widthTrackedSeparately = false;
        for ( auto& p : data_.ParticleDatas ){
            // only look at valid R-odd decays
            if ( p.ProductionMechanism != ParticleProductionMechanism::THERMAL || p.Charges[0].Value != -1 ){
                continue;
            }
            // if daughter is tracked separately and NOT LSP, continue trying to match keys
            if ( p.ParticleKey == child1.Key && !p.IsLSP ){
                // we've matched one key as tracked separately, but second key may not necessarily be same as first
                for ( auto& p2 : data_.ParticleDatas ){
                    // TODO: FINAL CHECK HERE IS A BUG WORKAROUND WHERE OTHER PARTICLE HAS DECAYED AND IS TURNED OFF BEFORE PARENT HAS BEGUN TO DECAY
                    // TREAT AS RADIATION IF HAS ALREADY DECAYED TO AT LEAST GET ENTROPY PRODUCTION IN CASCADE DECAYS
                    // PROPER SOLUTION IS TO REWRITE EQUATIONS SO NON-SINGULAR WHEN SPECIES IS ENTIRELY DECAYED
                    if ( p2.ParticleKey == child2.Key && !p2.IsLSP 
//                        && p2.IsActive 
                    ){
                        // now we've verified both children in width are tracked separately, and NEITHER is the LSP
                        widthTrackedSeparately = true;
                        break;
                    }
                }
            }
        }
        if ( !widthTrackedSeparately ){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

double BoltzmannBuilder::Br_Reven_to_Rodd(const ParticleData& parent, const ParticleData& daughter){
    double br = 0.;

    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        if ( width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure both children are R-odd
        if ( (int)child1.Charges[0].Value == 1 || (int)child2.Charges[0].Value == 1 ){
            continue;
        }

        // if child is R-odd and NOT the LSP, take only the widths associated to child
        if ( child1.Key == daughter.ParticleKey || child2.Key == daughter.ParticleKey ){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

double BoltzmannBuilder::Br(const ParticleData& parent, const ParticleData& daughter){    
    // if the daughter is kinematically inaccessible, no point in continuing - but can't tell other particle, so only look at parent/daughter masses
    if (
        parent.Mass <= daughter.Mass 
//        || !daughter.IsActive
    ){
        return 0.;
    }
    
    // if parent is R-odd and 2 body decay, one child is R-odd and one is R-even
    // since we're analyzing each child separately in the function calling this method, only look at the final "cascade decay" products
    if ( (int)parent.Charges[0].Value == -1 ){
        if ( (int)daughter.Charges[0].Value == -1 && !daughter.IsLSP ){
            return Br_Rodd_to_Rodd( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == -1 && daughter.IsLSP ){
            return Br_Rodd_to_LSP( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == 1 && daughter.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
            // if child is R-even, effectively SM - so can treat as radiation
            return Br_Rodd_to_radiation( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == 1 && daughter.ProductionMechanism == ParticleProductionMechanism::THERMAL ){
            return Br_Rodd_to_Reven( parent, daughter );
        }
    }

    // if parent is R-even and 2 body decay, both children are either R-odd or R-even
    if ( (int)parent.Charges[0].Value == 1 ){
        if ( (int)daughter.Charges[0].Value == -1 && !daughter.IsLSP ){
            return Br_Reven_to_Rodd( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == -1 && daughter.IsLSP ){
            return Br_Reven_to_LSP( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == 1 && daughter.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
            return Br_Reven_to_radiation( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == 1 && daughter.ProductionMechanism == ParticleProductionMechanism::THERMAL ){
            return Br_Reven_to_Reven( parent, daughter );
        }
    }

    return 0;
}


// This method assembles a particle's number density and energy density Boltzmann equations
// This method also adds the particle's contribution to the entropy equation
ComponentBuilder BoltzmannBuilder::Build_Particle_Boltzmann_Eqs(const double& t, const ParticleData &particle, const ParticleData &rad){
    ComponentBuilder builder( 2 * data_.ParticleDatas.size() );
    builder.EvolutionId = particle.ParticleEvolutionId;
    // also if not active (already decayed, or coh. osc. modes not oscillating), don't calculate equations
    // for coh. osc. modes, densities are frozen until start oscillating
    // in this case, just return an empty builder
    if (!particle.IsActive){
        return builder;
    }

    auto expansion = calculate_hubble_contribution(particle);
    builder.NumberDensityEquation += expansion.NumberDensityEquation;
    for (size_t j = 0; j < expansion.NumberDensityJacobian.size(); ++j){
        builder.NumberDensityJacobian[j] += expansion.NumberDensityJacobian[j];
    }

    // only continue if we have "valid" densities - solver adapts, no point in continuing if bad
    if (
        particle.NumberDensity > 0. 
    ){
        if ( particle.TotalWidth > 0.){
            auto entropy = calculate_particle_entropy(t, particle, rad);
            // build entropy equation solely from calculate_particle_entropy method
            builder.EntropyEquation += entropy.EntropyEquation;
            builder.EntropyJacobian[ 0 ] += entropy.EntropyJacobian[ 0 ];
            builder.EntropyJacobian[ 2 * particle.EqnIndex ] += entropy.EntropyJacobian[ 2 * particle.EqnIndex ];
            builder.EntropyJacobian[ 2 * particle.EqnIndex + 1 ] += entropy.EntropyJacobian[ 2 * particle.EqnIndex + 1 ];

            auto decay = calculate_decay_terms(particle);
            builder.NumberDensityEquation += decay.NumberDensityEquation;
            for (size_t j = 0; j < decay.NumberDensityJacobian.size(); ++j){
                builder.NumberDensityJacobian[j] += decay.NumberDensityJacobian[j];
                builder.EnergyDensityJacobian[j] += decay.EnergyDensityJacobian[j];
            }
        }

        // the rest of the terms apply only to thermally produced species 
        // radiation captured in entropy eqn, and coh. osc. don't annihilate or get produced via decays and have 0 pressure
        if ( particle.ProductionMechanism == ParticleProductionMechanism::THERMAL ){
            auto annihilations = calculate_annihilation_term( particle, rad );
            auto injections = calculate_injection_terms( particle );
            auto pressure = calculate_pressure_term( particle );

            builder.NumberDensityEquation += annihilations.NumberDensityEquation;
            builder.NumberDensityEquation += injections.NumberDensityEquation;
            builder.EnergyDensityEquation += injections.EnergyDensityEquation;
            builder.EnergyDensityEquation += pressure.EnergyDensityEquation;

            // handle the jacobians here
            for (size_t j = 0; j < builder.NumberDensityJacobian.size(); ++j){
                builder.NumberDensityJacobian[j] += annihilations.NumberDensityJacobian[j];
                builder.EnergyDensityJacobian[j] += annihilations.EnergyDensityJacobian[j];
                builder.NumberDensityJacobian[j] += injections.NumberDensityJacobian[j];
                builder.EnergyDensityJacobian[j] += injections.EnergyDensityJacobian[j];
                builder.EnergyDensityJacobian[j] += pressure.EnergyDensityJacobian[j];
            }
        }
    }

    return builder;
}
