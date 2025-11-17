#include <ParticleDefs/Gravitino/GravitinoWidths.h>

using namespace std;

GravitinoWidths::GravitinoWidths(const ModelBase& model, const std::deque< Models::Particle >& particles)
    : model_(model),
    particles_(particles)
{
    parent_ = ModelBaseOps::FindConst(particles_, "gravitino");

    gPrime_ = sqrt(4. * M_PI * model_.StandardModel.Couplings.alphaEM / (1. - model_.StandardModel.MixingAngles.SinSqrThetaW));
    g_ = sqrt(4. * M_PI * model_.StandardModel.Couplings.alphaEM / model_.StandardModel.MixingAngles.SinSqrThetaW);
    gZ_ = sqrt( pow(gPrime_, 2.) + pow(g_, 2.) );

    auto lightSquarks = widthLightSquarks();
    auto sbottoms = widthSbottom();
    auto stops = widthStop();
    auto staus = widthStau();
    auto sneutrinos = widthSneutrinos();
    auto lightSleptons = widthLightLeptons();
    auto neutralinosZboson = widthNeutralinoZBoson();
    auto neutralinosPhoton = widthNeutralinoPhoton();
    auto charginosWboson = widthCharginoWBoson();
    auto neutralinoHiggs = widthNeutralinoHiggs();
    auto charginoHiggs = widthCharginoHiggs();
    Decays.insert(Decays.end(), lightSquarks.begin(), lightSquarks.end());
    Decays.insert(Decays.end(), sbottoms.begin(), sbottoms.end());
    Decays.insert(Decays.end(), stops.begin(), stops.end());
    Decays.insert(Decays.end(), staus.begin(), staus.end());
    Decays.insert(Decays.end(), sneutrinos.begin(), sneutrinos.end());
    Decays.insert(Decays.end(), lightSleptons.begin(), lightSleptons.end());
    Decays.insert(Decays.end(), neutralinosZboson.begin(), neutralinosZboson.end());
    Decays.insert(Decays.end(), neutralinosPhoton.begin(), neutralinosPhoton.end());
    Decays.insert(Decays.end(), charginosWboson.begin(), charginosWboson.end());
    Decays.insert(Decays.end(), neutralinoHiggs.begin(), neutralinoHiggs.end());
    Decays.insert(Decays.end(), charginoHiggs.begin(), charginoHiggs.end());
    Decays.push_back(widthGluino());
    Decays.push_back(widthAxinoAxion());
    Decays.push_back(widthAxinoSaxion());
}

GravitinoWidths::~GravitinoWidths(){}

double GravitinoWidths::gscl(double mFermion, double mScalar, double cLLRR, double cLRRL){
    double mGravitino = parent_.Mass;
    double pq = ( pow(mGravitino, 2.) + pow(mScalar, 2.) - pow(mFermion, 2.) ) / 2.;
    double pqL = ( pow(mGravitino, 2.) - pow(mScalar, 2.) + pow(mFermion, 2.) ) / 2.;
    double qqL = ( pow(mGravitino, 2.) - pow(mScalar, 2.) - pow(mFermion, 2.) ) / 2.;

    double width = 0.;
    if (abs(mGravitino) > (abs(mScalar) + abs(mFermion))){
        double mSqr = (1./3.) * cLLRR * ( 
            pow(pq / mGravitino, 2.) - pow(mScalar, 2.) 
        ) * pqL 
        + (1./3.) * cLRRL * ( 
            pow(pq / mGravitino, 2.) - pow(mScalar, 2.) 
        ) * mGravitino * mFermion;
        double bf = (1./pow(mGravitino, 2.)) * sqrt( pow(mGravitino, 4.) - 2. * pow(mGravitino, 2.) * ( pow(mScalar, 2.) + pow(mFermion, 2.) ) + pow( pow(mFermion, 2.) - pow(mScalar, 2.), 2.));
        width = bf * mSqr / ( 16. * M_PI * mGravitino * pow(model_.Constants.mPlanck, 2.) );
        if (width < 0.){
            throw logic_error("asdf");
        }
    }
    return width;
}

