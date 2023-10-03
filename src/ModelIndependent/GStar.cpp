#include <ModelIndependent/GStar.h>

using namespace std;

GStar::GStar(/* args */){}
GStar::~GStar(){}

double GStar::integralEval(double numFactor, double denomFactor, double t0, double tf, double dt){
    state_type x0(1);
    x0[0] = 0.;
    // have to increment t0 slightly if ==0 since otherwise the exp(1/t) will give an unevaluate-able expression which gets carried through, but incrementing using smallest float precision value should get washed out in adaptive algorithm
    if (t0==0.){ t0 += numeric_limits<float>::min(); }

    // use a lambda expression to insert the integral needed into boost's integrate function
    auto result = boost::numeric::odeint::integrate(
        [numFactor, denomFactor](const state_type &x, state_type &dxdt, double t){
            if (pow(t,2.)*pow(numFactor,2.) > 1.){
                dxdt[0] = 0;
            } else{
                dxdt[0] = sqrt(1 - pow(t, 2.) * pow(numFactor, 2.) ) / ( pow(t, 5.) * ( exp(1. / t) + denomFactor ) );
            }
        },
        x0, t0, tf, dt
    );
    return (x0[0]*0.15399);
}

double GStar::defaultIntegralEval(const double& numFactor, const double& denomFactor){
    return integralEval(numFactor, denomFactor, 0., 1. / numFactor, 0.1);
}

Models::Particle GStar::findParticle( const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, std::string key ){
    for( auto& p : particles){
        if (p.Key == key){
            return p;
        }
    }
    throw_with_trace( logic_error("Could not find particle") );
}


double GStar::calculateQCD(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, const double& T, const CosmologicalTemperatures& temperatures){
    double gQCD = 0.;
    if (T < temperatures.Hadronization){
        auto gPion = GStar::defaultIntegralEval(abs(findParticle(particles, "pion0").Mass) / T, -1. );
        auto gEta = GStar::defaultIntegralEval(abs(findParticle(particles, "eta").Mass) / T, -1. );
        auto gRho = GStar::defaultIntegralEval(abs(findParticle(particles, "rho0").Mass) / T, -1. );
        auto gOmega = GStar::defaultIntegralEval(abs(findParticle(particles, "omega").Mass) / T, -1. );
        auto gKaon = GStar::defaultIntegralEval(abs(findParticle(particles, "kaon0").Mass) / T, -1. );
        gQCD = (4. * gPion) + (2. * gEta) + (3. * 2. * gRho) + (3. * 2. * gOmega) + (4. * gKaon);
    } else{
        double gGluon = ( 1. * 2. * 8. );
        double gLightQuark = ( (7. / 8.) * 3. ); // 3 gens of quarks approximated as massless
        auto gCharm = GStar::defaultIntegralEval(abs(findParticle(particles, "charmq").Mass) / T, 1. );
        auto gBottom = GStar::defaultIntegralEval(abs(findParticle(particles, "bottomq").Mass) / T, 1. );
        auto gTop = GStar::defaultIntegralEval(abs(findParticle(particles, "topq").Mass) / T, 1. );
        gQCD = gGluon + 4. * 3. * ( gLightQuark + gCharm + gBottom + gTop );
    }

    if (T > 0.15 && T < 0.25){
        double fit = 3.21029 + (91.7341 - 184.36 * T) / ( 1. + exp(100. * (0.2-T) ) );
        if (T < temperatures.Hadronization){
            gQCD = max(fit, gQCD);
        } else{
            gQCD = min(fit, gQCD);
        }
    }
    return gQCD;
}

double GStar::calculateGaugeBosons(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, const double& T, const CosmologicalTemperatures& temperatures){
    auto wBoson = findParticle(particles, "wboson");
    auto zBoson = findParticle(particles, "zboson");
    auto gW = GStar::defaultIntegralEval(abs(wBoson.Mass) / T, -1. );
    auto gZ = GStar::defaultIntegralEval(abs(zBoson.Mass) / T, -1. );
    auto gPhoton = 2.;

    double gGauges = 6. * gW + 3. * gZ + gPhoton;
    return gGauges;
}

