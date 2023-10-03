#include <ParticleDefs/Saxion/SaxionWidthsDFSZ.h>

using namespace std;

SaxionWidthsDFSZ::SaxionWidthsDFSZ(const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles) 
    : SaxionWidths(model, particles)
{
    _epsilonhl = ephl(2., model_.StandardModel.vEW);
    _epsilonHH = epHH(2., model_.StandardModel.vEW);

    auto fermionfermion = widthFermionFermion();
    auto vBosonvBoson = widthVBosonVBoson();
    auto vBosonHiggs = widthVBosonHiggs();
    auto higgsHiggs = widthDiHiggs(2.);
    auto charginoChargino = widthCharginoCharginoSameGen(2.);
    auto sneutrinoSneutrino = widthSneutrinoSneutrino();
    auto squarkSquark = widthSquarkSquark();
    auto sleptonSlepton = widthSleptonSlepton();
    auto neutralinoNeutralino = widthNeutralinoNeutralino(2.);
    Decays.insert(Decays.end(), fermionfermion.begin(), fermionfermion.end());
    Decays.insert(Decays.end(), vBosonvBoson.begin(), vBosonvBoson.end());
    Decays.insert(Decays.end(), vBosonHiggs.begin(), vBosonHiggs.end());
    Decays.insert(Decays.end(), higgsHiggs.begin(), higgsHiggs.end());
    Decays.insert(Decays.end(), charginoChargino.begin(), charginoChargino.end());
    Decays.insert(Decays.end(), sneutrinoSneutrino.begin(), sneutrinoSneutrino.end());
    Decays.insert(Decays.end(), squarkSquark.begin(), squarkSquark.end());
    Decays.insert(Decays.end(), sleptonSlepton.begin(), sleptonSlepton.end());
    Decays.insert(Decays.end(), neutralinoNeutralino.begin(), neutralinoNeutralino.end());
    Decays.push_back(widthCharginoCharginoDifferentGen(2.));
}

SaxionWidthsDFSZ::~SaxionWidthsDFSZ(){}

// from arXiv:1309.5365
std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> SaxionWidthsDFSZ::widthVBosonVBoson(){
    auto wBoson = ModelBaseOps::FindConst(particles_, "wboson");
    auto zBoson = ModelBaseOps::FindConst(particles_, "zboson");

    double alpha = model_.Susy.MixingAngles.alfah;
    double beta = atan(model_.Susy.TanBeta);
    // couplings from A.8
    double gsVV = _epsilonhl * sin(beta + alpha) + _epsilonHH * cos(beta + alpha);
    double ggZ = model_.StandardModel.Couplings.gSU2 / sqrt(1. - model_.StandardModel.MixingAngles.SinSqrThetaW);
    double ggW = model_.StandardModel.Couplings.gSU2;
    
    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widths;
    widths.push_back(widthZBosonZBoson(zBoson, ggZ, gsVV));
    widths.push_back(widthWBosonWBoson(wBoson, ggW, gsVV));
    return widths;
}

std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> SaxionWidthsDFSZ::widthVBosonHiggs(){
    auto higgsPseudo = ModelBaseOps::FindConst(particles_, "higgspseudoscalar");
    auto higgsCharged = ModelBaseOps::FindConst(particles_, "higgscharged");
    auto wBoson = ModelBaseOps::FindConst(particles_, "wboson");
    auto zBoson = ModelBaseOps::FindConst(particles_, "zboson");

    double alpha = model_.Susy.MixingAngles.alfah;
    double beta = atan(model_.Susy.TanBeta);
    double gsVh = _epsilonhl * cos(beta + alpha) - _epsilonHH * sin(beta + alpha);
    double ggZ = model_.StandardModel.Couplings.gSU2 / sqrt(1 - model_.StandardModel.MixingAngles.SinSqrThetaW);
    double ggW = model_.StandardModel.Couplings.gSU2;

    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widths;
    widths.push_back(widthZBosonHiggs(zBoson, higgsPseudo, ggZ, gsVh));
    widths.push_back(widthWBosonHiggs(wBoson, higgsCharged, ggW, gsVh));
    return widths;
}

// from arXiv:1309.5365
std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> SaxionWidthsDFSZ::widthFermionFermion(){
    double alpha = model_.Susy.MixingAngles.alfah;
    double beta = atan(model_.Susy.TanBeta);
    // couplings from A.12
    double gsffU = 1. / sin(beta) * ( - _epsilonhl * cos(alpha) + _epsilonHH * sin(alpha) );
    double gsffD = 1. / cos(beta) * ( - _epsilonhl * sin(alpha) - _epsilonHH * cos(alpha) );
    // sum over top and bottom quarks
    double wTT = widthFermionFermion(parent_.Mass, model_.Masses.MSusy.topMSusy, model_.StandardModel.vEW, gsffU, model_.StandardModel.NC);
    double wBB = widthFermionFermion(parent_.Mass, model_.Masses.MSusy.botMSusy, model_.StandardModel.vEW, gsffD, model_.StandardModel.NC);

    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widths;
    widths.push_back(Models::PartialWidth( parent_, { ModelBaseOps::FindConst(particles_, "bottomq"), ModelBaseOps::FindConst(particles_, "bottomq") }, wBB));
    widths.push_back(Models::PartialWidth( parent_, { ModelBaseOps::FindConst(particles_, "topq"), ModelBaseOps::FindConst(particles_, "topq") }, wTT));
    return widths;
}

double SaxionWidthsDFSZ::widthNeutralinoNeutralino(double mNeutralino1, double mNeutralino2, double mSaxion, double thz1, double thz2, double zetsIJ, double zetsJI){
    double width = 0.;
    if (abs(mSaxion) > abs(mNeutralino1) + abs(mNeutralino2)){
        width = (1. / ( 8. * M_PI ) ) * abs(mSaxion) * pow( ( zetsIJ + zetsJI ), 2. ) * (1. - pow( ( abs(mNeutralino1) + thz1 * thz2 * abs(mNeutralino2) ) / abs(mSaxion), 2. ) ) * sqrt( psLamb( 1., pow(mNeutralino1, 2.) / pow(mSaxion, 2.), pow(mNeutralino2, 2.) / pow(mSaxion, 2.) ) );
    }
    return width;
}