double GravitinoWidths::gvec(double mFermion, double mVector, double cgg1, double cgg2, double cgh1, double cgh2, double chh1, double chh2){
    double mGravitino = parent_.Mass;
    double pq = ( pow(mGravitino, 2.) + pow(mVector, 2.) - pow(mFermion, 2.) ) / 2.;
    double pqL = ( pow(mGravitino, 2.) - pow(mVector, 2.) + pow(mFermion, 2.) ) / 2.;
    double qqL = ( pow(mGravitino, 2.) - pow(mVector, 2.) - pow(mFermion, 2.) ) / 2.;

    double width = 0.;    
    if (abs(mGravitino) > (abs(mVector) + abs(mFermion))){
        double mSqr = (2./3.) * cgg1 
            * ( 
                pow(pq, 2.) * pqL / pow(mGravitino, 2.) + pq * qqL - 0.5 * pow(mVector, 2.) * pqL 
            ) 
        - cgg2 * mGravitino * mFermion * pow(mVector, 2.) 
        + (2./3.) * cgh1 * mGravitino *( 0.5 * qqL + pq * pqL / pow(mGravitino, 2.) ) 
        + cgh2 * mFermion * pq;
        if (abs(mVector) > 0.){
            mSqr += (2./3.) * chh1 
                * ( 
                    1. + 0.5 * pow(pq / ( mGravitino * mVector ), 2.)
                ) * pqL 
                + (2./3.) * chh2 
                * ( 
                    1. + 0.5 * pow(pq / ( mGravitino * mVector ), 2.)
                ) * mGravitino * mFermion;
        }
        double bf = (1. / pow(mGravitino, 2.) ) 
        * sqrt( 
            pow(mGravitino, 4.) - 2. * pow(mGravitino, 2.) * ( pow(mVector, 2.) +  pow(mFermion, 2.) ) + pow( pow(mFermion, 2.) -  pow(mVector, 2.), 2.) 
        );
        width = bf * mSqr / (16. * M_PI * mGravitino * pow(model_.Constants.mPlanck, 2.) );
        if (width < 0.){
            throw logic_error("asdf");
        }
    }
    return width;
}

Models::PartialWidth GravitinoWidths::widthAxinoAxion(){
    double width = 0.;
    auto axino = ModelBaseOps::FindConst(particles_, "axino");
    auto axion = ModelBaseOps::FindConst(particles_, "axion");
    if (abs(axino.Mass) > 0.){
        width = gscl(axino.Mass, axion.Mass, 2., 0.);
    }
    return Models::PartialWidth(parent_, { axino, axion }, width);
}

Models::PartialWidth GravitinoWidths::widthAxinoSaxion(){
    double width = 0.;
    auto axino = ModelBaseOps::FindConst(particles_, "axino");
    auto saxion = ModelBaseOps::FindConst(particles_, "saxion");
    if (abs(axino.Mass) > 0. && abs(saxion.Mass) > 0.){
        width = gscl(axino.Mass, saxion.Mass, 2., 0. );
    }
    return Models::PartialWidth(parent_, { axino, saxion }, width);
}

Models::PartialWidth GravitinoWidths::widthGluino(){
    auto gluino = ModelBaseOps::FindConst(particles_, "gluino");
    double width = 8. * gvec(abs(gluino.Mass), 0., 2., 0., 0., 0., 0., 0. );
    return Models::PartialWidth(parent_, { gluino, ModelBaseOps::FindConst(particles_, "gluon") }, width);
}

double GravitinoWidths::widthNeutralinoHH(double mNeutralino, double mHH, double alpha, double zMix0, double zMix1){
    double cc1 = 2. * ( pow( zMix1 * cos( alpha ), 2. ) + pow( zMix0 * sin( alpha ), 2. ) - sin( 2. * alpha ) * zMix0 * zMix1 );
    double cc2 = copysign(1., mNeutralino) * cc1;

    return gscl(abs(mNeutralino), mHH, cc1, cc2);
}

double GravitinoWidths::widthNeutralinoHl(double mNeutralino, double mHl, double alpha, double zMix0, double zMix1){
    double cc1 = 2. * ( pow( zMix0 * cos( alpha ), 2. ) + pow( zMix1 * sin( alpha ), 2. ) + sin( 2. * alpha ) * zMix0 * zMix1 );
    double cc2 = copysign(1., mNeutralino) * cc1;

    return gscl(abs(mNeutralino), mHl, cc1, cc2);
}

