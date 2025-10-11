#include <ParticleDefs/Neutralino/NeutralinoWidths.h>

using namespace std;

NeutralinoWidths::NeutralinoWidths(const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles) 
    : model_(model),
    particles_(particles)
{
    auto axino = ModelBaseOps::FindConst(particles_, "axino");
    auto z = ModelBaseOps::FindConst(particles_, "zboson");
    auto neutralino1 = ModelBaseOps::FindConst(particles_, "neutralino1");

    auto widthAxinoDiElectron = widthNeutralinoAxinoMassiveDiLepton( neutralino1, axino, ModelBaseOps::FindConst(particles_, "electron"), z );
    auto widthAxinoDiMuon = widthNeutralinoAxinoMassiveDiLepton( neutralino1, axino, ModelBaseOps::FindConst(particles_, "muon"), z );
    auto widthAxinoDiTau = widthNeutralinoAxinoMassiveDiLepton( neutralino1, axino, ModelBaseOps::FindConst(particles_, "tau"), z );
    auto widthAxinoDiElectronNeutrino = widthNeutralinoAxinoMasslessDiLepton( neutralino1, axino, ModelBaseOps::FindConst(particles_, "neutrinoelectron"), z);
    auto widthAxinoDiMuonNeutrino = widthNeutralinoAxinoMasslessDiLepton( neutralino1, axino, ModelBaseOps::FindConst(particles_, "neutrinomuon"), z );
    auto widthAxinoDiTauNeutrino = widthNeutralinoAxinoMasslessDiLepton( neutralino1, axino, ModelBaseOps::FindConst(particles_, "neutrinotau"), z );
    auto widthAxinoDiUp = widthNeutralinoAxinoUpQuark( neutralino1, axino, ModelBaseOps::FindConst(particles_, "upq"), z );
    auto widthAxinoDiCharm = widthNeutralinoAxinoUpQuark( neutralino1, axino, ModelBaseOps::FindConst(particles_, "charmq"), z );
    auto widthAxinoDiDown = widthNeutralinoAxinoDownQuark( neutralino1, axino, ModelBaseOps::FindConst(particles_, "downq"), z );
    auto widthAxinoDiStrange = widthNeutralinoAxinoDownQuark( neutralino1, axino, ModelBaseOps::FindConst(particles_, "strangeq"), z );
    auto widthAxinoDiBottom = widthNeutralinoAxinoDownQuark( neutralino1, axino, ModelBaseOps::FindConst(particles_, "bottomq"), z );
    auto widthAxinoPhoton = widthNeutralinoAxinoPhoton( neutralino1, axino, ModelBaseOps::FindConst(particles_, "photon") );
    auto widthAxinoZ = widthNeutralinoAxinoZ( neutralino1, axino, z);
    // TODO:  NO DI-TOP QUARK DECAY CHANNEL??????



    auto upl= ModelBaseOps::FindConst(particles_, "suplsq");
    auto el = ModelBaseOps::FindConst(particles_, "electron");
    auto upq = ModelBaseOps::FindConst(particles_, "upq");
    auto downq = ModelBaseOps::FindConst(particles_, "downq");
    double alpha = 1.;
    double lambda = model_.Options.NonUniversalSugra.LambdaRPV;//1.e-7;
    double widthRPVsketch = (3./128.)*alpha*pow( lambda / M_PI, 2.)*pow( abs( neutralino1.Mass ), 5.) / pow( upl.Mass, 4. );
    auto widthRPV = Models::PartialWidth( neutralino1, { el, upq, downq }, widthRPVsketch );


    Decays.push_back( widthRPV );


    Decays.push_back( widthAxinoDiElectron );
    Decays.push_back( widthAxinoDiMuon );
    Decays.push_back( widthAxinoDiTau );
    Decays.push_back( widthAxinoDiElectronNeutrino );
    Decays.push_back( widthAxinoDiMuonNeutrino );
    Decays.push_back( widthAxinoDiTauNeutrino );
    Decays.push_back( widthAxinoDiUp );
    Decays.push_back( widthAxinoDiCharm );
    Decays.push_back( widthAxinoDiDown );
    Decays.push_back( widthAxinoDiStrange );
    Decays.push_back( widthAxinoDiBottom );
    Decays.push_back( widthAxinoPhoton );
    Decays.push_back( widthAxinoZ );
    //  Decays.insert(Decays.end(), lightSquarks.begin(), lightSquarks.end());
}
NeutralinoWidths::~NeutralinoWidths(){}

