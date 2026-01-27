#include <ModelIndependent/GStar.h>

using namespace std;

GStar::GStar(/* args */){}
GStar::~GStar(){}

double GStar::integralEval(
    double numFactor, 
    double denomFactor, 
    double t0, double tf, double dt
){
    using namespace boost::math::quadrature;

    // avoid t = 0 singularity
    if (t0 == 0.0)
        t0 = std::numeric_limits<double>::min();

    // define the integrand
    auto integrand = [numFactor, denomFactor](double t) -> double {
        double t2 = t * t;
        double nf2 = numFactor * numFactor;

        if (t2 * nf2 > 1.0)
            return 0.0;

        double sqrt_term = std::sqrt(1.0 - t2 * nf2);
        double denom = std::pow(t, 5.0) * (std::exp(1.0 / t) + denomFactor);
        return sqrt_term / denom;
    };

    // perform integration using adaptive Gauss–Kronrod
    // we integrate the *derivative*, not a differential equation, so this is direct quadrature
    double result = 0.0;
    try {
        // n = 15 gives good performance–accuracy tradeoff
        result = gauss_kronrod<double, 15>::integrate(integrand, t0, tf, 5, 1e-8);
    } catch (const std::exception &e) {
        std::cerr << "Integration failed: " << e.what() << std::endl;
        return 0.0;
    }

    // multiply by the normalization factor
    return result * 0.15399;
}

double GStar::defaultIntegralEval(const double& numFactor, const double& denomFactor){
    return integralEval(numFactor, denomFactor, 0., 1. / numFactor, 0.1);
}

Models::Particle& GStar::findParticle(
    const std::vector<Models::Particle>& particles,
    const std::string& key)
{

    for (auto& p : particles)
        if (p.Key == key)
            return const_cast<Models::Particle&>(p);

    throw_with_trace(logic_error("Could not find particle with key: " + key));
}