double GravitinoWidths::widthNeutralinoHA(double mNeutralino, double mHA, double beta, double zMix0, double zMix1){
    double cc1 = 2. * ( pow( zMix0 * cos( beta ), 2. ) + pow( zMix1 * sin( beta ), 2. ) + sin( 2. * beta ) * zMix0 * zMix1 );
    double cc2 = -copysign(1., mNeutralino) * cc1;

    return gscl(abs(mNeutralino), mHA, cc1, cc2);
}

deque< Models::PartialWidth > GravitinoWidths::widthCharginoHiggs(){
    auto higgsCharged = ModelBaseOps::FindConst(particles_, "higgscharged");
    auto chargino1 = ModelBaseOps::FindConst(particles_, "chargino1");
    auto chargino2 = ModelBaseOps::FindConst(particles_, "chargino2");

    double thX = copysign(1., 1./tan(model_.Susy.MixingAngles.gammaL));
    double thY = copysign(1., 1./tan(model_.Susy.MixingAngles.gammaR));
    double beta = atan(model_.Susy.TanBeta);

    deque< Models::PartialWidth > widths;
    double cc11 = 2. * ( pow( sin( beta ) * cos( model_.Susy.MixingAngles.gammaL ), 2. ) + pow( cos( beta ) * cos( model_.Susy.MixingAngles.gammaR ), 2. ) );
    double cc21 = - copysign(1., chargino1.Mass) * 4. * thX * sin( beta ) * cos( beta ) * cos( model_.Susy.MixingAngles.gammaL ) * cos( model_.Susy.MixingAngles.gammaR );
    widths.push_back( Models::PartialWidth(parent_, { chargino1, higgsCharged }, 2. * gscl(chargino1.Mass, higgsCharged.Mass, cc11, cc21)) );

    double cc12 = 2. * ( pow( sin( beta ) * sin( model_.Susy.MixingAngles.gammaL ), 2. ) + pow( cos( beta ) * sin( model_.Susy.MixingAngles.gammaR ), 2. ) );
    double cc22 = copysign(1., chargino2.Mass) * 4. * thY * sin( beta ) * cos( beta ) * sin( model_.Susy.MixingAngles.gammaL ) * sin( model_.Susy.MixingAngles.gammaR );
    widths.push_back( Models::PartialWidth(parent_, { chargino2, higgsCharged }, 2. * gscl(chargino2.Mass, higgsCharged.Mass, cc12, cc22)) );
    return widths;
}

deque< Models::PartialWidth > GravitinoWidths::widthNeutralinoHiggs(){
    auto higgsLight = ModelBaseOps::FindConst(particles_, "higgslight");
    auto higgsHeavy = ModelBaseOps::FindConst(particles_, "higgsheavy");
    auto higgsPseudo = ModelBaseOps::FindConst(particles_, "higgspseudoscalar");
    auto neutralino1 = ModelBaseOps::FindConst(particles_, "neutralino1");
    auto neutralino2 = ModelBaseOps::FindConst(particles_, "neutralino2");
    auto neutralino3 = ModelBaseOps::FindConst(particles_, "neutralino3");
    auto neutralino4 = ModelBaseOps::FindConst(particles_, "neutralino4");

    double alpha = model_.Susy.MixingAngles.alfah;
    double beta = atan( model_.Susy.TanBeta );
    deque<double> neutralinoMasses = { neutralino1.Mass, neutralino2.Mass, neutralino3.Mass, neutralino4.Mass };
    deque<double> widthsHl, widthsHH, widthsHA;
    for (int i=0; i<4; ++i){
        widthsHl.push_back( widthNeutralinoHl( neutralinoMasses[i], higgsLight.Mass, alpha, model_.Susy.MixingAngles.zMix[i][0], model_.Susy.MixingAngles.zMix[i][1]) );
        widthsHH.push_back( widthNeutralinoHH( neutralinoMasses[i], higgsHeavy.Mass, alpha, model_.Susy.MixingAngles.zMix[i][0], model_.Susy.MixingAngles.zMix[i][1]) );
        widthsHA.push_back( widthNeutralinoHA( neutralinoMasses[i], higgsPseudo.Mass, beta, model_.Susy.MixingAngles.zMix[i][0], model_.Susy.MixingAngles.zMix[i][1]) );
    }
    deque< Models::PartialWidth > widths;
    widths.push_back( Models::PartialWidth(parent_, { neutralino1, higgsLight }, widthsHl[0] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino2, higgsLight }, widthsHl[1] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino3, higgsLight }, widthsHl[2] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino4, higgsLight }, widthsHl[3] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino1, higgsHeavy }, widthsHH[0] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino2, higgsHeavy }, widthsHH[1] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino3, higgsHeavy }, widthsHH[2] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino4, higgsHeavy }, widthsHH[3] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino1, higgsPseudo }, widthsHA[0] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino2, higgsPseudo }, widthsHA[1] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino3, higgsPseudo }, widthsHA[2] ) );
    widths.push_back( Models::PartialWidth(parent_, { neutralino4, higgsPseudo }, widthsHA[3] ) );
    return widths;
}