Models::PartialWidth NeutralinoWidths::widthNeutralinoAxinoZ(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& zBoson){
    // TODO: find out why/what/a better way
    double Gz = model_.StandardModel.Couplings.alphaEM / ( 16. * M_PI * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) * ( 8. / 3. ) * model_.Susy.MixingAngles.zMix[0][3] * sqrt( model_.StandardModel.MixingAngles.SinSqrThetaW ) / model_.PQSector.Fa;
    double width = 0.;
    if (abs( neutralino1.Mass ) > (zBoson.Mass + axino.Mass) ){
        width = pow( Gz, 2. ) * ( axino.Mass + abs(neutralino1.Mass) - zBoson.Mass ) 
        * ( axino.Mass + abs(neutralino1.Mass) + zBoson.Mass ) 
        * sqrt( ( -axino.Mass + abs(neutralino1.Mass) - zBoson.Mass ) * ( axino.Mass + abs(neutralino1.Mass) - zBoson.Mass ) * ( -axino.Mass + abs(neutralino1.Mass) + zBoson.Mass ) * ( axino.Mass + abs(neutralino1.Mass) + zBoson.Mass ) )
        * ( 2. * pow( axino.Mass - abs( neutralino1.Mass ), 2. ) + pow( zBoson.Mass, 2. ) ) / ( M_PI * pow( abs(neutralino1.Mass), 3. ) );
    }
    return Models::PartialWidth( neutralino1, { axino, zBoson }, width );
}

Models::PartialWidth NeutralinoWidths::widthNeutralinoAxinoPhoton(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& photon){
    // TODO: find out why/what/a better way
    double Gg = model_.StandardModel.Couplings.alphaEM / ( 16. * M_PI * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) * ( 8. / 3. ) * model_.Susy.MixingAngles.zMix[0][3] * sqrt( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) / model_.PQSector.Fa;
    double width = 0.;
        if ( abs( neutralino1.Mass ) > axino.Mass){
        width = 2. * pow( Gg, 2. ) * pow( 
            pow( abs(neutralino1.Mass), 2. ) 
            - 
            pow( axino.Mass, 2. ), 3. 
        ) / ( M_PI * pow( abs(neutralino1.Mass), 3. ) );
        } 
    return Models::PartialWidth( neutralino1, { axino, photon }, width );
}

Models::PartialWidth NeutralinoWidths::widthNeutralinoAxinoUpQuark(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& quark, const Models::Particle& zBoson){
    double width = 0.;
    if ( abs( neutralino1.Mass ) > ( axino.Mass + quark.Mass ) ){
        auto elChrge = find_if(quark.Charges.begin(), quark.Charges.end(), [&] (Models::Charge const& c){ return c.Group == Models::GaugeGroup::Electromagnetic; } );

        // TODO: find out why/what/a better way
        double sgz = sqrt( 4. * M_PI * model_.StandardModel.Couplings.alphaEM / ( model_.StandardModel.MixingAngles.SinSqrThetaW * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) );
        double gv = 1./4. - ( 2. * model_.StandardModel.MixingAngles.SinSqrThetaW / 3. );
        double ga = -1./4.;
        double Gz = model_.StandardModel.Couplings.alphaEM / ( 16. * M_PI * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) * ( 8. / 3. ) * model_.Susy.MixingAngles.zMix[0][3] * sqrt( model_.StandardModel.MixingAngles.SinSqrThetaW ) / model_.PQSector.Fa;
        double Gg = model_.StandardModel.Couplings.alphaEM / ( 16. * M_PI * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) * ( 8. / 3. ) * model_.Susy.MixingAngles.zMix[0][3] * sqrt( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) / model_.PQSector.Fa;
        width = NeutralinoWidths::phaseSpaceIntegral(axino.Mass, neutralino1.Mass, quark.Mass, zBoson.Mass, zBoson.Width, sgz, gv, ga, Gz, Gg, ( (*elChrge).Value * sqrt( 4. * M_PI * model_.StandardModel.Couplings.alphaEM) ) );
    }
    // adhoc multiply by 3 to get each color (TODO: review!)
    return Models::PartialWidth( neutralino1, { axino, quark, quark }, 3. * width );
}

Models::PartialWidth NeutralinoWidths::widthNeutralinoAxinoDownQuark(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& quark, const Models::Particle& zBoson){
    double width = 0.;
    if ( abs( neutralino1.Mass ) > ( axino.Mass + quark.Mass ) ){
        auto elChrge = find_if(quark.Charges.begin(), quark.Charges.end(), [&] (Models::Charge const& c){ return c.Group == Models::GaugeGroup::Electromagnetic; } );

        // TODO: find out why/what/a better way
        double sgz = sqrt( 4. * M_PI * model_.StandardModel.Couplings.alphaEM / ( model_.StandardModel.MixingAngles.SinSqrThetaW * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) );
        double gv = -1./4. + ( model_.StandardModel.MixingAngles.SinSqrThetaW / 3. );
        double ga = 1./4.;
        double Gz = model_.StandardModel.Couplings.alphaEM / ( 16. * M_PI * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) * ( 8. / 3. ) * model_.Susy.MixingAngles.zMix[0][3] * sqrt( model_.StandardModel.MixingAngles.SinSqrThetaW ) / model_.PQSector.Fa;
        double Gg = model_.StandardModel.Couplings.alphaEM / ( 16. * M_PI * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) * ( 8. / 3. ) * model_.Susy.MixingAngles.zMix[0][3] * sqrt( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) / model_.PQSector.Fa;
        width = NeutralinoWidths::phaseSpaceIntegral(axino.Mass, neutralino1.Mass, quark.Mass, zBoson.Mass, zBoson.Width, sgz, gv, ga, Gz, Gg, ( (*elChrge).Value * sqrt( 4. * M_PI * model_.StandardModel.Couplings.alphaEM) ) );
    }
    // adhoc multiply by 3 to get each color (TODO: review!)
    return Models::PartialWidth( neutralino1, { axino, quark, quark }, 3. * width );
}