double GStar::calculateQCD(
    const std::vector< Models::Particle >& particles, 
    const double& T, 
    const CosmologicalTemperatures& temperatures
){
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

double GStar::calculateGaugeBosons(
    const std::vector< Models::Particle >& particles, 
    const double& T, 
    const CosmologicalTemperatures& temperatures
){
    auto wBoson = findParticle(particles, "wboson");
    auto zBoson = findParticle(particles, "zboson");
    auto gW = GStar::defaultIntegralEval(abs(wBoson.Mass) / T, -1. );
    auto gZ = GStar::defaultIntegralEval(abs(zBoson.Mass) / T, -1. );
    auto gPhoton = 2.;

    double gGauges = 6. * gW + 3. * gZ + gPhoton;
    return gGauges;
}

double GStar::calculateLeptons(
    const std::vector< Models::Particle >& particles, 
    const double& T, 
    const CosmologicalTemperatures& temperatures
){
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

double GStar::calculateSusy(
    const std::vector< Models::Particle >& particles, 
    const double& T, 
    const CosmologicalTemperatures& temperatures
){
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


double GStar::Calculate(
    const ModelBase& model, 
    double T
){
    auto susy = GStar::calculateSusy(model.Particles, T, model.Cosmology.Temperatures);
    auto leptons = GStar::calculateLeptons(model.Particles, T, model.Cosmology.Temperatures);
    auto qcd = GStar::calculateQCD(model.Particles, T, model.Cosmology.Temperatures);
    auto gb = GStar::calculateGaugeBosons(model.Particles, T, model.Cosmology.Temperatures);
    double gstr = (susy + leptons + qcd + gb);

    return gstr;
}

double GStar::CalculateEntropic(
    const ModelBase& model, 
    double T
){
    double gstr = Calculate(model, T);
    if (T <= model.Cosmology.Temperatures.NeutrinoDecouple){
        return ( gstr - (7. / 8.) * 6. * ( pow(4. / 11., 4. / 3.) - (4. / 11.) ) );
    }
    return gstr;
}

double GStar::Calculate(
    Connection& connection, 
    double T
){
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

    auto particles = cb.CallbackReturn.Particles;

    auto susy = GStar::calculateSusy(particles, T, connection.Model.Cosmology.Temperatures);
    auto leptons = GStar::calculateLeptons(particles, T, connection.Model.Cosmology.Temperatures);
    auto qcd = GStar::calculateQCD(particles, T, connection.Model.Cosmology.Temperatures);
    auto gb = GStar::calculateGaugeBosons(particles, T, connection.Model.Cosmology.Temperatures);
    double gstr = (susy + leptons + qcd + gb);

    connection.Log.Trace("Calculated g_{*}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}

double GStar::CalculateEntropic(
    Connection& connection, 
    double T
){
    double gstr = Calculate(connection, T);
    if (T <= connection.Model.Cosmology.Temperatures.NeutrinoDecouple){
        return ( gstr - (7. / 8.) * 6. * ( pow(4. / 11., 4. / 3.) - (4. / 11.) ) );
    }
    connection.Log.Trace("Calculated g_{*S}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}

double GStar::Calculate(
    DbManager& db, 
    Connection& connection, 
    double T
){
    Models::Particle p;
    auto statement = Statements::Particle(p, Statements::Read);
    auto filter = Filters::Particle( connection.InputId, Filters::WhereUUID::InputId );
    statement.AddFilter( filter );
    auto cb = Callbacks::Particle();
    db.Execute(statement, cb.Callback, cb.CallbackReturn);

    if (cb.CallbackReturn.Particles.size() == 0){
        throw_with_trace( logic_error("Could not find particles!") );
    }

    auto particles = cb.CallbackReturn.Particles;

    auto susy = GStar::calculateSusy(particles, T, connection.Model.Cosmology.Temperatures);
    auto leptons = GStar::calculateLeptons(particles, T, connection.Model.Cosmology.Temperatures);
    auto qcd = GStar::calculateQCD(particles, T, connection.Model.Cosmology.Temperatures);
    auto gb = GStar::calculateGaugeBosons(particles, T, connection.Model.Cosmology.Temperatures);
    double gstr = (susy + leptons + qcd + gb);

    connection.Log.Trace("Calculated g_{*}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}

double GStar::CalculateEntropic(
    DbManager& db, 
    Connection& connection, 
    double T, 
    double gstr
){
    if (T <= connection.Model.Cosmology.Temperatures.NeutrinoDecouple){
        return ( gstr - (7. / 8.) * 6. * ( pow(4. / 11., 4. / 3.) - (4. / 11.) ) );
    }
    connection.Log.Trace("Calculated g_{*S}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}


double GStar::Calculate(
    const std::vector< Models::Particle >& particles, 
    Connection& connection, 
    double T
){
    auto susy = GStar::calculateSusy(particles, T, connection.Model.Cosmology.Temperatures);
    auto leptons = GStar::calculateLeptons(particles, T, connection.Model.Cosmology.Temperatures);
    auto qcd = GStar::calculateQCD(particles, T, connection.Model.Cosmology.Temperatures);
    auto gb = GStar::calculateGaugeBosons(particles, T, connection.Model.Cosmology.Temperatures);
    double gstr = (susy + leptons + qcd + gb);

    if (std::isnan(gstr)){
        connection.Log.Debug("Invalid g_{*} calculated at T=" + to_string(T) + " GeV... Setting to maximal g_{*} = 225");
        gstr = 225.;
    }

    connection.Log.Trace("Calculated g_{*}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}

double GStar::CalculateEntropic(
    const std::vector< Models::Particle >& particles, 
    Connection& connection, 
    double T
){
    double gstr = Calculate( particles, connection, T );
    if (T <= connection.Model.Cosmology.Temperatures.NeutrinoDecouple){
        return ( gstr - (7. / 8.) * 6. * ( pow(4. / 11., 4. / 3.) - (4. / 11.) ) );
    }
    connection.Log.Trace("Calculated g_{*S}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}

double GStar::CalculateEntropic(
    const std::vector< Models::Particle >& particles, 
    Connection& connection, 
    double T, 
    double gstr
){
    if (T <= connection.Model.Cosmology.Temperatures.NeutrinoDecouple){
        return ( gstr - (7. / 8.) * 6. * ( pow(4. / 11., 4. / 3.) - (4. / 11.) ) );
    }
    connection.Log.Trace("Calculated g_{*S}=" + to_string(gstr) + " at T=" + to_string(T) + " GeV");
    return gstr;
}