double GravitinoWidths::widthNeutralinoZBoson(double mNeutralino, double mZ, double zMix0, double zMix1, double zMix2, double zMix3, double vuq, double vdq){
    double cgg1 = 2. * ( pow( g_ * zMix2, 2. ) + pow( gPrime_ * zMix3, 2. ) - 2. * g_ * gPrime_ * zMix2 * zMix3 ) / pow( gZ_, 2. );
    double cgg2 = copysign(1., mNeutralino) * cgg1;

    double cgh1 = 2. * sqrt(2.) * ( 
        vuq * g_ * zMix0 * zMix2 
        - vuq * gPrime_ * zMix0 * zMix3 
        - vdq * g_ * zMix1 * zMix2 
        + vdq * gPrime_ * zMix1 * zMix3 
    );
    double cgh2 = - copysign(1., mNeutralino) * cgh1;

    double chh1 = pow( gZ_, 2. ) * ( pow( vuq * zMix0, 2. ) + pow( vdq * zMix1, 2. ) - 2. * vuq * vdq * zMix0 * zMix1 );
    double chh2 = - copysign(1., mNeutralino) * chh1;

    return gvec(abs(mNeutralino), mZ, cgg1, cgg2, cgh1, cgh2, chh1, chh2);
}

deque< Models::PartialWidth > GravitinoWidths::widthNeutralinoZBoson(){
    auto neutralino1 = ModelBaseOps::FindConst(particles_, "neutralino1");
    auto neutralino2 = ModelBaseOps::FindConst(particles_, "neutralino2");
    auto neutralino3 = ModelBaseOps::FindConst(particles_, "neutralino3");
    auto neutralino4 = ModelBaseOps::FindConst(particles_, "neutralino4");
    auto zBoson = ModelBaseOps::FindConst(particles_, "zboson");
    
    deque< Models::PartialWidth > neutralinoWidths;
    deque<double> widthVals;
    deque<double> neutralinoMasses = { neutralino1.Mass, neutralino2.Mass, neutralino3.Mass, neutralino4.Mass };
    for (int i=0; i<4; ++i){
        widthVals.push_back( widthNeutralinoZBoson( neutralinoMasses[i], zBoson.Mass, model_.Susy.MixingAngles.zMix[i][0], model_.Susy.MixingAngles.zMix[i][1], model_.Susy.MixingAngles.zMix[i][2], model_.Susy.MixingAngles.zMix[i][3], model_.Susy.Couplings.vuq, model_.Susy.Couplings.vdq) );
    }
    neutralinoWidths.push_back( Models::PartialWidth(parent_, { neutralino1, zBoson }, widthVals[0] ) );
    neutralinoWidths.push_back( Models::PartialWidth(parent_, { neutralino2, zBoson }, widthVals[1] ) );
    neutralinoWidths.push_back( Models::PartialWidth(parent_, { neutralino3, zBoson }, widthVals[2] ) );
    neutralinoWidths.push_back( Models::PartialWidth(parent_, { neutralino4, zBoson }, widthVals[3] ) );
    return neutralinoWidths;
}