Models::PartialWidth NeutralinoWidths::widthNeutralinoAxinoMasslessDiLepton(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& lepton, const Models::Particle& zBoson){
    double width = 0.;
    if ( abs( neutralino1.Mass ) > ( axino.Mass + 2. * lepton.Mass ) ){
        auto elChrge = find_if(lepton.Charges.begin(), lepton.Charges.end(), [&] (Models::Charge const& c){ return c.Group == Models::GaugeGroup::Electromagnetic; } );

        // TODO: find out why/what/a better way
        double sgz = sqrt( 4. * M_PI * model_.StandardModel.Couplings.alphaEM / ( model_.StandardModel.MixingAngles.SinSqrThetaW * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) );
        double gv = 1./4.;
        double ga = -1./4.;
        double Gz = model_.StandardModel.Couplings.alphaEM / ( 16. * M_PI * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) * ( 8. / 3. ) * model_.Susy.MixingAngles.zMix[0][3] * sqrt( model_.StandardModel.MixingAngles.SinSqrThetaW ) / model_.PQSector.Fa;
        double Gg = model_.StandardModel.Couplings.alphaEM / ( 16. * M_PI * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) * ( 8. / 3. ) * model_.Susy.MixingAngles.zMix[0][3] * sqrt( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) / model_.PQSector.Fa;
        width = NeutralinoWidths::phaseSpaceIntegral(axino.Mass, neutralino1.Mass, lepton.Mass, zBoson.Mass, zBoson.Width, sgz, gv, ga, Gz, Gg, ( (*elChrge).Value * sqrt( 4. * M_PI * model_.StandardModel.Couplings.alphaEM) ) );
    }
    return Models::PartialWidth( neutralino1, { axino, lepton, lepton }, width );
}

Models::PartialWidth NeutralinoWidths::widthNeutralinoAxinoMassiveDiLepton(const Models::Particle& neutralino1, const Models::Particle& axino, const Models::Particle& lepton, const Models::Particle& zBoson){
    double width = 0.;
    if ( abs( neutralino1.Mass ) > ( axino.Mass + 2. * lepton.Mass ) ){   
        auto elChrge = find_if(lepton.Charges.begin(), lepton.Charges.end(), [&] (Models::Charge const& c){ return c.Group == Models::GaugeGroup::Electromagnetic; } );

        // TODO: find out why/what/a better way
        double sgz = sqrt( 4. * M_PI * model_.StandardModel.Couplings.alphaEM / ( model_.StandardModel.MixingAngles.SinSqrThetaW * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) );
        double gv = -1./4. + model_.StandardModel.MixingAngles.SinSqrThetaW;
        double ga = 1./4.;
        double Gz = model_.StandardModel.Couplings.alphaEM / ( 16. * M_PI * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) * ( 8. / 3. ) * model_.Susy.MixingAngles.zMix[0][3] * sqrt( model_.StandardModel.MixingAngles.SinSqrThetaW ) / model_.PQSector.Fa;
        double Gg = model_.StandardModel.Couplings.alphaEM / ( 16. * M_PI * ( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) ) * ( 8. / 3. ) * model_.Susy.MixingAngles.zMix[0][3] * sqrt( 1. - model_.StandardModel.MixingAngles.SinSqrThetaW ) / model_.PQSector.Fa;
        width = NeutralinoWidths::phaseSpaceIntegral(axino.Mass, neutralino1.Mass, lepton.Mass, zBoson.Mass, zBoson.Width, sgz, gv, ga, Gz, Gg, ( (*elChrge).Value * sqrt( 4. * M_PI * model_.StandardModel.Couplings.alphaEM) ) );
    }
    return Models::PartialWidth( neutralino1, { axino, lepton, lepton }, width );
}

