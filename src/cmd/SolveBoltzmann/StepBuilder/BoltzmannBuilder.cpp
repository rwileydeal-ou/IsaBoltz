#include <cmd/SolveBoltzmann/StepBuilder/BoltzmannBuilder.h>

using namespace std;

BoltzmannBuilder::BoltzmannBuilder(
    Connection& connection, 
    const BoltzmannData& data,
    std::unordered_map< boost::uuids::uuid, Models::Particle, boost::hash<boost::uuids::uuid> >& particleCache
) : 
    connection_(connection),
    data_(data),
    db_(connection),
    particleCache_(particleCache)
{
}
BoltzmannBuilder::~BoltzmannBuilder(){
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
void BoltzmannBuilder::calculate_particle_entropy(
    const double& t, 
    const ParticleData& particle, 
    const ParticleData& rad,
    ComponentBuilder& builder
){
    auto cascadeBr = CascadeBranchingRatios(connection_, data_, particleCache_);
    long double widthXmass = cascadeBr.Calculate(particle, rad) * particle.TotalWidth * particle.Mass;
    long double expTerm = expl( particle.Y1 + 3. * t - rad.Y1 );
    if ( particle.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION ){
        builder.EntropyEquation += widthXmass * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
        builder.EntropyJacobian[ 0 ] += - widthXmass * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
        builder.EntropyJacobian[ 2 * particle.EqnIndex ] += widthXmass * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
        builder.EntropyJacobian[ 2 * particle.EqnIndex + 1 ] += 0.;//- widthXmass * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
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
        builder.EntropyJacobian[ 2 * particle.EqnIndex + 1 ] += 0.;//- widthXmass * ( 1. - nRatio ) * expTerm / ( data_.CurrentPoint.Hubble * data_.CurrentPoint.Temperature );
    }
}

// This method computes the "hubble dilution" term in the number density equation
void BoltzmannBuilder::calculate_hubble_contribution(
    const ParticleData &particle, 
    ComponentBuilder& builder
){
    // This is the Hubble expansion term (dividing by n_i H)
    builder.NumberDensityEquation += -3.; 
}

// This method computes the "pressure" term in the energy density equation
void BoltzmannBuilder::calculate_pressure_term(
    const ParticleData& parent,
    ComponentBuilder& builder
){
    long double rhoN1 = parent.EnergyDensity / parent.NumberDensity;

    // This is first term on RHS in e.g. Eq 2.6 from arXiv 1406.4138
    // Both sides were multiplied by n_i / (H * rho_i) = 1 / (H * Y2) with our defn of Y2
    // This makes diff eq. defined WRT log(rho_i / n_i))
    long double P1num = 3. * getPN(rhoN1, parent.Mass) / rhoN1;
    builder.EnergyDensityEquation += -P1num;
//    builder.EnergyDensityJacobian[  2 * parent.EqnIndex ] += P1num; // this is correct assuming P doesn't depend on rho/n
//    builder.EnergyDensityJacobian[  2 * parent.EqnIndex + 1 ] += P1num;
}

// This method builds the contribution from the annihilation terms from the specified particle
void BoltzmannBuilder::calculate_annihilation_term( 
    const ParticleData& parent, 
    const ParticleData& rad,
    ComponentBuilder& builder
){
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
}

// This method calculates the decay terms from the specified parent 
void BoltzmannBuilder::calculate_decay_terms(
    const ParticleData& parent,
    ComponentBuilder& builder
){
    if (parent.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION){
        calculate_cohOsc_field_decay(parent, data_.CurrentPoint.Hubble, builder);
    } else if (parent.ProductionMechanism == ParticleProductionMechanism::THERMAL){
        calculate_thermal_particle_decays(parent, data_.CurrentPoint.Hubble, builder);
    }
}

// This method is a helper function to calculate the decays of thermally produced particles
// called in the calculate_decay_terms method for the appropriate parent types
void BoltzmannBuilder::calculate_thermal_particle_decays(
    const ParticleData& parent, 
    const long double& hubble,
    ComponentBuilder& builder
){
    double relativisticFactor = parent.Mass * parent.NumberDensity / ( parent.EnergyDensity + nRegularizer_ );
    if ( !std::isfinite(relativisticFactor) || relativisticFactor > 1. ){
        relativisticFactor = 1.;
    }
    // Decay contribution
    // This is just the Gamma*mass*n/rho*H term
    builder.NumberDensityEquation += - parent.TotalWidth * relativisticFactor / hubble;
    builder.NumberDensityJacobian[2 * parent.EqnIndex+1] += parent.TotalWidth * relativisticFactor / hubble;
         
    // These are inverse decay contributions
    for ( auto& daughter : data_.ParticleDatas ){
        if ( 
            !daughter.IsActive 
            || daughter.ParticleEvolutionId == parent.ParticleEvolutionId 
            || daughter.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION
        ){
            continue;
        }
        if ( 
            ( 
                daughter.ProductionMechanism == ParticleProductionMechanism::THERMAL 
                && daughter.EquilibriumNumberDensity > 0. 
            ) || daughter.ProductionMechanism == ParticleProductionMechanism::RADIATION 
        ){
            calculate_inverse_decay(parent, daughter, hubble, builder);
        }
    }
}

void BoltzmannBuilder::calculate_inverse_decay(
    const ParticleData& parent, 
    const ParticleData& daughter, 
    const long double& hubble,
    ComponentBuilder& builder
){
    // This term is the prefactor to the decay piece, Gamma_i * m_i / (rho_i * H), which then multiplies the rest of the decay piece
    // (see e.g. sum in the second term on RHS of Eq. 2.4 in arXiv 1406.4138, note EQ was divided by n_i * H for computation)
    long double preFactor = parent.TotalWidth * parent.Mass / ( hubble * ( parent.EnergyDensity + nRegularizer_ ) );
    long double postFactor = 0.;

    auto cascadeBr = CascadeBranchingRatios(connection_, data_, particleCache_);
    
    if ( daughter.ProductionMechanism == ParticleProductionMechanism::THERMAL ){
        if ( (int)parent.Charges[0].Value == -1 && (int)daughter.Charges[0].Value == -1 ){
            // if parent is R-odd and child is R-odd, assume other child is effectively radiation in thermal equilibrium (R-even)
            // We'll use this variable for the term in parentheses within e.g. the second term of RHS of Eq. 2.4 of arXiv 1406.4138
            long double nEff = (
                daughter.NumberDensity / ( daughter.EquilibriumNumberDensity + nRegularizer_ )
            ) * parent.EquilibriumNumberDensity;
            postFactor = cascadeBr.Calculate(parent, daughter) * nEff;
            builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += - preFactor * postFactor;
            builder.NumberDensityJacobian[ 2 * daughter.EqnIndex ] += preFactor * postFactor;
            builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += - preFactor * postFactor;
        }
/*        else if ( (int)parent.Charges[0].Value == -1 && (int)daughter.Charges[0].Value == 1 ){
            // if parent is R-odd and child is R-even
            long double nEff = parent.EquilibriumNumberDensity;
            postFactor = cascadeBr.Calculate(parent, daughter) * nEff;
            builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += - preFactor * postFactor;
            builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += - preFactor * postFactor;
        }*/
        else if ( (int)parent.Charges[0].Value == 1 && (int)daughter.Charges[0].Value == 1 ){
            // if parent is R-even and child is R-even, take both daughters to be the same (don't assume thermal eq)
            long double nEff = pow( 
                daughter.NumberDensity / ( daughter.EquilibriumNumberDensity + nRegularizer_ ), 2. 
            ) * parent.EquilibriumNumberDensity;
            postFactor = cascadeBr.Calculate(parent, daughter) * nEff;
            builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += - preFactor * postFactor;
            builder.NumberDensityJacobian[ 2 * daughter.EqnIndex ] += 2. * preFactor * postFactor;
            builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += - preFactor * postFactor;
        }
        else if ( (int)parent.Charges[0].Value == 1 && (int)daughter.Charges[0].Value == -1 ){
            // if parent is R-even and child is R-odd, take both daughters to be the same (not thermal eq)
            long double nEff = pow( 
                daughter.NumberDensity / ( daughter.EquilibriumNumberDensity + nRegularizer_ ), 2. 
            ) * parent.EquilibriumNumberDensity;
            // if decays to R-odd pair, end up with 2 neutralinos in final state so need to multiply n and rho by 2
            postFactor = 2. * cascadeBr.Calculate(parent, daughter) * nEff;
            builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += - preFactor * postFactor;
            builder.NumberDensityJacobian[ 2 * daughter.EqnIndex ] += 2. * preFactor * postFactor;
            builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += - preFactor * postFactor;
        }
    } else if ( daughter.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
        // if the daughter is radiation, assume a = b = radiation, so n_a / neq_a = n_b / neq_b = 1
        long double nEff = parent.EquilibriumNumberDensity;
        postFactor = cascadeBr.Calculate(parent, daughter) * nEff;
        builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += - preFactor * postFactor;
        builder.NumberDensityJacobian[ 2 * parent.EqnIndex + 1 ] += - preFactor * postFactor;
    }

    // Now we'll assemble the pre-sum piece with the sum piece
    builder.NumberDensityEquation += preFactor * postFactor;
}


// This method is a helper function to calculate the decays of coherently oscillating fields
// called in the calculate_decay_terms method for the appropriate parent types
void BoltzmannBuilder::calculate_cohOsc_field_decay(
    const ParticleData& parent, 
    const long double& hubble,
    ComponentBuilder& builder
){
    // For coh. osc. fields, have normal decay piece, but no inverse decays
    // This is then just the Gamma*mass*n / rho*H term, but rho=n*m
    builder.NumberDensityEquation += - parent.TotalWidth / hubble;
}

// This method computes the "injection" terms in number & energy density eqns 
// i.e. for species "i", decay of "a" produces "i" - applies only to thermal/radiation-type species "i"
void BoltzmannBuilder::calculate_injection_terms(
    const ParticleData& parent, 
    ComponentBuilder& builder
){

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
            calculate_injection_contribution(parent, daughter, data_.CurrentPoint.Hubble, builder);
        }
    }
}