double GravitinoWidths::widthNeutralinoPhoton(double mNeutralino, double mPhoton, double zMix0, double zMix1, double zMix2, double zMix3, double vuq, double vdq){
    double cgg1 = 2. * ( pow( g_ * zMix3, 2. ) + pow( gPrime_ * zMix2, 2. ) + 2. * g_ * gPrime_ * zMix2 * zMix3 ) / pow( gZ_, 2. );
    // cgg2 is technically non-zero, but multiplies photon mass, so effective width is 0
    return gvec(abs(mNeutralino), mPhoton, cgg1, 0., 0., 0., 0., 0.);
}

deque< Models::PartialWidth > GravitinoWidths::widthNeutralinoPhoton(){
    auto neutralino1 = ModelBaseOps::FindConst(particles_, "neutralino1");
    auto neutralino2 = ModelBaseOps::FindConst(particles_, "neutralino2");
    auto neutralino3 = ModelBaseOps::FindConst(particles_, "neutralino3");
    auto neutralino4 = ModelBaseOps::FindConst(particles_, "neutralino4");

    deque< Models::PartialWidth > neutralinoWidths;
    deque<double> widthVals;
    deque<double> neutralinoMasses = { neutralino1.Mass, neutralino2.Mass, neutralino3.Mass, neutralino4.Mass };
    for (int i=0; i<4; ++i){
        widthVals.push_back( widthNeutralinoPhoton( neutralinoMasses[i], 0., model_.Susy.MixingAngles.zMix[i][0], model_.Susy.MixingAngles.zMix[i][1], model_.Susy.MixingAngles.zMix[i][2], model_.Susy.MixingAngles.zMix[i][3], model_.Susy.Couplings.vuq, model_.Susy.Couplings.vdq) );
    }
    neutralinoWidths.push_back( Models::PartialWidth(parent_, { neutralino1, ModelBaseOps::FindConst(particles_, "photon") }, widthVals[0] ) );
    neutralinoWidths.push_back( Models::PartialWidth(parent_, { neutralino2, ModelBaseOps::FindConst(particles_, "photon") }, widthVals[1] ) );
    neutralinoWidths.push_back( Models::PartialWidth(parent_, { neutralino3, ModelBaseOps::FindConst(particles_, "photon") }, widthVals[2] ) );
    neutralinoWidths.push_back( Models::PartialWidth(parent_, { neutralino4, ModelBaseOps::FindConst(particles_, "photon") }, widthVals[3] ) );
    return neutralinoWidths;
}

deque< Models::PartialWidth > GravitinoWidths::widthCharginoWBoson(){
    auto chargino1 = ModelBaseOps::FindConst(particles_, "chargino1");
    auto chargino2 = ModelBaseOps::FindConst(particles_, "chargino2");
    auto wBoson = ModelBaseOps::FindConst(particles_, "wboson");
    
    deque< Models::PartialWidth > widths;
    double THX = copysign(1., 1./tan(model_.Susy.MixingAngles.gammaL ));
    double THY = copysign(1., 1./tan(model_.Susy.MixingAngles.gammaR ));
    double gammaR = model_.Susy.MixingAngles.gammaR;
    double gammaL = model_.Susy.MixingAngles.gammaL;
    double vu = model_.Susy.Couplings.vuq;
    double vd = model_.Susy.Couplings.vdq;

    double cgg11 = pow( sin( gammaL ), 2. ) + pow( sin( gammaR ), 2. );
    double cgg21 = copysign(1., chargino1.Mass) * 2. * THX * sin( gammaL ) * sin( gammaR );
    double cgh11 = 2. * g_ * ( vu * sin( gammaR ) * cos( gammaR ) - vd * sin( gammaL ) * cos( gammaL ) );
    double cgh21 = copysign(1., chargino1.Mass) * 2. * g_ * THX * ( vd * cos( gammaL ) * sin( gammaR ) - vu * sin( gammaL ) * cos( gammaR ) );
    double chh11 = pow( g_, 2. ) * ( pow( vd * cos( gammaL ), 2. ) + pow( vu * cos( gammaR ), 2. ) );
    double chh21 = copysign(1., chargino1.Mass) * 2. * pow( g_, 2. ) * vu * vd * THX * cos( gammaL ) * cos( gammaR );

    widths.push_back( Models::PartialWidth(parent_, { chargino1, wBoson }, 2. * gvec(abs(chargino1.Mass), wBoson.Mass, cgg11, cgg21, cgh11, cgh21, chh11, chh21) ) );

    double cgg12 = pow( cos( gammaL ), 2. ) + pow( cos( gammaR ), 2. );
    double cgg22 = - copysign(1., chargino2.Mass) * 2. * THY * cos( gammaL ) * cos( gammaR );
    double cgh12 = - 2. * g_ * ( vu * sin( gammaR ) * cos( gammaR ) - vd * sin( gammaL ) * cos( gammaL ) );
    double cgh22 = copysign(1., chargino2.Mass) * 2. * g_ * THY * ( vd * sin( gammaL ) * cos( gammaR ) - vu * cos( gammaL ) * sin( gammaR ) );
    double chh12 = pow( g_, 2. ) * ( pow( vd * sin( gammaL ), 2. ) + pow( vu * sin( gammaR ), 2. ) );
    double chh22 = - copysign(1., chargino2.Mass) * 2. * pow( g_, 2. ) * vu * vd * THY * sin( gammaL ) * sin( gammaR );

    // multiply the overall width by 2 since have W+ chi- and W- chi+ final states
    widths.push_back( Models::PartialWidth(parent_, { chargino2, wBoson }, 2. * gvec(abs(chargino2.Mass), wBoson.Mass, cgg12, cgg22, cgh12, cgh22, chh12, chh22) ) );
    return widths;
}