double NeutralinoWidths::phaseSpaceIntegral(double mAxino, double mNeutralino, double mFermion, double mZ, double GammaZ, double sgz, double gv, double ga, double Gz, double Gg, double Qe){
    state_type x0(1);
    x0[0] = 0.;

    // because legacy code was terrible, that's why
    mNeutralino = abs(mNeutralino);

    double t0 = 4. * pow( mFermion, 2. ) / pow( mNeutralino, 2. ); 
    double tf = 1. - pow( mAxino, 2. ) / pow( mNeutralino, 2. ) - 2. * mFermion * abs(mAxino) / pow( mNeutralino, 2. );

    // TODO: make better approximation for dt
    double dt = (tf - t0) / 1000;

    // use a lambda expression to insert the integral needed into boost's integrate function
    auto result = boost::numeric::odeint::integrate_adaptive(
        boost::numeric::odeint::make_controlled<error_stepper_type> (1.0e-10, 1.0e-6),
        [&](const state_type &x, state_type &dxdt, double t){
            dxdt[0] = gam0(t, mAxino, mNeutralino, mFermion, mZ, GammaZ, sgz, gv, ga, Gz, Gg, Qe);
        },
        x0, t0, tf, dt
    );
    return x0[0];
}

double NeutralinoWidths::gam0(double x1, double mAxino, double mNeutralino, double mFermion, double mZ, double GammaZ, double sgz, double gv, double ga, double Gz, double Gg, double Qe){
    double mua = pow(mAxino, 2.) / pow(mNeutralino, 2.);
    double muq = pow(mFermion, 2.) / pow(mNeutralino, 2.);
    double L1 = (
        x1 * ( -1. + mua + x1) 
        + 
        2. * muq * ( -3. + mua + 2. * x1) 
        - 
        sqrt( -4. * muq + pow(x1, 2.) ) 
        * 
        sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) )
    ) / ( -1. + muq + x1 );
    double L2 = ( 
        x1 * ( -1. + mua + x1 ) 
        + 
        2. * muq * ( -3. + mua + 2. * x1) 
        + 
        sqrt( -4. * muq + pow(x1, 2.) ) 
        * 
        sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1) )
    ) / ( -1. + muq + x1 );
    double L3 = (
        2. * pow(GammaZ, 2.) * pow(mZ, 2.) 
        + 
        2. * pow(mZ, 4.) * pow(-1. + x1, 2.) 
        + 
        pow(mNeutralino, 4.) * x1 * ( -1. + mua + x1 ) * ( 
        x1 * ( -1. + mua + x1 )
        - 
        sqrt( -4. * muq + pow(x1, 2.) ) * sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) )
        )
        + 
        2. * pow(muq, 2.) * ( 
        pow(GammaZ, 2.) * pow(mZ, 2.) 
        +
        pow(mZ, 4.) 
        - 
        2. * pow(mNeutralino, 2.) * pow(mZ, 2.) * ( -3. + mua + 2. * x1) 
        +
        pow(mNeutralino, 4.) * (
            pow(mua, 2.) + pow(3. - 2. * x1, 2.) + mua * ( -2. + 4. * x1)
        )
        ) 
        +
        2. * pow(mZ, 2.) * ( 
        pow(GammaZ, 2.) * ( -2. + x1 ) * x1 
        +
        pow(mNeutralino, 2.) * ( -1. + x1 ) * (
            -1. * x1 * ( -1. + mua + x1 )
            +
            sqrt( -4. * muq + pow(x1, 2.) ) 
            *
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1) )
        )
        )
        -
        2. * muq * (
        -2. * pow(mZ, 2.) * ( pow(GammaZ, 2.) + pow(mZ, 2.) ) * (-1. + x1) 
        +
        pow(mNeutralino, 2.) * pow(mZ, 2.) * (
            6. + mua * ( -2. + 3. * x1 ) + x1 * ( -11. + 5. * x1 ) 
            -
            sqrt( -4. * muq + pow(x1, 2.) ) 
            * 
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1) )
        ) 
        +
        pow(mNeutralino, 4.) * (
            pow(-1. + mua, 2.) 
            - 
            ( -5. + mua ) * ( -1. + mua ) * x1 
            - 
            2. * ( -3. + mua ) * pow(x1, 2.) 
            - 
            2. * pow(x1, 3.) 
            -
            3. * sqrt( -4. * muq + pow(x1, 2.) ) 
            * 
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) ) 
            +
            mua * sqrt( -4. * muq + pow(x1, 2.) ) * sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) ) 
            + 
            2. * x1 * sqrt( -4. * muq + pow(x1, 2.) ) * sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) ) 
        )
        )
    ) / pow(-1. + muq + x1, 2.);

    double L4 = (
        2. * pow(GammaZ, 2.) * pow(mZ, 2.) 
        +
        2. * pow(mZ, 4.) * pow(-1. + x1, 2.) 
        +
        pow(mNeutralino, 4.) * x1 * ( -1. + mua + x1 ) * (
        x1 * ( -1. + mua + x1) 
        +
        sqrt( -4. * muq + pow(x1, 2.) ) 
        * 
        sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) )
        ) 
        + 
        2. * pow(muq, 2.) * (
        pow(GammaZ, 2.) * pow(mZ, 2.) 
        + 
        pow(mZ, 4.) 
        -
        2. * pow(mNeutralino, 2.) * pow(mZ, 2.) * ( -3. + mua + 2. * x1 ) 
        + 
        pow(mNeutralino, 4.) * (
            pow(mua, 2.) 
            + 
            pow(3. - 2. * x1, 2.) 
            + 
            mua * ( -2. + 4. * x1)
        )
        ) 
        +
        2. * pow(mZ, 2.) * (
        pow(GammaZ, 2.) * ( -2. + x1 ) * x1 
        -
        pow(mNeutralino, 2.) * ( -1. + x1 ) * (
            x1 * ( -1. + mua + x1 ) 
            +
            sqrt( -4. * muq + pow(x1, 2.) )
            *
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) )
        )
        )
        +
        2. * muq * ( 
        2. * pow(mZ, 2.) * ( pow(GammaZ, 2.) + pow(mZ, 2.) ) * ( -1. + x1 )
        -
        pow(mNeutralino, 2.) * pow(mZ, 2.) * (
            6. + mua * ( -2. + 3. * x1) 
            +
            x1 * ( -11. + 5. * x1) 
            + 
            sqrt( -4. * muq + pow(x1, 2.) ) 
            *
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) )
        ) 
        +
        pow(mNeutralino, 4.) * (
            -1. + pow(mua, 2.) * ( -1. + x1 ) 
            -
            3. * sqrt( -4. * muq + pow(x1, 2.) ) 
            *
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1) ) 
            +
            mua * (
            2. 
            + 
            2. * ( -3. + x1 ) * x1 
            + 
            sqrt( -4. * muq + pow(x1, 2.) ) 
            * 
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) )
            )
            +
            x1 * ( 
            5. 
            + 
            2. * ( -3. + x1 ) * x1 
            + 
            2. * sqrt( -4. * muq + pow(x1, 2.) ) 
            *
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1) )
            )
        )
        )
    ) / pow(-1. + muq + x1, 2.);


    double L7 = ( 
        2. * pow(GammaZ, 2.) * pow(mZ, 2.)
        +
        2. * pow(mZ, 4.) * pow(-1. + x1, 2.)
        +
        pow(mNeutralino, 4.) * x1 * (-1. + mua + x1) * (
        x1 * ( -1. + mua + x1 )
        -
        sqrt( -4. * muq + pow(x1, 2.) )
        *
        sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) )
        )
        +
        2. * pow(muq, 2.) * ( 
        pow(GammaZ, 2.) * pow(mZ, 2.)
        +
        pow(mZ, 4.)
        - 
        2. * pow(mNeutralino, 2.) * pow(mZ, 2.) * (-3. + mua + 2. * x1) 
        +
        pow(mNeutralino, 4.) * ( 
            pow(mua, 2.)
            + 
            pow(3. - 2. * x1, 2.) 
            + 
            mua * (-2. + 4. * x1) 
        )
        ) 
        + 
        2. * pow(mZ, 2.) * (
        pow(GammaZ, 2.) * (-2. + x1) * x1 
        + 
        pow(mNeutralino, 2.) * (-1. + x1) * (
            -1. * x1 * (-1. + mua + x1) 
            +
            sqrt( -4. * muq + pow(x1, 2.) ) 
            * 
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1) )
        )
        ) 
        -
        2. * muq * (
        -2. * pow(mZ, 2.) * ( pow(GammaZ, 2.) + pow(mZ, 2.) ) * (-1. + x1) 
        + 
        pow(mNeutralino, 2.) * pow(mZ, 2.) * (
            6.
            + 
            mua * (-2. + 3. * x1) 
            + 
            x1 * (-11. + 5. * x1) 
            - 
            sqrt( -4. * muq + pow(x1, 2.) ) 
            *
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) )
        )
        +
        pow(mNeutralino, 4.) * ( 
            pow(-1. + mua, 2.) 
            - 
            (-5. + mua) * (-1. + mua) * x1 
            - 
            2. * (-3. + mua) * pow(x1, 2.)
            - 
            2. * pow(x1, 3.) 
            - 
            3. * sqrt( -4. * muq + pow(x1, 2.) ) 
            *
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * ( -1. - 2. * muq + x1 ) )
            +
            mua * sqrt(-4. * muq + pow(x1, 2.) ) 
            *
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * (-1. - 2. * muq + x1) )
            +
            2. * x1 * sqrt(-4. * muq + pow(x1, 2.) )
            *
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * (-1. - 2. * muq + x1) )
        )
        )
    ) / pow(-1. + muq + x1, 2.);

    // TODO: Check everything and find a better way to do these!
    double L5 = L1;
    double L6 = L2;
    double L8 = L4;

    double dgam1 = gam1(x1, mAxino, mNeutralino, mFermion, Gg, Qe, L1, L2);
    double dgam2 = gam2(x1, mAxino, mNeutralino, mFermion, mZ, sgz, gv, ga, Gz, GammaZ, L3, L4);
    double dgam12 = gam12(x1, mAxino, mNeutralino, mFermion, mZ, sgz, gv, Gz, Gg, Qe, GammaZ, L5, L6, L7, L8);

    double dgam0 = ( dgam1 + dgam2 + (2. * dgam12) );
    if ( std::isnan(dgam0) ){
        return 0;
    }
    return dgam0;
}