// This method is a helper function to calculate the injection terms from a specified "parent" to a specified "daughter"
// This method is called in the calculate_injection_terms method
// This is the last term in e.g. Eqs. 2.4, 2.5, 2.6 in 1406.4138
// Note that sum is taken care of by the loop in the calculate_injection_terms method
// THE CONVENTION IN THIS METHOD IS WEIRD, HAVE THE DAUGHTER DECAY INTO PARENT...
// TODO: SHOULD REDO THIS SO LESS CONFUSING...
void BoltzmannBuilder::calculate_injection_contribution(
    const ParticleData& parent, 
    const ParticleData& daughter, 
    const long double& hubble,
    ComponentBuilder& builder
){
    long double n1 = parent.NumberDensity;
    long double rhoN1 = parent.EnergyDensity / parent.NumberDensity;
    long double n2 = daughter.NumberDensity;
    long double rhoN2 = daughter.EnergyDensity / daughter.NumberDensity;

    if (parent.NumberDensity==0.) rhoN1 = parent.Mass;
    if (daughter.NumberDensity==0.) rhoN2 = daughter.Mass;

    auto cascadeBr = CascadeBranchingRatios(connection_, data_, particleCache_);
    long double widthXmass2 = cascadeBr.Calculate( daughter, parent ) * daughter.TotalWidth * daughter.Mass;

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

        builder.EnergyDensityJacobian[ 2 * parent.EqnIndex + 1 ] += - widthXmass2 * ( 0.5 + ( rhoN1 / rhoN2 ) ) * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * parent.EnergyDensity );
        builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += - widthXmass2 * ( 0.5 - 3. * ( rhoN1 / rhoN2 ) ) * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * parent.EnergyDensity );
        builder.EnergyDensityJacobian[ 2 * parent.EqnIndex ] += widthXmass2 *( rhoN1 / rhoN2 ) * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * parent.EnergyDensity );
        builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex ] += widthXmass2 * ( 0.5 - 2. * ( rhoN1 / rhoN2 ) ) * ( n2 - daughter.EquilibriumNumberDensity * nRatioDaughters ) / ( hubble * parent.EnergyDensity );
    } else{
        // these are the cases where neq = 0, so no "inverse decay" injections
        // applies to coh. osc. fields, and to fields with neq=0 -> effectively "frozen out" so inverse injection should be negligible

        double nEOM = widthXmass2 * n2 / ( hubble * rhoN2 * ( n1 + nRegularizer_ ) );
        double rhoEOM = widthXmass2 * n2 * ( 0.5 - rhoN1 / rhoN2 ) / ( hubble * ( parent.EnergyDensity + nRegularizer_ ) );
        builder.NumberDensityEquation += nEOM;
        builder.EnergyDensityEquation += rhoEOM;

        builder.NumberDensityJacobian[ 2 * parent.EqnIndex ] += - nEOM;
        builder.NumberDensityJacobian[ 2 * daughter.EqnIndex ] += nEOM;
        builder.NumberDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += - nEOM;        
        builder.EnergyDensityJacobian[ 2 * parent.EqnIndex ] += - rhoEOM;
        builder.EnergyDensityJacobian[ 2 * parent.EqnIndex + 1 ] += - widthXmass2 * n2 * ( 0.5 / rhoN1 ) / ( hubble * ( parent.NumberDensity + nRegularizer_ ) );
        builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex ] += rhoEOM;
        builder.EnergyDensityJacobian[ 2 * daughter.EqnIndex + 1 ] += widthXmass2 * n2 * ( rhoN1 / rhoN2 ) / ( hubble * ( parent.EnergyDensity + nRegularizer_ ) );
    }
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

    calculate_hubble_contribution(particle, builder);

    // only continue if we have "valid" densities - solver adapts, no point in continuing if bad
    if (
        particle.NumberDensity > 0. 
    ){
        if ( particle.TotalWidth > 0.){
            calculate_particle_entropy(t, particle, rad, builder);
            calculate_decay_terms(particle, builder);
        }

        // the rest of the terms apply only to thermally produced species 
        // radiation captured in entropy eqn, and coh. osc. don't annihilate or get produced via decays and have 0 pressure
        if ( particle.ProductionMechanism == ParticleProductionMechanism::THERMAL ){
            calculate_annihilation_term( particle, rad, builder );
            calculate_injection_terms( particle, builder );
            calculate_pressure_term( particle, builder );
        }
    }

    return builder;
}