deque< Models::PartialWidth > GravitinoWidths::widthLightSquarks(){
    auto upl = ModelBaseOps::FindConst(particles_, "suplsq");
    auto upr = ModelBaseOps::FindConst(particles_, "suprsq");
    auto dnl = ModelBaseOps::FindConst(particles_, "sdownlsq");
    auto dnr = ModelBaseOps::FindConst(particles_, "sdownrsq");
    auto stl = ModelBaseOps::FindConst(particles_, "sstrangelsq");
    auto str = ModelBaseOps::FindConst(particles_, "sstrangersq");
    auto chl = ModelBaseOps::FindConst(particles_, "scharmlsq");
    auto chr = ModelBaseOps::FindConst(particles_, "scharmrsq");


    deque< Models::PartialWidth > widths;
    widths.push_back( Models::PartialWidth(parent_, { upl, ModelBaseOps::FindConst(particles_, "upq") }, 3. * gscl(0., abs(upl.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { upr, ModelBaseOps::FindConst(particles_, "upq") }, 3. * gscl(0., abs(upr.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { dnl, ModelBaseOps::FindConst(particles_, "downq") }, 3. * gscl(0., abs(dnl.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { dnr, ModelBaseOps::FindConst(particles_, "downq") }, 3. * gscl(0., abs(dnr.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { stl, ModelBaseOps::FindConst(particles_, "strangeq") }, 3. * gscl(0., abs(stl.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { str, ModelBaseOps::FindConst(particles_, "strangeq") }, 3. * gscl(0., abs(str.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { chl, ModelBaseOps::FindConst(particles_, "charmq") }, 3. * gscl(0., abs(chl.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { chr, ModelBaseOps::FindConst(particles_, "charmq") }, 3. * gscl(0., abs(chr.Mass), 2., 0.) ) );
    return widths;
}

deque< Models::PartialWidth > GravitinoWidths::widthSbottom(){
    auto sb1 = ModelBaseOps::FindConst(particles_, "sbottom1sq");
    auto sb2 = ModelBaseOps::FindConst(particles_, "sbottom2sq");

    double cc11 = 2.;
    double cc21 = -2. * sin(2. * model_.Susy.MixingAngles.thetaB);
    double cc12 = 2.;
    double cc22 = 2. * sin(2. * model_.Susy.MixingAngles.thetaB);
    
    deque< Models::PartialWidth > widths;
    widths.push_back( Models::PartialWidth(parent_, { sb1, ModelBaseOps::FindConst(particles_, "bottomq") }, 3. * gscl(4.7, abs(sb1.Mass), cc11, cc21) ) );
    widths.push_back( Models::PartialWidth(parent_, { sb2, ModelBaseOps::FindConst(particles_, "bottomq") }, 3. * gscl(4.7, abs(sb2.Mass), cc12, cc22) ) );
    return widths;
}

deque< Models::PartialWidth > GravitinoWidths::widthStop(){
    auto st1 = ModelBaseOps::FindConst(particles_, "stop1sq");
    auto st2 = ModelBaseOps::FindConst(particles_, "stop2sq");

    double cc11 = 2.;
    double cc21 = -2. * sin(2. * model_.Susy.MixingAngles.thetaT);
    double cc12 = 2.;
    double cc22 = 2. * sin(2. * model_.Susy.MixingAngles.thetaT);
    
    deque< Models::PartialWidth > widths;
    widths.push_back( Models::PartialWidth(parent_, { st1, ModelBaseOps::FindConst(particles_, "topq") }, 3. * gscl(173.1, abs(st1.Mass), cc11, cc21) ) );
    widths.push_back( Models::PartialWidth(parent_, { st2, ModelBaseOps::FindConst(particles_, "topq") }, 3. * gscl(173.1, abs(st2.Mass), cc12, cc22) ) );
    return widths;
}

deque< Models::PartialWidth > GravitinoWidths::widthStau(){
    auto tau = ModelBaseOps::FindConst(particles_, "tau");
    auto stau1 = ModelBaseOps::FindConst(particles_, "stau1");
    auto stau2 = ModelBaseOps::FindConst(particles_, "stau2");

    double cc11 = 2.;
    double cc21 = -2. * sin(2. * model_.Susy.MixingAngles.thetaL);
    double cc12 = 2.;
    double cc22 = 2. * sin(2. * model_.Susy.MixingAngles.thetaL);
    
    deque< Models::PartialWidth > widths;
    widths.push_back( Models::PartialWidth(parent_, { stau1, tau }, gscl(tau.Mass, abs(stau1.Mass), cc11, cc21) ) );
    widths.push_back( Models::PartialWidth(parent_, { stau2, tau }, gscl(tau.Mass, abs(stau2.Mass), cc12, cc22) ) );
    return widths;
}

deque< Models::PartialWidth > GravitinoWidths::widthSneutrinos(){
    auto nuel = ModelBaseOps::FindConst(particles_, "sneutrinoselectronl");
    auto numu = ModelBaseOps::FindConst(particles_, "sneutrinosmuonl");
    auto nutau = ModelBaseOps::FindConst(particles_, "sneutrinostaul");

    deque< Models::PartialWidth > widths;
    widths.push_back( Models::PartialWidth(parent_, { nuel, ModelBaseOps::FindConst(particles_, "neutrinoelectron") }, gscl(0., abs(nuel.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { numu, ModelBaseOps::FindConst(particles_, "neutrinomuon") }, gscl(0., abs(numu.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { nutau, ModelBaseOps::FindConst(particles_, "neutrinotau") }, gscl(0., abs(nutau.Mass), 2., 0.) ) );
    return widths;
}

deque< Models::PartialWidth > GravitinoWidths::widthLightLeptons(){
    auto sel = ModelBaseOps::FindConst(particles_, "selectronl");
    auto ser = ModelBaseOps::FindConst(particles_, "selectronr");
    auto smul = ModelBaseOps::FindConst(particles_, "smuonl");
    auto smur = ModelBaseOps::FindConst(particles_, "smuonr");

    deque< Models::PartialWidth > widths;
    widths.push_back( Models::PartialWidth(parent_, { sel, ModelBaseOps::FindConst(particles_, "electron") }, gscl(0., abs(sel.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { ser, ModelBaseOps::FindConst(particles_, "electron") }, gscl(0., abs(ser.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { smul, ModelBaseOps::FindConst(particles_, "muon") }, gscl(0., abs(smul.Mass), 2., 0.) ) );
    widths.push_back( Models::PartialWidth(parent_, { smur, ModelBaseOps::FindConst(particles_, "muon") }, gscl(0., abs(smur.Mass), 2., 0.) ) );
    return widths;
}