double NeutralinoWidths::gam1(double x1, double mAxino, double mNeutralino, double mFermion, double Gg, double Qe, double L1, double L2){
    double mua = pow(mAxino, 2.) / pow(mNeutralino, 2.);
    double muq = pow(mFermion, 2.) / pow(mNeutralino, 2.);
    //gamma-gamma contribution:
    double gam1 = 0.004031441804 * pow(Gg, 2.) * pow(mNeutralino, 3.) * pow(Qe, 2.) * (
        2. * ( pow(-1. + mua, 2.) - 2. * pow(-1. + sqrt(mua ), 2.) * muq + 2. * (-1. + mua ) * x1 + 2. * pow(x1, 2.) ) * log(L1/L2) 
        + 
        2. * 
        ( 
            (
                sqrt(-4. * muq + pow(x1, 2.) ) * (
                    (-1. + mua ) * (
                        -2. * pow(mua, 1.5) * (1. + muq) 
                        + 
                        pow(mua, 2.) * (1. + muq) 
                        - 
                        2. * mua * (1. + muq) * (2. + muq) 
                        + 
                        (1. + muq) * (3. + 2. * muq) 
                        + 
                        2. * sqrt(mua ) * ( 1. + 9. * muq )
                    )
                    +
                    ( 
                    pow(-1. + mua, 2.) * ( 5. - 2. * sqrt(mua ) + mua )
                    + 
                    2. * ( 13. + 12. * sqrt(mua) - 14. * mua - 4. * pow(mua, 1.5) + pow(mua, 2.) ) * muq
                    ) * x1
                    +
                    4. * ( 
                    1. 
                    - 
                    2. * pow(mua, 1.5) 
                    + 
                    pow(mua, 2.) 
                    - 
                    2. * sqrt(mua ) * (-1. + muq) 
                    - 
                    7. * muq 
                    + 
                    mua * (-2. + 3. * muq) 
                    ) * pow(x1, 2.) 
                    + 
                    2. * ( -5. - 2. * sqrt(mua) + 5. * mua + 4. * muq ) * pow(x1, 3.) 
                    + 
                    4. * pow(x1, 4.) 
                ) * sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * (-1. - 2. * muq + x1) )
            ) / (
                (-1. + muq + x1) 
                * ( 
                    (-1. + mua ) * (-1. + mua + (-9. + mua ) * muq) + ( pow(-1. + mua, 2.) + 4. * (-3. + mua ) * muq) * x1
                    + 4. * (-1. + mua + muq) * pow(x1, 2.) + 2. * pow(x1, 3.) 
                ) 
            ) 
        ) 
    );
    return gam1;
}