double GStar::calculateLeptons(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, const double& T, const CosmologicalTemperatures& temperatures){
    auto electron = findParticle(particles, "electron");
    auto muon = findParticle(particles, "muon");
    auto tau = findParticle(particles, "tau");
    auto gEl = GStar::defaultIntegralEval(abs(electron.Mass) / T, 1. );
    auto gMu = GStar::defaultIntegralEval(abs(muon.Mass) / T, 1. );
    auto gTau = GStar::defaultIntegralEval(abs(tau.Mass) / T, 1. );

    double gNeutrinos = 0.;
    if (T > temperatures.NeutrinoDecouple){
        gNeutrinos = (7. / 8.) * 6.; // neutrinos are still coupled
    } else{
        gNeutrinos = (7. / 8.) * 6. * pow(4./11., 4./3.); // decoupled neutrinos
    }

    double gLeptons = 4. * ( gEl + gMu + gTau ) + gNeutrinos;
    return gLeptons;
}

double GStar::calculateSusy(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, const double& T, const CosmologicalTemperatures& temperatures){
    auto gluino = findParticle(particles, "gluino");
    auto higgsLight = findParticle(particles, "higgslight");
    auto higgsHeavy = findParticle(particles, "higgsheavy");
    auto higgsPseudo = findParticle(particles, "higgspseudoscalar");
    auto higgsCharged = findParticle(particles, "higgscharged");
    auto neutralino1 = findParticle(particles, "neutralino1");
    auto neutralino2 = findParticle(particles, "neutralino2");
    auto neutralino3 = findParticle(particles, "neutralino3");
    auto neutralino4 = findParticle(particles, "neutralino4");
    auto chargino1 = findParticle(particles, "chargino1");
    auto chargino2 = findParticle(particles, "chargino2");
    auto upl = findParticle(particles, "suplsq");
    auto upr = findParticle(particles, "suprsq");
    auto dnl = findParticle(particles, "sdownlsq");
    auto dnr = findParticle(particles, "sdownrsq");
    auto stl = findParticle(particles, "sstrangelsq");
    auto str = findParticle(particles, "sstrangersq");
    auto chl = findParticle(particles, "scharmlsq");
    auto chr = findParticle(particles, "scharmrsq");
    auto sb1 = findParticle(particles, "sbottom1sq");
    auto sb2 = findParticle(particles, "sbottom2sq");
    auto st1 = findParticle(particles, "stop1sq");
    auto st2 = findParticle(particles, "stop2sq");
    auto nuel = findParticle(particles, "sneutrinoselectronl");
    auto numu = findParticle(particles, "sneutrinosmuonl");
    auto nutau = findParticle(particles, "sneutrinostaul");
    auto sel = findParticle(particles, "selectronl");
    auto ser = findParticle(particles, "selectronr");
    auto smul = findParticle(particles, "smuonl");
    auto smur = findParticle(particles, "smuonr");
    auto stau1 = findParticle(particles, "stau1");
    auto stau2 = findParticle(particles, "stau2");

    auto gGl = GStar::defaultIntegralEval(abs(gluino.Mass) / T, 1.);
    // squarks
    auto gUpL = GStar::defaultIntegralEval(abs(upl.Mass) / T, -1. );
    auto gUpR = GStar::defaultIntegralEval(abs(upr.Mass) / T, -1.);
    auto gDnL = GStar::defaultIntegralEval(abs(dnl.Mass) / T, -1.);
    auto gDnR = GStar::defaultIntegralEval(abs(dnr.Mass) / T, -1.);
    auto gStL = GStar::defaultIntegralEval(abs(stl.Mass) / T, -1.);
    auto gStR = GStar::defaultIntegralEval(abs(str.Mass) / T, -1.);
    auto gChL = GStar::defaultIntegralEval(abs(chl.Mass) / T, -1.);
    auto gChR = GStar::defaultIntegralEval(abs(chr.Mass) / T, -1.);
    auto gB1 = GStar::defaultIntegralEval(abs(sb1.Mass) / T, -1.);
    auto gB2 = GStar::defaultIntegralEval(abs(sb2.Mass) / T, -1.);
    auto gT1 = GStar::defaultIntegralEval(abs(st1.Mass) / T, -1.);
    auto gT2 = GStar::defaultIntegralEval(abs(st2.Mass) / T, -1.);
    
    // sneutrinos
    auto gNuEl = GStar::defaultIntegralEval(abs(nuel.Mass) / T, -1.);
    auto gNuMl = GStar::defaultIntegralEval(abs(numu.Mass) / T, -1.);
    auto gNuTl = GStar::defaultIntegralEval(abs(nutau.Mass) / T, -1.);
    
    // sleptons 
    auto gEl = GStar::defaultIntegralEval(abs(sel.Mass) / T, -1.);
    auto gEr = GStar::defaultIntegralEval(abs(ser.Mass) / T, -1.);
    auto gMul = GStar::defaultIntegralEval(abs(smul.Mass) / T, -1.);
    auto gMur = GStar::defaultIntegralEval(abs(smur.Mass) / T, -1.);
    auto gTau1 = GStar::defaultIntegralEval(abs(stau1.Mass) / T, -1.);
    auto gTau2 = GStar::defaultIntegralEval(abs(stau2.Mass) / T, -1.);

    // neutralinos (for whatever reason, omit Z1, TODO: find out why) & charginos
    auto gZ2 = GStar::defaultIntegralEval(abs(neutralino2.Mass) / T, 1.);
    auto gZ3 = GStar::defaultIntegralEval(abs(neutralino3.Mass) / T, 1.);
    auto gZ4 = GStar::defaultIntegralEval(abs(neutralino4.Mass) / T, 1.);
    auto gW1 = GStar::defaultIntegralEval(abs(chargino1.Mass) / T, 1.);
    auto gW2 = GStar::defaultIntegralEval(abs(chargino2.Mass) / T, 1.);

    // higgses
    auto gHl0 = GStar::defaultIntegralEval(abs(higgsLight.Mass) / T, -1.);
    auto gHh0 = GStar::defaultIntegralEval(abs(higgsHeavy.Mass) / T, -1.);
    auto gHa0 = GStar::defaultIntegralEval(abs(higgsPseudo.Mass) / T, -1.);
    auto gHpls = GStar::defaultIntegralEval(abs(higgsCharged.Mass) / T, -1.);

    // now retrieve results and multiply by charge/color/etc factors
    double gGluino = 2. * 8. * gGl;
    double gSquarks = 2. * 3.* (gUpL + gUpR + gDnL + gDnR + gStL + gStR + gChL + gChR + gB1 + gB2 + gT1 + gT2);
    double gSneutrinos = gNuEl + gNuMl + gNuTl;
    double gSleptons = 2. * ( gEl + gEr + gMul + gMur + gTau1 + gTau2 );
    double gGauginos = 2. * (gZ2 + gZ3 + gZ4) + 4. * (gW1 + gW2);
    double gHiggs = gHl0 + gHh0 + gHa0 + 2. * (gHpls);

    return (gGluino + gSquarks + gSneutrinos + gSleptons + gGauginos + gHiggs);
}