std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> SaxionWidthsDFSZ::widthNeutralinoNeutralino(double cH){
    auto neutralino1 = ModelBaseOps::FindConst(particles_, "neutralino1");
    auto neutralino2 = ModelBaseOps::FindConst(particles_, "neutralino2");
    auto neutralino3 = ModelBaseOps::FindConst(particles_, "neutralino3");
    auto neutralino4 = ModelBaseOps::FindConst(particles_, "neutralino4");

    double zets[4][4];
    double thz[4] = { copysign(1., neutralino1.Mass), copysign(1., neutralino2.Mass), copysign(1., neutralino3.Mass), copysign(1., neutralino4.Mass) };

    for (int i=0; i<4; ++i){
        for (int j=0; j<4; ++j){
            double xhl = -(1. / 2.) * thz[i] * thz[j] * ( model_.Susy.MixingAngles.zMix[i][1] * sin(model_.Susy.MixingAngles.alfah) - model_.Susy.MixingAngles.zMix[i][0] * cos(model_.Susy.MixingAngles.alfah) ) * ( model_.StandardModel.Couplings.gSU2 * model_.Susy.MixingAngles.zMix[j][2] - model_.StandardModel.Couplings.gU1 * model_.Susy.MixingAngles.zMix[j][3] );
            double xhh = -(1. / 2.) * thz[i] * thz[j] * ( model_.Susy.MixingAngles.zMix[i][1] * cos(model_.Susy.MixingAngles.alfah) + model_.Susy.MixingAngles.zMix[i][0] * sin(model_.Susy.MixingAngles.alfah) ) * ( model_.StandardModel.Couplings.gSU2 * model_.Susy.MixingAngles.zMix[j][2] - model_.StandardModel.Couplings.gU1 * model_.Susy.MixingAngles.zMix[j][3] );
            double xs = thz[i] * thz[j] * model_.Susy.MixingAngles.zMix[i][0] * model_.Susy.MixingAngles.zMix[j][1];
            zets[i][j] = _epsilonhl * xhl + _epsilonHH * xhh - cH * model_.Susy.Couplings.mu * xs / ( 2. * sqrt(2.) * model_.PQSector.Potential.vPQ );
        }
    }

    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widths;
    // same states get additional 1/2 in width since identical particles
    widths.push_back( Models::PartialWidth( parent_, { neutralino1, neutralino1 }, 1./2. * widthNeutralinoNeutralino(neutralino1.Mass, neutralino1.Mass, parent_.Mass, thz[0], thz[0], zets[0][0], zets[0][0]) ) );
    widths.push_back( Models::PartialWidth( parent_, { neutralino2, neutralino2 }, 1./2. * widthNeutralinoNeutralino(neutralino2.Mass, neutralino2.Mass, parent_.Mass, thz[1], thz[1], zets[1][1], zets[1][1]) ) );
    widths.push_back( Models::PartialWidth( parent_, { neutralino3, neutralino3 }, 1./2. * widthNeutralinoNeutralino(neutralino3.Mass, neutralino3.Mass, parent_.Mass, thz[2], thz[2], zets[2][2], zets[2][2]) ) );
    widths.push_back( Models::PartialWidth( parent_, { neutralino4, neutralino4 }, 1./2. * widthNeutralinoNeutralino(neutralino4.Mass, neutralino4.Mass, parent_.Mass, thz[3], thz[3], zets[3][3], zets[3][3]) ) );

    // different states get additional 2(????) since summing only over upper triangle of zets matrix
    widths.push_back( Models::PartialWidth( parent_, { neutralino1, neutralino2 }, 2. * widthNeutralinoNeutralino(neutralino1.Mass, neutralino2.Mass, parent_.Mass, thz[0], thz[1], zets[0][1], zets[1][0]) ) );
    widths.push_back( Models::PartialWidth( parent_, { neutralino1, neutralino3 }, 2. * widthNeutralinoNeutralino(neutralino1.Mass, neutralino3.Mass, parent_.Mass, thz[0], thz[2], zets[0][2], zets[2][0]) ) );
    widths.push_back( Models::PartialWidth( parent_, { neutralino1, neutralino4 }, 2. * widthNeutralinoNeutralino(neutralino1.Mass, neutralino4.Mass, parent_.Mass, thz[0], thz[3], zets[0][3], zets[3][0]) ) );
    widths.push_back( Models::PartialWidth( parent_, { neutralino2, neutralino3 }, 2. * widthNeutralinoNeutralino(neutralino2.Mass, neutralino3.Mass, parent_.Mass, thz[1], thz[2], zets[1][2], zets[2][1]) ) );
    widths.push_back( Models::PartialWidth( parent_, { neutralino2, neutralino4 }, 2. * widthNeutralinoNeutralino(neutralino2.Mass, neutralino4.Mass, parent_.Mass, thz[1], thz[3], zets[1][3], zets[3][1]) ) );
    widths.push_back( Models::PartialWidth( parent_, { neutralino3, neutralino4 }, 2. * widthNeutralinoNeutralino(neutralino3.Mass, neutralino4.Mass, parent_.Mass, thz[2], thz[3], zets[2][3], zets[3][2]) ) );
    return widths;
}