double NeutralinoWidths::gam2(double x1, double mAxino, double mNeutralino, double mFermion, double mZ, double sgz, double gv, double ga, double Gz, double GammaZ, double L3, double L4){
    double mua = pow(mAxino, 2.) / pow(mNeutralino, 2.);
    double muq = pow(mFermion, 2.) / pow(mNeutralino, 2.);
    //Z-Z contribution:
    double gam2 = 0.004031441804 * pow(sgz, 2.) * pow(Gz, 2.) * mNeutralino *
    (
        (
            -2. * pow(mNeutralino, 2.) * sqrt( -4. * muq + pow(x1, 2.) ) 
            * 
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * (-1. - 2. * muq + x1) ) 
            * (
            pow(ga, 2.) * (1. + 2. * sqrt( mua ) - mua + 2. * muq - 2. * x1) 
            - 
            pow(gv, 2.) * (-1. - 2. * sqrt( mua ) + mua + 2. * x1) 
            )   
        ) / (-1. + muq + x1) 
        + 
        ( 
            2. * (
            pow(ga, 2.) * (
                2. * muq * ( 
                pow(-1. + mua, 2.) * pow(mNeutralino, 4.) 
                + 
                ( pow(GammaZ, 2.) + 4. * sqrt( mua ) * pow(mNeutralino, 2.) ) * pow(mZ, 2.) 
                - 
                pow(mZ, 4.) 
                )
                +
                pow(mZ, 2.) * (
                -1. * pow(GammaZ, 2.) * (-1. - 2. * sqrt( mua ) + mua + 2. * x1) 
                + 
                pow(mZ, 2.) * (-1. - 2. * sqrt( mua ) + mua + 2. * x1) 
                - 
                pow(mNeutralino, 2.) * ( pow(-1. + mua, 2.) + 2. * (-1. + mua ) * x1 + 2. * pow(x1, 2.) ) 
                )
            )
            - 
            pow(gv, 2.) * (
                2. * pow(-1. + mua, 2.) * muq * pow(mNeutralino, 4.) 
                - 
                2. * pow(-1. + sqrt( mua ), 2.) * muq * pow(mNeutralino, 2.) * pow(mZ, 2.) 
                + 
                pow(mZ, 2.) * (
                pow(GammaZ, 2.) * (-1. - 2. * sqrt( mua ) + mua + 2. * x1) 
                - 
                pow(mZ, 2.) * (-1. - 2. * sqrt( mua ) + mua + 2. * x1)
                + 
                pow(mNeutralino, 2.) * ( pow(-1. + mua, 2.) + 2. * (-1. + mua ) * x1 + 2. * pow(x1, 2.) )
                )
            )
            )
            * 
            arccot(
            ( 
                2. * GammaZ * mZ * (-1. + muq + x1) 
            ) / (
                2. * pow(mZ, 2.) * (-1. + x1) 
                - 
                pow(mNeutralino, 2.) * x1 * (-1. + mua + x1) 
                + 
                pow(mNeutralino, 2.) * sqrt( -4. * muq + pow(x1, 2.) ) 
                *
                sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * (-1. - 2. * muq + x1) )
                + 
                2. * muq * ( pow(mZ, 2.) - pow(mNeutralino, 2.) * (-3. + mua + 2. * x1) )
            )
            )
            + 
            2. * (
            pow(gv, 2.) * ( 
                2. * pow(-1. + mua, 2.) * muq * pow(mNeutralino, 4.) 
                - 
                2. * pow(-1. + sqrt( mua ), 2.) * muq * pow(mNeutralino, 2.) * pow(mZ, 2.) 
                +
                pow(mZ, 2.) * (
                pow(GammaZ, 2.) * (-1. - 2. * sqrt( mua ) + mua + 2. * x1) 
                - 
                pow(mZ, 2.) * (-1. - 2. * sqrt( mua ) + mua + 2. * x1) 
                + 
                pow(mNeutralino, 2.) * ( pow(-1. + mua, 2.) + 2. * (-1. + mua ) * x1 + 2. * pow(x1, 2.) )
                )
            )
            + 
            pow(ga, 2.) * (
                2. * muq * ( 
                -1. * pow(-1. + mua, 2.) * pow(mNeutralino, 4.) 
                - 
                ( pow(GammaZ, 2.) + 4. * sqrt( mua ) * pow(mNeutralino, 2.) ) * pow(mZ, 2.) 
                + 
                pow(mZ, 4.) 
                ) 
                + 
                pow(mZ, 2.) * (
                pow(GammaZ, 2.) * (-1. - 2. * sqrt( mua ) + mua + 2. * x1) 
                - 
                pow(mZ, 2.) * (-1. - 2. * sqrt( mua ) + mua + 2. * x1) 
                + 
                pow(mNeutralino, 2.) * (pow(-1. + mua, 2.) + 2. * (-1. + mua ) * x1 + 2. * pow(x1, 2.) )
                )
            )
            )
            *
            arccot(
                ( 
                    2. * GammaZ * mZ * (-1. + muq + x1) 
                ) / (
                    2. * pow(mZ, 2.) * (-1. + x1) 
                    + 
                    2. * muq * ( pow(mZ, 2.) - pow(mNeutralino, 2.) * (-3. + mua + 2. * x1) ) 
                    - 
                    pow(mNeutralino, 2.) * (
                    x1 * (-1. + mua + x1) 
                    + 
                    sqrt( -4. * muq + pow(x1, 2.) ) 
                    * 
                    sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * (-1. - 2. * muq + x1) )
                    )
                )
            )
            +
            GammaZ * mZ * (
                pow(ga, 2.) * (
                    -2. * pow(mZ, 2.) * ( -1. - 2. * sqrt( mua ) + mua - 2. * muq + 2. * x1 ) 
                    +
                    pow(mNeutralino, 2.) * (
                    1. 
                    + 
                    pow(mua, 2.) 
                    - 
                    8. * sqrt( mua ) * muq 
                    + 
                    2. * mua * (-1. + x1) 
                    + 
                    2. * (-1. + x1) * x1 
                    )
                )
                + 
                pow(gv, 2.) * (
                    -2. * pow(mZ, 2.) * (-1. - 2. * sqrt( mua ) + mua + 2. * x1) 
                    +
                    pow(mNeutralino, 2.) * ( 
                    pow(-1. + mua, 2.) 
                    - 
                    2. * pow(-1. + sqrt( mua ), 2.) * muq 
                    + 
                    2. * (-1. + mua ) * x1 
                    + 
                    2. * pow(x1, 2.) 
                    )
                )
            ) * log(L3/L4)
        ) / (GammaZ * mZ)
    );
      return gam2;
}