double GStar::Calculate(const ModelBase& model, double T){
    auto susy = GStar::calculateSusy(model.Particles, T, model.Cosmology.Temperatures);
    auto leptons = GStar::calculateLeptons(model.Particles, T, model.Cosmology.Temperatures);
    auto qcd = GStar::calculateQCD(model.Particles, T, model.Cosmology.Temperatures);
    auto gb = GStar::calculateGaugeBosons(model.Particles, T, model.Cosmology.Temperatures);
    double gstr = (susy + leptons + qcd + gb);

    return gstr;
}

double GStar::CalculateEntropic(const ModelBase& model, double T){
    double gstr = Calculate(model, T);
    if (T <= model.Cosmology.Temperatures.NeutrinoDecouple){
        return ( gstr - (7. / 8.) * 6. * ( pow(4. / 11., 4. / 3.) - (4. / 11.) ) );
    }
    return gstr;
}

double GStar::Calculate(Connection& connection, double T){
    DbManager db(connection.SqlConnectionString, connection.Log);
    db.Open();

    Models::Particle p;
    auto statement = Statements::Particle(p, Statements::Read);
    auto filter = Filters::Particle( connection.InputId, Filters::WhereUUID::InputId );
    statement.AddFilter( filter );
    auto cb = Callbacks::Particle();
    db.Execute(statement, cb.Callback, cb.CallbackReturn);

    db.Close();
    if (cb.CallbackReturn.Particles.size() == 0){
        throw_with_trace( logic_error("Could not find particles!") );
    }

    auto susy = GStar::calculateSusy(cb.CallbackReturn.Particles, T, connection.Model.Cosmology.Temperatures);
    auto leptons = GStar::calculateLeptons(cb.CallbackReturn.Particles, T, connection.Model.Cosmology.Temperatures);
    auto qcd = GStar::calculateQCD(cb.CallbackReturn.Particles, T, connection.Model.Cosmology.Temperatures);
    auto gb = GStar::calculateGaugeBosons(cb.CallbackReturn.Particles, T, connection.Model.Cosmology.Temperatures);
    double gstr = (susy + leptons + qcd + gb);

    connection.Log.Trace("Calculated g_{*}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}

double GStar::CalculateEntropic(Connection& connection, double T){
    double gstr = Calculate(connection, T);
    if (T <= connection.Model.Cosmology.Temperatures.NeutrinoDecouple){
        return ( gstr - (7. / 8.) * 6. * ( pow(4. / 11., 4. / 3.) - (4. / 11.) ) );
    }
    connection.Log.Trace("Calculated g_{*S}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}

double GStar::Calculate(DbManager& db, Connection& connection, double T){
    Models::Particle p;
    auto statement = Statements::Particle(p, Statements::Read);
    auto filter = Filters::Particle( connection.InputId, Filters::WhereUUID::InputId );
    statement.AddFilter( filter );
    auto cb = Callbacks::Particle();
    db.Execute(statement, cb.Callback, cb.CallbackReturn);

    if (cb.CallbackReturn.Particles.size() == 0){
        throw_with_trace( logic_error("Could not find particles!") );
    }

    auto susy = GStar::calculateSusy(cb.CallbackReturn.Particles, T, connection.Model.Cosmology.Temperatures);
    auto leptons = GStar::calculateLeptons(cb.CallbackReturn.Particles, T, connection.Model.Cosmology.Temperatures);
    auto qcd = GStar::calculateQCD(cb.CallbackReturn.Particles, T, connection.Model.Cosmology.Temperatures);
    auto gb = GStar::calculateGaugeBosons(cb.CallbackReturn.Particles, T, connection.Model.Cosmology.Temperatures);
    double gstr = (susy + leptons + qcd + gb);

    connection.Log.Trace("Calculated g_{*}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}

double GStar::CalculateEntropic(DbManager& db, Connection& connection, double T, double gstr){
    if (T <= connection.Model.Cosmology.Temperatures.NeutrinoDecouple){
        return ( gstr - (7. / 8.) * 6. * ( pow(4. / 11., 4. / 3.) - (4. / 11.) ) );
    }
    connection.Log.Trace("Calculated g_{*S}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}


double GStar::Calculate(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, Connection& connection, double T){
    auto susy = GStar::calculateSusy(particles, T, connection.Model.Cosmology.Temperatures);
    auto leptons = GStar::calculateLeptons(particles, T, connection.Model.Cosmology.Temperatures);
    auto qcd = GStar::calculateQCD(particles, T, connection.Model.Cosmology.Temperatures);
    auto gb = GStar::calculateGaugeBosons(particles, T, connection.Model.Cosmology.Temperatures);
    double gstr = (susy + leptons + qcd + gb);

/*
    double gstr = 0.;
    double gQCD = 0.;

    for( auto& p : particles ){
        if ( T < connection.Model.Cosmology.Temperatures.Hadronization && ( p.Key == "charmq" || p.Key == "bottomq" || p.Key == "topq" ) ){
            continue;
        } else if ( T >= connection.Model.Cosmology.Temperatures.Hadronization && ( p.Key == "pion0" || p.Key == "eta" || p.Key == "rho0" || p.Key == "omega" || p.Key == "kaon0" ) ){
            continue;
        } else if ( p.Key == "photon" || p.Key == "gluon" || p.Key == "upq" || p.Key == "downq" || p.Key == "strangeq" || p.Key == "neutralino1" ){
            continue;
        } else if ( p.Key == "modulus" || p.Key == "axion" || p.Key == "saxion" || p.Key == "axino" ){
            continue;            
        }

        double den = 0.;
        if (p.Statistics == ParticleStatistics::Fermion){ den = 1.; }
        else{ den = -1.; }

        double gs = GStar::defaultIntegralEval(abs(p.Mass) / T, den);        

        if (p.Key == "gluino"){
            gstr += 16. * gs;
        } else if ( p.Key == "suplsq" || p.Key == "suprsq" || p.Key == "sdownlsq" || p.Key == "sdownrsq" || p.Key == "sstrangelsq" || p.Key == "sstrangersq" || p.Key == "scharmlsq" || p.Key == "scharmrsq" || p.Key == "sbottom1sq" || p.Key == "sbottom2sq" || p.Key == "stop1sq" || p.Key == "stop2sq" ){
            gstr += 6. * gs;
        } else if ( p.Key == "sneutrinoselectronl" || p.Key == "sneutrinosmuonl" || p.Key == "sneutrinostaul" ){
            gstr += gs;
        } else if ( p.Key == "selectronl" || p.Key == "selectronr" || p.Key == "smuonl" || p.Key == "smuonr" || p.Key == "stau1" || p.Key == "stau2" ){
            gstr += 2. * gs;
        } else if ( p.Key == "neutralino2" || p.Key == "neutralino3" || p.Key == "neutralino4" ){
            // neutralinos (for whatever reason, omit Z1, TODO: find out why)
            gstr += 2. * gs;
        } else if ( p.Key == "chargino1" || p.Key == "chargino2" ){
            gstr += 4. * gs;
        } else if ( p.Key == "higgslight" || p.Key == "higgsheavy" || p.Key == "higgspseudoscalar" ){
            gstr += gs;
        } else if ( p.Key == "higgscharged"){
            gstr += 2. * gs;
        } else if ( p.Key == "wboson"){
            gstr += 6. * gs;
        } else if ( p.Key == "zboson"){
            gstr += 3. * gs;
        } else if ( p.Key == "electron" || p.Key == "muon" || p.Key == "tau"){
            gstr += 4. * gs;
        }

        if (T < connection.Model.Cosmology.Temperatures.Hadronization){
            if ( p.Key == "pion0" ){
                gQCD += 4. * gs;
            } else if ( p.Key == "eta" ){
                gQCD += 2. * gs;
            } else if ( p.Key == "rho0" ){
                gQCD += 6. * gs;
            } else if ( p.Key == "omega" ){
                gQCD += 6. * gs;
            } else if ( p.Key == "kaon0" ){
                gQCD += 4. * gs;
            }
        } else{
            if ( p.Key == "charmq" || p.Key == "bottomq" || p.Key == "topq" ){
                gQCD += 12. * gs;
            }
        }
    }

    if ( T >= connection.Model.Cosmology.Temperatures.Hadronization ){
        // gluon
        gQCD += ( 1. * 2. * 8. );
        // light quarks
        gQCD += 12. * ( (7. / 8.) * 3. ); // 3 gens of quarks approximated as massless
    }

 
    if (T > 0.15 && T < 0.25){
        double fit = 3.21029 + (91.7341 - 184.36 * T) / ( 1. + exp(100. * (0.2-T) ) );
        if (T < connection.Model.Cosmology.Temperatures.Hadronization){
            gQCD = max(fit, gQCD);
        } else{
            gQCD = min(fit, gQCD);
        }
    }


    // photon
    gstr += 2.;

    // neutrinos
    if (T > connection.Model.Cosmology.Temperatures.NeutrinoDecouple){
        gstr += (7. / 8.) * 6.; // neutrinos are still coupled
    } else{
        gstr += (7. / 8.) * 6. * pow(4./11., 4./3.); // decoupled neutrinos
    }

    gstr += gQCD;
*/

    if (std::isnan(gstr)){
        connection.Log.Warn("Invalid g_{*} calculated at T=" + to_string(T) + " GeV... Setting to maximal g_{*} = 225");
        gstr = 225.;
    }

    connection.Log.Trace("Calculated g_{*}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}

double GStar::CalculateEntropic(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, Connection& connection, double T){
    double gstr = Calculate( particles, connection, T );
    if (T <= connection.Model.Cosmology.Temperatures.NeutrinoDecouple){
        return ( gstr - (7. / 8.) * 6. * ( pow(4. / 11., 4. / 3.) - (4. / 11.) ) );
    }
    connection.Log.Trace("Calculated g_{*S}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}

double GStar::CalculateEntropic(const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles, Connection& connection, double T, double gstr){
    if (T <= connection.Model.Cosmology.Temperatures.NeutrinoDecouple){
        return ( gstr - (7. / 8.) * 6. * ( pow(4. / 11., 4. / 3.) - (4. / 11.) ) );
    }
    connection.Log.Trace("Calculated g_{*S}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}