std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> SaxionWidthsDFSZ::widthSneutrinoSneutrino(){
    auto nuel = ModelBaseOps::FindConst(particles_, "sneutrinoselectronl");
    auto numu = ModelBaseOps::FindConst(particles_, "sneutrinosmuonl");
    auto nutau = ModelBaseOps::FindConst(particles_, "sneutrinostaul");
    auto wBoson = ModelBaseOps::FindConst(particles_, "wboson");

    double beta = atan(model_.Susy.TanBeta);
    // calculate all squark couplings
    auto upSneutrinoAhl = AhlUp(1./2., -1., 0., 0., 0., 0., model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto upSneutrinoAHH = AHUp(1./2., -1., 0., 0., 0., 0., model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto upSneutrinosMixedAhl = couplingMatrixSfermions(upSneutrinoAhl.sLsL, 0., 0., 0.);
    auto upSneutrinosMixedAHH = couplingMatrixSfermions(upSneutrinoAHH.sLsL, 0., 0., 0.);

    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widths;
    widths.push_back( Models::PartialWidth( parent_, { nuel, nuel }, widthSfermionSfermion(nuel.Mass, nuel.Mass, parent_.Mass, upSneutrinosMixedAhl.LL, upSneutrinosMixedAHH.LL, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { nuel, ModelBaseOps::FindConst(particles_, "neutrinoelectron") }, 2. * widthSfermionSfermion(nuel.Mass, 0., parent_.Mass, upSneutrinosMixedAhl.LR, upSneutrinosMixedAHH.LR, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { ModelBaseOps::FindConst(particles_, "neutrinoelectron"), ModelBaseOps::FindConst(particles_, "neutrinoelectron") }, widthSfermionSfermion(0., 0., parent_.Mass, upSneutrinosMixedAhl.RR, upSneutrinosMixedAHH.RR, 1.) ));

    widths.push_back( Models::PartialWidth( parent_, { numu, numu }, widthSfermionSfermion(numu.Mass, numu.Mass, parent_.Mass, upSneutrinosMixedAhl.LL, upSneutrinosMixedAHH.LL, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { numu, ModelBaseOps::FindConst(particles_, "neutrinomuon") }, 2. * widthSfermionSfermion(numu.Mass, 0., parent_.Mass, upSneutrinosMixedAhl.LR, upSneutrinosMixedAHH.LR, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { ModelBaseOps::FindConst(particles_, "neutrinomuon"), ModelBaseOps::FindConst(particles_, "neutrinomuon") }, widthSfermionSfermion(0., 0., parent_.Mass, upSneutrinosMixedAhl.RR, upSneutrinosMixedAHH.RR, 1.) ));

    widths.push_back( Models::PartialWidth( parent_, { nutau, nutau }, widthSfermionSfermion(nutau.Mass, nutau.Mass, parent_.Mass, upSneutrinosMixedAhl.LL, upSneutrinosMixedAHH.LL, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { nutau, ModelBaseOps::FindConst(particles_, "neutrinotau") }, 2. * widthSfermionSfermion(nutau.Mass, 0., parent_.Mass, upSneutrinosMixedAhl.LR, upSneutrinosMixedAHH.LR, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { ModelBaseOps::FindConst(particles_, "neutrinotau"), ModelBaseOps::FindConst(particles_, "neutrinotau") }, widthSfermionSfermion(0., 0., parent_.Mass, upSneutrinosMixedAhl.RR, upSneutrinosMixedAHH.RR, 1.) ));
    return widths;
}

std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> SaxionWidthsDFSZ::widthSleptonSlepton(){
    auto sel = ModelBaseOps::FindConst(particles_, "selectronl");
    auto ser = ModelBaseOps::FindConst(particles_, "selectronr");
    auto smul = ModelBaseOps::FindConst(particles_, "smuonl");
    auto smur = ModelBaseOps::FindConst(particles_, "smuonr");
    auto stau1 = ModelBaseOps::FindConst(particles_, "stau1");
    auto stau2 = ModelBaseOps::FindConst(particles_, "stau2");
    auto wBoson = ModelBaseOps::FindConst(particles_, "wboson");

    double beta = atan(model_.Susy.TanBeta);
    auto downSleptonAhl = AhlDown(-1./2., -1., 0., 2., 0., 0., model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto downSleptonAHH = AHDown(-1./2., -1., 0., 2., 0., 0., model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto downSleptonsMixedAhl = couplingMatrixSfermions(downSleptonAhl.sLsL, downSleptonAhl.sRsR, downSleptonAhl.sLsR, 0.);
    auto downSleptonsMixedAHH = couplingMatrixSfermions(downSleptonAHH.sLsL, downSleptonAhl.sRsR, downSleptonAhl.sLsR, 0.);

    auto stauSleptonAhl = AhlDown(-1./2., -1., 0., 2., model_.Susy.Couplings.A_tau, model_.Masses.MSusy.tauMSusy, model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto stauSleptonAHH = AHDown(-1./2., -1., 0., 2., model_.Susy.Couplings.A_tau, model_.Masses.MSusy.tauMSusy, model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto stauSleptonsMixedAhl = couplingMatrixSfermions(stauSleptonAhl.sLsL, stauSleptonAhl.sRsR, stauSleptonAhl.sLsR, model_.Susy.MixingAngles.thetaL);
    auto stauSleptonsMixedAHH = couplingMatrixSfermions(stauSleptonAHH.sLsL, stauSleptonAHH.sRsR, stauSleptonAHH.sLsR, model_.Susy.MixingAngles.thetaL);

    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widths;
    widths.push_back( Models::PartialWidth( parent_, { sel, sel }, widthSfermionSfermion(sel.Mass, sel.Mass, parent_.Mass, downSleptonsMixedAhl.LL, downSleptonsMixedAHH.LL, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { sel, ser }, 2. * widthSfermionSfermion(sel.Mass, ser.Mass, parent_.Mass, downSleptonsMixedAhl.LR, downSleptonsMixedAHH.LR, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { ser, ser }, widthSfermionSfermion(ser.Mass, ser.Mass, parent_.Mass, downSleptonsMixedAhl.RR, downSleptonsMixedAHH.RR, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { smul, smul }, widthSfermionSfermion(smul.Mass, smul.Mass, parent_.Mass, downSleptonsMixedAhl.LL, downSleptonsMixedAHH.LL, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { smul, smur }, 2. * widthSfermionSfermion(smul.Mass, smur.Mass, parent_.Mass, downSleptonsMixedAhl.LR, downSleptonsMixedAHH.LR, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { smur, smur }, widthSfermionSfermion(smur.Mass, smur.Mass, parent_.Mass, downSleptonsMixedAhl.RR, downSleptonsMixedAHH.RR, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { stau1, stau1 }, widthSfermionSfermion(stau1.Mass, stau1.Mass, parent_.Mass, stauSleptonsMixedAhl.LL, stauSleptonsMixedAHH.LL, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { stau1, stau2 }, 2. * widthSfermionSfermion(stau1.Mass, stau2.Mass, parent_.Mass, stauSleptonsMixedAhl.LR, stauSleptonsMixedAHH.LR, 1.) ));
    widths.push_back( Models::PartialWidth( parent_, { stau2, stau2 }, widthSfermionSfermion(stau2.Mass, stau2.Mass, parent_.Mass, stauSleptonsMixedAhl.RR, stauSleptonsMixedAHH.RR, 1.) ));
    return widths;
}

std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> SaxionWidthsDFSZ::widthSquarkSquark(){
    auto upl = ModelBaseOps::FindConst(particles_, "suplsq");
    auto upr = ModelBaseOps::FindConst(particles_, "suprsq");
    auto dnl = ModelBaseOps::FindConst(particles_, "sdownlsq");
    auto dnr = ModelBaseOps::FindConst(particles_, "sdownrsq");
    auto stl = ModelBaseOps::FindConst(particles_, "sstrangelsq");
    auto str = ModelBaseOps::FindConst(particles_, "sstrangersq");
    auto chl = ModelBaseOps::FindConst(particles_, "scharmlsq");
    auto chr = ModelBaseOps::FindConst(particles_, "scharmrsq");
    auto sb1 = ModelBaseOps::FindConst(particles_, "sbottom1sq");
    auto sb2 = ModelBaseOps::FindConst(particles_, "sbottom2sq");
    auto st1 = ModelBaseOps::FindConst(particles_, "stop1sq");
    auto st2 = ModelBaseOps::FindConst(particles_, "stop2sq");
    auto wBoson = ModelBaseOps::FindConst(particles_, "wboson");

    double beta = atan(model_.Susy.TanBeta);
    // calculate all squark couplings
    auto upSquarkAhl = AhlUp(1./2., 1./3., 0., -4./3., 0., 0., model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto upSquarkAHH = AHUp(1./2., 1./3., 0., -4./3., 0., 0., model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto upSquarksMixedAhl = couplingMatrixSfermions(upSquarkAhl.sLsL, upSquarkAhl.sRsR, upSquarkAhl.sLsR, 0.);
    auto upSquarksMixedAHH = couplingMatrixSfermions(upSquarkAHH.sLsL, upSquarkAHH.sRsR, upSquarkAHH.sLsR, 0.);

    auto stopSquarkAhl = AhlUp(1./2., 1./3., 0., -4./3., model_.Susy.Couplings.A_t, model_.Masses.MSusy.topMSusy, model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto stopSquarkAHH = AHUp(1./2., 1./3., 0., -4./3., model_.Susy.Couplings.A_t, model_.Masses.MSusy.topMSusy, model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto stopSquarksMixedAhl = couplingMatrixSfermions(stopSquarkAhl.sLsL, stopSquarkAhl.sRsR, stopSquarkAhl.sLsR, model_.Susy.MixingAngles.thetaT);
    auto stopSquarksMixedAHH = couplingMatrixSfermions(stopSquarkAHH.sLsL, stopSquarkAHH.sRsR, stopSquarkAHH.sLsR, model_.Susy.MixingAngles.thetaT);

    auto downSquarkAhl = AhlDown(-1./2., 1./3., 0., 2./3., 0., 0., model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto downSquarkAHH = AHDown(-1./2., 1./3., 0., 2./3., 0., 0., model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto downSquarksMixedAhl = couplingMatrixSfermions(downSquarkAhl.sLsL, downSquarkAhl.sRsR, downSquarkAhl.sLsR, 0.);
    auto downSquarksMixedAHH = couplingMatrixSfermions(downSquarkAHH.sLsL, downSquarkAHH.sRsR, downSquarkAHH.sLsR, 0.);

    auto sbottomSquarkAhl = AhlDown(-1./2., 1./3., 0., 2./3., model_.Susy.Couplings.A_b, model_.Masses.MSusy.botMSusy, model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto sbottomSquarkAHH = AHDown(-1./2., 1./3., 0., 2./3., model_.Susy.Couplings.A_b, model_.Masses.MSusy.botMSusy, model_.Susy.MixingAngles.alfah, beta, model_.StandardModel.Couplings.gSU2, wBoson.Mass, model_.StandardModel.MixingAngles.SinSqrThetaW, 1.-model_.StandardModel.MixingAngles.SinSqrThetaW, model_.Susy.Couplings.mu);
    auto sbottomSquarksMixedAhl = couplingMatrixSfermions(sbottomSquarkAhl.sLsL, sbottomSquarkAhl.sRsR, sbottomSquarkAhl.sLsR, model_.Susy.MixingAngles.thetaB);
    auto sbottomSquarksMixedAHH = couplingMatrixSfermions(sbottomSquarkAHH.sLsL, sbottomSquarkAHH.sRsR, sbottomSquarkAHH.sLsR, model_.Susy.MixingAngles.thetaB);

    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widths;
    // get up squark widths
    widths.push_back( Models::PartialWidth( parent_, { upl, upl }, widthSfermionSfermion(upl.Mass, upl.Mass, parent_.Mass, upSquarksMixedAhl.LL, upSquarksMixedAHH.LL, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { upl, upr }, 2. * widthSfermionSfermion(upl.Mass, upr.Mass, parent_.Mass, upSquarksMixedAhl.LR, upSquarksMixedAHH.LR, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { upr, upr }, widthSfermionSfermion(upr.Mass, upr.Mass, parent_.Mass, upSquarksMixedAhl.RR, upSquarksMixedAHH.RR, 3.) ));

    widths.push_back( Models::PartialWidth( parent_, { chl, chl }, widthSfermionSfermion(chl.Mass, chl.Mass, parent_.Mass, upSquarksMixedAhl.LL, upSquarksMixedAHH.LL, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { chl, chr }, 2. * widthSfermionSfermion(chl.Mass, chr.Mass, parent_.Mass, upSquarksMixedAhl.LR, upSquarksMixedAHH.LR, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { chr, chr }, widthSfermionSfermion(chr.Mass, chr.Mass, parent_.Mass, upSquarksMixedAhl.RR, upSquarksMixedAHH.RR, 3.) ));

    widths.push_back( Models::PartialWidth( parent_, { st1, st1 }, widthSfermionSfermion(st1.Mass, st1.Mass, parent_.Mass, stopSquarksMixedAhl.LL, stopSquarksMixedAHH.LL, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { st1, st2 }, 2. * widthSfermionSfermion(st1.Mass, st2.Mass, parent_.Mass, stopSquarksMixedAhl.LR, stopSquarksMixedAHH.LR, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { st2, st2 }, widthSfermionSfermion(st2.Mass, st2.Mass, parent_.Mass, stopSquarksMixedAhl.RR, stopSquarksMixedAHH.RR, 3.) ));
    
    // get down squark widths
    widths.push_back( Models::PartialWidth( parent_, { dnl, dnl }, widthSfermionSfermion(dnl.Mass, dnl.Mass, parent_.Mass, downSquarksMixedAhl.LL, downSquarksMixedAHH.LL, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { dnl, dnr }, 2. * widthSfermionSfermion(dnl.Mass, dnr.Mass, parent_.Mass, downSquarksMixedAhl.LR, downSquarksMixedAHH.LR, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { dnr, dnr }, widthSfermionSfermion(dnr.Mass, dnr.Mass, parent_.Mass, downSquarksMixedAhl.RR, downSquarksMixedAHH.RR, 3.) ));

    widths.push_back( Models::PartialWidth( parent_, { stl, stl }, widthSfermionSfermion(stl.Mass, stl.Mass, parent_.Mass, downSquarksMixedAhl.LL, downSquarksMixedAHH.LL, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { stl, str }, 2. * widthSfermionSfermion(stl.Mass, str.Mass, parent_.Mass, downSquarksMixedAhl.LR, downSquarksMixedAHH.LR, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { str, str }, widthSfermionSfermion(str.Mass, str.Mass, parent_.Mass, downSquarksMixedAhl.RR, downSquarksMixedAHH.RR, 3.) ));

    widths.push_back( Models::PartialWidth( parent_, { sb1, sb1 }, widthSfermionSfermion(sb1.Mass, sb1.Mass, parent_.Mass, sbottomSquarksMixedAhl.LL, sbottomSquarksMixedAHH.LL, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { sb1, sb2 }, 2. * widthSfermionSfermion(sb1.Mass, sb2.Mass, parent_.Mass, sbottomSquarksMixedAhl.LR, sbottomSquarksMixedAHH.LR, 3.) ));
    widths.push_back( Models::PartialWidth( parent_, { sb2, sb2 }, widthSfermionSfermion(sb2.Mass, sb2.Mass, parent_.Mass, sbottomSquarksMixedAhl.RR, sbottomSquarksMixedAHH.RR, 3.) ));

    return widths;
}

double SaxionWidthsDFSZ::widthSfermionSfermion(double mSfermion1, double mSfermion2, double mSaxion, double AhlSFCoupling, double AHHSFCoupling, double nCF){
    if (mSaxion > (mSfermion1 + mSfermion2)){
        return ( 1./(16. * M_PI * mSaxion) ) * ( pow( (_epsilonhl * AhlSFCoupling + _epsilonHH * AHHSFCoupling), 2. ) ) * nCF * sqrt( psLamb( 1., pow( mSfermion1 , 2. ) / pow( mSaxion, 2. ), pow( mSfermion2, 2. ) / pow( mSaxion, 2. ) ) );
    }
    return 0;
}

SfermionCouplings SaxionWidthsDFSZ::AhlUp(double T3L, double YL, double T3R, double YR, double AU, double mUpFermion, double alpha, double beta, double gSU2, double mW, double STW2, double CTW2, double mu){
    SfermionCouplings couplings;
    couplings.sLsL = gSU2 * ( mW * ( T3L - (1./2.) * YL * (STW2/CTW2) ) * sin(beta-alpha) - ( pow( mUpFermion, 2. ) / mW ) * cos(alpha) / sin(beta) );
    couplings.sRsR = gSU2 * ( mW * ( T3R - (1./2.) * YR * (STW2/CTW2) ) * sin(beta-alpha) - ( pow( mUpFermion, 2. ) / mW ) * cos(alpha) / sin(beta) );
    couplings.sLsR = gSU2 * ( mUpFermion / (2. * mW * sin(beta) ) ) * ( -mu * sin(alpha) + AU * cos(alpha) );
    return couplings;
}

SfermionCouplings SaxionWidthsDFSZ::AhlDown(double T3L, double YL, double T3R, double YR, double AD, double mDownFermion, double alpha, double beta, double gSU2, double mW, double STW2, double CTW2, double mu){
    SfermionCouplings couplings;
    couplings.sLsL = gSU2 * ( mW * ( T3L - (1./2.) * YL * (STW2/CTW2) ) * sin(beta-alpha) - ( pow( mDownFermion, 2. ) / mW ) * sin(alpha) / cos(beta) );
    couplings.sRsR = gSU2 * ( mW * ( T3R - (1./2.) * YR * (STW2/CTW2) ) * sin(beta-alpha) - ( pow( mDownFermion, 2. ) / mW ) * sin(alpha) / cos(beta) );
    couplings.sLsR = gSU2 * ( mDownFermion / ( 2. * mW * cos(beta) ) ) * ( -mu * cos(alpha) + AD * sin(alpha) );
    return couplings;
}

SfermionCouplings SaxionWidthsDFSZ::AHUp(double T3L, double YL, double T3R, double YR, double AU, double mUpFermion, double alpha, double beta, double gSU2, double mW, double STW2, double CTW2, double mu){
    SfermionCouplings couplings;
    couplings.sLsL = gSU2 * ( -mW * ( T3L - (1./2.) * YL * (STW2/CTW2) ) * cos(beta-alpha) + ( pow( mUpFermion, 2. ) / mW ) * sin(alpha) / sin(beta) );
    couplings.sRsR = gSU2 * ( -mW * ( T3R - (1./2.) * YR * (STW2/CTW2) ) * cos(beta-alpha) + ( pow( mUpFermion, 2. ) / mW ) * sin(alpha) / sin(beta) );
    couplings.sLsR = gSU2 * ( mUpFermion / ( 2. * mW * sin(beta) ) ) * ( -mu * cos(alpha) - AU * sin(alpha) );
    return couplings;
}

SfermionCouplings SaxionWidthsDFSZ::AHDown(double T3L, double YL, double T3R, double YR, double AD, double mDownFermion, double alpha, double beta, double gSU2, double mW, double STW2, double CTW2, double mu){
    SfermionCouplings couplings;
    couplings.sLsL = gSU2 * ( -mW * ( T3L - (1./2.) * YL * (STW2/CTW2) ) * cos(beta-alpha) - ( pow( mDownFermion, 2. ) / mW ) * cos(alpha) / cos(beta) );
    couplings.sRsR = gSU2 * ( -mW * ( T3R - (1./2.) * YR * (STW2/CTW2) ) * cos(beta-alpha) - ( pow( mDownFermion, 2. ) / mW ) * cos(alpha) / cos(beta) );
    couplings.sLsR = gSU2 * ( mDownFermion / ( 2. * mW * cos(beta) ) ) * ( mu * sin(alpha) + AD * cos(alpha) );
    return couplings;
}

SfermionMixedCouplings SaxionWidthsDFSZ::couplingMatrixSfermions(double sLsLCoupling, double sRsRCoupling, double sLsRCoupling, double mixAngle){
    SfermionMixedCouplings couplings;
    couplings.LL = sLsLCoupling * pow( cos(mixAngle), 2. ) + sRsRCoupling * pow( sin(mixAngle), 2. ) - 2. * sLsRCoupling * cos(mixAngle) * sin(mixAngle);
    couplings.LR = sLsLCoupling * cos(mixAngle) * sin(mixAngle) - sRsRCoupling * cos(mixAngle) * sin(mixAngle) + 2. * sLsRCoupling * cos(2.*mixAngle);
    couplings.RR = sLsLCoupling * pow( sin(mixAngle), 2. ) + sRsRCoupling * pow( cos(mixAngle), 2. ) + 2. * sLsRCoupling * cos(mixAngle) * sin(mixAngle);
    return couplings;
}

double SaxionWidthsDFSZ::ephl(double cH, double vEW){
    auto higgsLight = ModelBaseOps::FindConst(particles_, "higgslight");
    auto higgsPseudo = ModelBaseOps::FindConst(particles_, "higgspseudoscalar");
    double beta = atan(model_.Susy.TanBeta);
    double tanr = - cH * vEW * sin(2. * beta) / (2. * model_.PQSector.Potential.vPQ);
    double ephl = - tanr / (pow(higgsLight.Mass, 2.) - pow(parent_.Mass, 2.)) * ( -pow(higgsPseudo.Mass, 2.0) * cos( beta - model_.Susy.MixingAngles.alfah ) + 4. * pow( model_.Susy.Couplings.mu, 2. ) * sin( model_.Susy.MixingAngles.alfah + beta ) / sin( 2. * beta ) );
    return ephl;
}

double SaxionWidthsDFSZ::epHH(double cH, double vEW){
    auto higgsHeavy = ModelBaseOps::FindConst(particles_, "higgsheavy");
    auto higgsPseudo = ModelBaseOps::FindConst(particles_, "higgspseudoscalar");
    double beta = atan(model_.Susy.TanBeta);
    double tanr = - cH * vEW * sin(2. * beta) / (2. * model_.PQSector.Potential.vPQ);
    double ephl = - tanr / (pow(higgsHeavy.Mass, 2.) - pow(parent_.Mass, 2.)) * ( -pow(higgsPseudo.Mass, 2.0) * sin( beta - model_.Susy.MixingAngles.alfah ) + 4. * pow( model_.Susy.Couplings.mu, 2.) * cos( model_.Susy.MixingAngles.alfah + beta ) / sin( 2. * beta ) );
    return ephl;
}

double SaxionWidthsDFSZ::psLamb(double x, double y, double z){
    double result = pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0) - 2.0 * ( x * y + y * z + z * x);
    return result;
}

std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> SaxionWidthsDFSZ::widthDiHiggs(double cH){
    auto higgsLight = ModelBaseOps::FindConst(particles_, "higgslight");
    auto higgsHeavy = ModelBaseOps::FindConst(particles_, "higgsheavy");
    auto higgsPseudo = ModelBaseOps::FindConst(particles_, "higgspseudoscalar");
    auto higgsCharged = ModelBaseOps::FindConst(particles_, "higgscharged");
    auto zBoson = ModelBaseOps::FindConst(particles_, "zboson");
    
    double alpha = model_.Susy.MixingAngles.alfah;
    double beta = atan(model_.Susy.TanBeta);
    double CTW2 = 1. - model_.StandardModel.MixingAngles.SinSqrThetaW;
    // calculate couplings
    double lshl = ( sqrt(2.) * cH * pow(model_.Susy.Couplings.mu,2.) / model_.PQSector.Potential.vPQ ) * ( 1. - (1./4.) * ( pow(higgsPseudo.Mass, 2.) / pow(model_.Susy.Couplings.mu, 2.) ) * sin(2. * beta) * sin(2. * alpha) ) + ( pow(zBoson.Mass, 2.) / (sqrt(2.) * model_.StandardModel.vEW) ) * cos(2. * alpha) * sin(beta-alpha) * ( 3. * _epsilonhl - _epsilonHH * ( 2. * tan(2. * alpha) + pow(tan(beta-alpha), -1.) ) );
    double lsHH = ( sqrt(2.) * cH * pow(model_.Susy.Couplings.mu,2.) / model_.PQSector.Potential.vPQ ) * ( 1. + (1./4.) * ( pow(higgsPseudo.Mass, 2.) / pow(model_.Susy.Couplings.mu, 2.) ) * sin(2. * beta) * sin(2. * alpha) ) + ( pow(zBoson.Mass, 2.) / (sqrt(2.) * model_.StandardModel.vEW) ) * cos(2. * alpha) * sin(beta-alpha) * ( 3. * _epsilonHH * pow(tan(beta-alpha), -1.) + _epsilonhl * ( 2. * tan(2. * alpha) * pow(tan(beta-alpha), -1.) -1. ) );
    double lsA = ( sqrt(2.) * cH * pow(model_.Susy.Couplings.mu,2.) / model_.PQSector.Potential.vPQ ) * ( 1. + (1./4.) * ( pow(higgsPseudo.Mass, 2.) / pow(model_.Susy.Couplings.mu, 2.) ) * pow(sin(2. * beta), 2.) ) + ( pow(zBoson.Mass, 2.) / ( sqrt(2.) * model_.StandardModel.vEW ) ) * cos(2. * beta)  * sin(beta-alpha) * ( _epsilonhl - _epsilonHH * pow(tan(beta-alpha), -1.) );
    double lshlHH = -( cH * pow(higgsPseudo.Mass, 2.) /(2. * sqrt(2.) * model_.PQSector.Potential.vPQ ) ) * sin(2. * beta) * cos(2. * alpha) + ( pow(zBoson.Mass, 2.) / ( sqrt(2.) * model_.StandardModel.vEW ) ) * cos(2. * alpha) * sin(beta-alpha) * ( - _epsilonhl * ( 2. * tan(2. * alpha) + pow(tan(beta-alpha), -1.) ) + _epsilonHH * ( 2. * tan(2. * alpha) * pow(tan(beta-alpha), -1.) - 1. ) );
    double lsHpHm = ( sqrt(2.) * cH * pow(model_.Susy.Couplings.mu,2.) / model_.PQSector.Potential.vPQ ) * ( 1. + (1./4.) * ( pow(higgsPseudo.Mass, 2.) / pow(model_.Susy.Couplings.mu, 2.) ) * pow(sin(2. * beta), 2.) ) + ( pow(zBoson.Mass, 2.) / ( sqrt(2.) * model_.StandardModel.vEW ) ) * ( cos(2. * beta) * sin(beta-alpha) * ( _epsilonhl - _epsilonHH * pow(tan(beta-alpha), -1.)) + 2. * CTW2 * sin(beta+alpha) * ( _epsilonhl + _epsilonHH * pow(tan(beta+alpha), -1.) ) );

    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widths;
    widths.push_back( Models::PartialWidth( parent_, { higgsLight, higgsLight }, widthDiHiggs(lshl, higgsLight.Mass, higgsLight.Mass, parent_.Mass) ));
    widths.push_back( Models::PartialWidth( parent_, { higgsHeavy, higgsHeavy }, widthDiHiggs(lsHH, higgsHeavy.Mass, higgsHeavy.Mass, parent_.Mass) ));
    widths.push_back( Models::PartialWidth( parent_, { higgsPseudo, higgsPseudo }, widthDiHiggs(lsA, higgsPseudo.Mass, higgsPseudo.Mass, parent_.Mass) ));
    widths.push_back( Models::PartialWidth( parent_, { higgsLight, higgsHeavy }, 2. * widthDiHiggs(lshlHH, higgsLight.Mass, higgsHeavy.Mass, parent_.Mass) ));
    widths.push_back( Models::PartialWidth( parent_, { higgsCharged, higgsCharged }, 2. * widthDiHiggs(lsHpHm, higgsCharged.Mass, higgsCharged.Mass, parent_.Mass) ));
    return widths;
}

double SaxionWidthsDFSZ::widthDiHiggs(double coupling, double mHiggs1, double mHiggs2, double mSaxion){
    double width = 0.;
    if (abs(mSaxion) > ( abs(mHiggs1) + abs(mHiggs2) )){
        width = pow( coupling, 2. ) / (32. * M_PI * abs(mSaxion)) * sqrt( psLamb( 1., pow(mHiggs1, 2.) / pow(mSaxion, 2.), pow(mHiggs2, 2.) / pow(mSaxion, 2.) ) );
    }
    return width;
}

// from arXiv:1309.5365 - A.13
double SaxionWidthsDFSZ::widthCharginoCharginoSameGen(double mSaxion, double mChargino, double gSU2, double sigCoupling){
    double wCC = 0.;
    if (abs(mSaxion) > (2. * abs(mChargino))){
        wCC = pow(gSU2, 2.) / (4. * M_PI) * pow(sigCoupling, 2.) * abs(mSaxion) * pow(1. - 4. * pow(mChargino, 2.) / pow(mSaxion, 2.), 3./2.);
    }
    return wCC;
}

// from arXiv:1309.5365 - A.14
double SaxionWidthsDFSZ::widthCharginoCharginoDifferentGen(double mSaxion, double mChargino1, double mChargino2, double gSU2, double sigCoupling, double piCoupling){
    double wCC = 0.;
    if (abs(mSaxion) > (abs(mChargino1) + abs(mChargino2))){
        2. * pow(gSU2, 2.) / (16. * M_PI) * abs(mSaxion) * sqrt( psLamb(1., pow(mChargino1, 2.) / pow(mChargino1, 2.), pow(mChargino2, 2.) / pow(mChargino2, 2.)) ) * ( pow(sigCoupling, 2.) * ( 1. - pow((abs(mChargino1) + abs(mChargino2)) / mSaxion, 2.) ) + pow(piCoupling, 2.) * ( 1. - pow( (abs(mChargino2) - abs(mChargino1)) / mSaxion, 2. ) ) );
    }
    return wCC;
}

// helper method to calculate all couplings and return total width for same gen charginos total
std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> SaxionWidthsDFSZ::widthCharginoCharginoSameGen(double cH){
    auto chargino1 = ModelBaseOps::FindConst(particles_, "chargino1");
    auto chargino2 = ModelBaseOps::FindConst(particles_, "chargino2");

    auto alpha = model_.Susy.MixingAngles.alfah;
    auto gammaL = model_.Susy.MixingAngles.gammaL;
    auto gammaR = model_.Susy.MixingAngles.gammaR;
    double sgnGammaL = copysign(1.0, pow(tan(gammaL), -1.0));
    double sgnGammaR = copysign(1.0, pow(tan(gammaR), -1.0));
    double sgnW1 = copysign(1., chargino1.Mass);
    double sgnW2 = copysign(1., chargino2.Mass);

    // calculate same gen couplings
    double s1hl = 1./2. * sgnW1 * ( sin(alpha) * sin(gammaR) * cos(gammaL) + cos(alpha) * sin(gammaL) * cos(gammaR) );
    double s1HH = 1./2. * sgnW1 * ( cos(alpha) * sin(gammaR) * cos(gammaL) - sin(alpha) * sin(gammaL) * cos(gammaR) );
    double sig1s = _epsilonhl * s1hl + _epsilonHH * s1HH - cH * model_.Susy.Couplings.mu * sgnW1 * cos(gammaL) * cos(gammaR) / (4. * model_.StandardModel.Couplings.gSU2 * model_.PQSector.Potential.vPQ);

    double s2hl = - 1./2. * sgnW2 * sgnGammaL * sgnGammaR * ( sin(alpha) * cos(gammaR) * sin(gammaL) + cos(alpha) * cos(gammaL) * sin(gammaR) );
    double s2HH = - 1./2. * sgnW2 * sgnGammaL * sgnGammaR * ( cos(alpha) * cos(gammaR) * sin(gammaL) - sin(alpha) * cos(gammaL) * sin(gammaR) );
    double sig2s = _epsilonhl * s2hl + _epsilonHH * s2HH + cH * model_.Susy.Couplings.mu * sgnW2 * sgnGammaL * sgnGammaR * sin(gammaL) * sin(gammaR) / (4. * model_.StandardModel.Couplings.gSU2 * model_.PQSector.Potential.vPQ);

    std::deque<Models::PartialWidth, boost::pool_allocator<Models::PartialWidth>> widths;
    widths.push_back( Models::PartialWidth( parent_, { chargino1, chargino1 }, widthCharginoCharginoSameGen(parent_.Mass, chargino1.Mass, model_.StandardModel.Couplings.gSU2, sig1s) ));
    widths.push_back( Models::PartialWidth( parent_, { chargino2, chargino2 }, widthCharginoCharginoSameGen(parent_.Mass, chargino2.Mass, model_.StandardModel.Couplings.gSU2, sig2s) ));
    return widths;
}

// helper method to calculate all couplings and return total width for different gen charginos
Models::PartialWidth SaxionWidthsDFSZ::widthCharginoCharginoDifferentGen(double cH){
    auto chargino1 = ModelBaseOps::FindConst(particles_, "chargino1");
    auto chargino2 = ModelBaseOps::FindConst(particles_, "chargino2");

    auto alpha = model_.Susy.MixingAngles.alfah;
    auto gammaL = model_.Susy.MixingAngles.gammaL;
    auto gammaR = model_.Susy.MixingAngles.gammaR;
    double sgnGammaL = copysign(1.0, pow(tan(gammaL), -1.0));
    double sgnGammaR = copysign(1.0, pow(tan(gammaR), -1.0));
    double sgnW1 = copysign(1., chargino1.Mass);
    double sgnW2 = copysign(1., chargino2.Mass);

    // calculate different gen couplings
    double ss = 1./2. * ( sgnW1 * sgnGammaR * cos(gammaL) * sin(gammaR) - sgnW2 * sgnGammaL * sin(gammaL) * cos(gammaR) );
    double shl = 1./2. * (-sgnW1 * sgnGammaL * sin(alpha) * sin(gammaR) * sin(gammaL) + sgnW1 * sgnGammaL * cos(alpha) * cos(gammaL) * cos(gammaR) - sgnW2 * sgnGammaR * cos(alpha) * sin(gammaR) * sin(gammaL) + sgnW2 * sgnGammaR * sin(alpha) * cos(gammaR) * cos(gammaL));
    double shh = 1./2. * (-sgnW1 * sgnGammaL * cos(alpha) * sin(gammaR) * sin(gammaL) - sgnW1 * sgnGammaL * sin(alpha) * cos(gammaL) * cos(gammaR) + sgnW2 * sgnGammaR * sin(alpha) * sin(gammaR) * sin(gammaL) + sgnW2 * sgnGammaR * cos(alpha) * cos(gammaR) * cos(gammaL));
    double sigs = _epsilonhl * shl + _epsilonHH * shh - cH * model_.Susy.Couplings.mu * ss / (2. * model_.StandardModel.Couplings.gSU2 * model_.PQSector.Potential.vPQ);
    double ps = 1./2. * ( sgnW1 * sgnGammaR * cos(gammaL) * sin(gammaR) + sgnW2 * sgnGammaL * sin(gammaL) * cos(gammaR) );
    double phl = 1./2. * ( sgnW1 * sgnGammaL * sin(alpha) * sin(gammaR) * sin(gammaL) - sgnW1 * sgnGammaL * cos(alpha) * cos(gammaL) * cos(gammaR) - sgnW2 * sgnGammaR * cos(alpha) * sin(gammaR) * sin(gammaL) + sgnW2 * sgnGammaR * sin(alpha) * cos(gammaR) * cos(gammaL));
    double phh = 1./2. * ( sgnW1 * sgnGammaL * cos(alpha) * sin(gammaR) * sin(gammaL) + sgnW1 * sgnGammaL * sin(alpha) * cos(gammaL) * cos(gammaR) + sgnW2 * sgnGammaR * sin(alpha) * sin(gammaR) * sin(gammaL) + sgnW2 * sgnGammaR * cos(alpha) * cos(gammaR) * cos(gammaL));
    double pis = _epsilonhl * phl + _epsilonHH * phh - cH * model_.Susy.Couplings.mu * ps / (2. * model_.StandardModel.Couplings.gSU2 * model_.PQSector.Potential.vPQ);
    return Models::PartialWidth( parent_, { chargino1, chargino2 }, widthCharginoCharginoDifferentGen(parent_.Mass, chargino1.Mass, chargino2.Mass, model_.StandardModel.Couplings.gSU2, sigs, pis) );
}

// from arXiv:1309.5365 - A.7
Models::PartialWidth SaxionWidthsDFSZ::widthWBosonWBoson(const Models::Particle& wBoson, double ggW, double gsVV){
    double wWW = 0.;
    if (abs(parent_.Mass) > ( 2. * wBoson.Mass )){
        wWW = pow(ggW, 2.) * pow(gsVV, 2.) / (16. * M_PI) * abs(parent_.Mass) * ( 3. * pow(wBoson.Mass, 2.) / pow(parent_.Mass, 2.) + pow(parent_.Mass, 2.) / (4. * pow(wBoson.Mass, 2.)) ) * (1. - 4. * pow(wBoson.Mass, 2.) / pow(parent_.Mass, 2.)) * sqrt( 1. - 4. * pow(wBoson.Mass, 2.) / pow(parent_.Mass, 2.) );
    }
    return Models::PartialWidth( parent_, { wBoson, wBoson }, wWW);
}

// from arXiv:1309.5365 - A.7
Models::PartialWidth SaxionWidthsDFSZ::widthZBosonZBoson(const Models::Particle& zBoson, double ggZ, double gsVV){
    double wZZ = 0.;
    if (abs(parent_.Mass) > ( 2. * zBoson.Mass )){
        wZZ = pow(ggZ, 2.) * pow(gsVV, 2.) / (16. * M_PI) * abs(parent_.Mass) * ( 3. * pow(zBoson.Mass, 2.) / pow(parent_.Mass, 2.) + pow(parent_.Mass, 2.) / (4. * pow(zBoson.Mass, 2.)) ) * (1. - 4. * pow(zBoson.Mass, 2.) / pow(parent_.Mass, 2.)) * sqrt( 1. - 4. * pow(zBoson.Mass, 2.) / pow(parent_.Mass, 2.) ) / 2.;
    }
    return Models::PartialWidth( parent_, { zBoson, zBoson }, wZZ);
}

Models::PartialWidth SaxionWidthsDFSZ::widthWBosonHiggs(const Models::Particle& wBoson, const Models::Particle& higgsCharged, double ggW, double gsVh){
    double wWH = 0.;
    if (abs(parent_.Mass) > (wBoson.Mass + abs(higgsCharged.Mass))){
        wWH = pow(ggW, 2.) * pow(gsVh, 2.) / (32. * M_PI) * abs(pow(parent_.Mass, 3.)) / pow(wBoson.Mass, 2.) * ( pow( 1. - pow(higgsCharged.Mass, 2.) / pow(parent_.Mass, 2.), 2.) - 2. * pow(wBoson.Mass, 2.) / pow(parent_.Mass, 2.) * ( 1. + pow(higgsCharged.Mass, 2.) / pow(parent_.Mass, 2.) ) + pow(wBoson.Mass, 4.) / pow(parent_.Mass, 4.) ) * sqrt( psLamb(1., pow(wBoson.Mass, 2.) / pow(parent_.Mass, 2.), pow(higgsCharged.Mass, 2.) / pow(parent_.Mass, 2.) ) ) / 2.;
    }
    return Models::PartialWidth( parent_, { higgsCharged, wBoson }, wWH);
}

Models::PartialWidth SaxionWidthsDFSZ::widthZBosonHiggs(const Models::Particle& zBoson, const Models::Particle& higgsPseudo, double ggZ, double gsVh){
    double wZH = 0.;
    if (abs(parent_.Mass) > (zBoson.Mass + abs(higgsPseudo.Mass))){
        wZH = pow(ggZ, 2.) * pow(gsVh, 2.) / (32. * M_PI) * abs(pow(parent_.Mass, 3.)) / pow(zBoson.Mass, 2.) * ( pow( 1. - pow(higgsPseudo.Mass, 2.) / pow(parent_.Mass, 2.), 2.) - 2. * pow(zBoson.Mass, 2.) / pow(parent_.Mass, 2.) * ( 1. + pow(higgsPseudo.Mass, 2.) / pow(parent_.Mass, 2.) ) + pow(zBoson.Mass, 4.) / pow(parent_.Mass, 4.) ) * sqrt( psLamb(1., pow(zBoson.Mass, 2.) / pow(parent_.Mass, 2.), pow(higgsPseudo.Mass, 2.) / pow(parent_.Mass, 2.) ) ) / 2.;
    }
    return Models::PartialWidth( parent_, { higgsPseudo, zBoson }, wZH);
}

// from arXiv:1309.5365 - A.11
double SaxionWidthsDFSZ::widthFermionFermion(double mSaxion, double mFermion, double vEW, double gCoupling, double nC){
    double mFF = 0.;
    if (abs(mSaxion) > (2. * abs(mFermion))){
        mFF = nC / (16. * M_PI) * pow(mFermion, 2.) / pow(vEW, 2.) * pow(gCoupling, 2.) * abs(mSaxion) * pow(1. - 4. * pow(mFermion, 2.) / pow(mSaxion, 2.), 3./2.);
    }
    return mFF;
}