double NeutralinoWidths::gam12(double x1, double mAxino, double mNeutralino, double mFermion, double mZ, double sgz, double gv, double Gz, double Gg, double Qe, double GammaZ, double L5, double L6, double L7, double L8){
    double mua = pow(mAxino, 2.) / pow(mNeutralino, 2.);
    double muq = pow(mFermion, 2.) / pow(mNeutralino, 2.);
    double gam12 = 0.002015720902 * gv * sgz * Gz * pow(mNeutralino, 5.) * Qe * Gg 
        * (
            ( 
            4. * ( -1. - 2. * sqrt(mua) + mua + 2. * x1 ) * sqrt( -4. * muq + pow(x1, 2.) )
            * 
            sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * (-1. - 2. * muq + x1) )
            ) / ( pow(mNeutralino, 2.) * (-1. + muq + x1) )
            + 
            (
            2. * (
                2. * GammaZ * ( 
                2. * pow(-1. + mua, 2.) * muq * pow(mNeutralino, 4.)
                + 
                pow(mZ, 2.) * ( pow(GammaZ, 2.) + pow(mZ, 2.) ) * ( -1. - 2. * sqrt(mua) + mua + 2. * x1 )
                )
                * 
                atan(
                ( 2. * GammaZ * mZ * (-1. + muq + x1) ) 
                / ( 
                    2. * pow(mZ, 2.) * (-1. + x1) 
                    - 
                    pow(mNeutralino, 2.) * x1 * (-1. + mua + x1) 
                    + 
                    pow(mNeutralino, 2.) * sqrt( -4. * muq + pow(x1, 2.) )
                    * 
                    sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * (-1. - 2. * muq + x1) )
                    + 
                    2. * muq * ( pow(mZ, 2.) - pow(mNeutralino, 2.) * (-3. + mua + 2. * x1) )
                )
                ) 
                + 
                2. * GammaZ * (
                -2. * pow(-1. + mua, 2.) * muq * pow(mNeutralino, 4.) 
                - 
                pow(mZ, 2.) * ( pow(GammaZ, 2.) + pow(mZ, 2.) ) * ( -1. - 2. * sqrt(mua) + mua + 2. * x1 )
                )
                * atan(
                ( 2. * GammaZ * mZ * (-1. + muq + x1) )
                / (
                    2. * pow(mZ, 2.) * (-1. + x1) 
                    + 
                    2. * muq * ( pow(mZ, 2.) - pow(mNeutralino, 2.) * (-3. + mua + 2. * x1) ) 
                    - 
                    pow(mNeutralino, 2.) * (
                    x1 * (-1. + mua + x1) 
                    +
                    sqrt( -4. * muq + pow(x1, 2.) ) * sqrt( pow(mua, 2.) + pow(-1. + x1, 2.) + 2. * mua * (-1. - 2. * muq + x1) )
                    )
                )
                )
                +
                mZ * ( 
                -4. * pow(-1. + mua, 2.) * muq * pow(mNeutralino, 4.) * log(L5/L6) 
                +
                (
                    2. * pow(-1. + mua, 2.) * muq * pow(mNeutralino, 4.) 
                    -
                    2. * pow(-1. + sqrt(mua), 2.) * muq * pow(mNeutralino, 2.) * pow(mZ, 2.) 
                    +
                    pow(mZ, 2.) * (
                    -1. * pow(mZ, 2.) * ( -1. - 2. * sqrt(mua) + mua + 2. * x1 )
                    +
                    pow(mNeutralino, 2.) * (
                        pow(-1. + mua, 2.)
                        + 
                        2. * (-1. + mua) * x1 
                        + 
                        2. * pow(x1, 2.) 
                    )
                    )
                    +
                    pow(GammaZ, 2.) * (
                    -1. * pow(mZ, 2.) * ( -1. - 2. * sqrt(mua) + mua + 2. * x1 ) 
                    +
                    pow(mNeutralino, 2.) * ( 
                        pow(-1. + mua, 2.) 
                        - 
                        2. * pow(-1. + sqrt(mua), 2.) * muq 
                        + 
                        2. * (-1. + mua) * x1 
                        +
                        2. * pow(x1, 2.)
                    )
                    )
                ) * log(L7/L8)
                )
            )
            ) / ( pow(mNeutralino, 4.) * mZ * ( pow(GammaZ, 2.) + pow(mZ, 2.) ) ) );
    return gam12;
}

double NeutralinoWidths::arccot(double arg){
    return atan(1. / arg);
}
