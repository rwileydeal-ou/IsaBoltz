#include <ParticleDefs/Axino/AxinoWidthsDFSZ.h>

using namespace std;

AxinoWidthsDFSZ::AxinoWidthsDFSZ(
    const ModelBase& model, 
    const std::vector< Models::Particle >& particles
) :
    AxinoWidths(model, particles)
{
    // do any running adjustments to params here (?) and attach to model_ field variable

    for (int i = 0; i < 4; i++){
        V0i[i] = V0(i, 2.0, model_.StandardModel.vEW);
    } 

    auto fermionSfermion = widthFermionSfermionDFSZ();
    auto neutralinoHiggs = widthNeutralinoHiggsDFSZ(2.);
    auto neutralinoZ = widthNeutralinoZBosonDFSZ();
    auto charginoW = widthCharginoWBosonDFSZ();
    Decays.insert(Decays.end(), charginoW.begin(), charginoW.end());
    Decays.insert(Decays.end(), fermionSfermion.begin(), fermionSfermion.end());
    Decays.insert(Decays.end(), neutralinoHiggs.begin(), neutralinoHiggs.end());
    Decays.insert(Decays.end(), neutralinoZ.begin(), neutralinoZ.end());
    Decays.push_back(widthLightCharginoHiggsDFSZ(2.));
    Decays.push_back(widthHeavyCharginoHiggsDFSZ(2.));

}

AxinoWidthsDFSZ::~AxinoWidthsDFSZ(){}

double AxinoWidthsDFSZ::psLamb(double x, double y, double z){
    double result = pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0) - 2.0 * ( x * y + y * z + z * x);
    return result;
}

double AxinoWidthsDFSZ::V0(int neutralino, double cH, double vEW){
    auto neutralino1 = ModelBaseOps::FindConst(particles_, "neutralino1");
    auto neutralino2 = ModelBaseOps::FindConst(particles_, "neutralino2");
    auto neutralino3 = ModelBaseOps::FindConst(particles_, "neutralino3");
    auto neutralino4 = ModelBaseOps::FindConst(particles_, "neutralino4");
    
    double beta = atan(model_.Susy.TanBeta);
    double neutralinoMasses[4] = { neutralino1.Mass, neutralino2.Mass, neutralino3.Mass, neutralino4.Mass };

    // sum
    double v0 = 0.;
    for (int i = 0; i < 4; i++){
        v0 += ( cH * model_.Susy.Couplings.mu * vEW / model_.PQSector.Potential.vPQ ) * model_.Susy.MixingAngles.zMix[i][neutralino] * ( model_.Susy.MixingAngles.zMix[i][0] * cos(beta) + model_.Susy.MixingAngles.zMix[i][1] * sin(beta) )/( parent_.Mass - neutralinoMasses[i] );
    }
    return v0;
}

boost::numeric::ublas::vector<double> AxinoWidthsDFSZ::TAxinoNeutralinoScalar(double zMix0, double zMix1, double alpha, double mAxino, double mNeutralino){
    boost::numeric::ublas::vector<double> T(2);
    T(0) = zMix0;
    T(1) = zMix1;
    T = copysign(1., mAxino) * copysign(1., mNeutralino) * prod( prod( boost::numeric::ublas::real( Pauli::PauliX() ), Rotations::Rotate2d(alpha) ), T );
    return T;
}

double AxinoWidthsDFSZ::widthNeutralinoHiggsHLDFSZ(double mNeutralino, double mAxino, double mHL, double alpha, double zMix0, double zMix1, double zMix2, double zMix3, double gU1, double gSU2, double vPQ, double mu, double cH){
    double axnhl = 0.0;
    if (abs(mAxino) > (abs(mNeutralino) + abs(mHL))){
        double sgnAxino = copysign(1., mAxino);
        double sgnNeutralino = copysign(1., mNeutralino);
        double m_zNReducedSqr = pow(mNeutralino, 2.0) / pow(mAxino, 2.0);
        double mHLReducedSqr = pow(mHL, 2.0) / pow(mAxino, 2.0);

        double xcHL1 = (-1./2.) * sgnNeutralino * sgnAxino * ( zMix1 * sin(alpha) - zMix0 * cos(alpha) ) * ( gSU2 * V0i[2] - gU1 * V0i[3] );
        double xcHL2 = (-1./2.) * sgnNeutralino * sgnAxino * ( V0i[1] * sin(alpha) - V0i[0] * cos(alpha) ) * ( gSU2 * zMix2 - gU1 * zMix3 );
        double TaZhl = TAxinoNeutralinoScalar(zMix0, zMix1, alpha, mAxino, mNeutralino)[0];
        double LMBaZhl = xcHL1 + xcHL2 - cH * mu * TaZhl / (sqrt(2.0) * vPQ);
        axnhl = ( 1.0 / ( 16.0 * M_PI ) ) * pow(LMBaZhl, 2.0) * abs(mAxino) * sqrt( psLamb( 1.0, m_zNReducedSqr, mHLReducedSqr ) ) * ( ( 1.0 + m_zNReducedSqr - mHLReducedSqr ) + 2.0 * mNeutralino / mAxino );
    }
    return axnhl;
}

double AxinoWidthsDFSZ::widthNeutralinoHiggsHHDFSZ(double mNeutralino, double mAxino, double mHH, double alpha, double zMix0, double zMix1, double zMix2, double zMix3, double gU1, double gSU2, double vPQ, double mu, double cH){
    double axnhh = 0.0;
    if (abs(mAxino) > (abs(mNeutralino) + abs(mHH))){
        double sgnAxino = copysign(1., mAxino);
        double sgnNeutralino = copysign(1., mNeutralino);
        double m_zNReducedSqr = pow(mNeutralino, 2.0) / pow(mAxino, 2.0);
        double mHHReducedSqr = pow(mHH, 2.0) / pow(mAxino, 2.0);

        double xcHH1 = (-1./2.) * sgnNeutralino * sgnAxino * ( zMix1 * cos(alpha) + zMix0 * sin(alpha) ) * ( gSU2 * V0i[2] - gU1 * V0i[3] );
        double xcHH2 = (-1./2.) * sgnNeutralino * sgnAxino * ( V0i[1] * cos(alpha) + V0i[0] * sin(alpha) ) * ( gSU2 * zMix2 - gU1 * zMix3 );
        double TaZhh = TAxinoNeutralinoScalar(zMix0, zMix1, alpha, mAxino, mNeutralino)[1];
        double LMBaZhh = xcHH1 + xcHH2 - cH * mu * TaZhh / (sqrt(2.0) * vPQ);
        axnhh = ( 1.0 / ( 16.0 * M_PI ) ) * pow(LMBaZhh, 2.0) * abs(mAxino) * sqrt( psLamb( 1.0, m_zNReducedSqr, mHHReducedSqr ) ) * ( ( 1.0 + m_zNReducedSqr - mHHReducedSqr ) + 2.0 * mNeutralino / mAxino );
    }
    return axnhh;
}

double AxinoWidthsDFSZ::widthNeutralinoHiggsHADFSZ(double mNeutralino, double mAxino, double mHA, double alpha, double beta, double zMix0, double zMix1, double zMix2, double zMix3, double gU1, double gSU2, double vPQ, double mu, double cH){
    double axnha = 0.0;
    if (abs(mAxino) > (abs(mNeutralino) + abs(mHA))){
        double sgnAxino = copysign(1., mAxino);
        double sgnNeutralino = copysign(1., mNeutralino);
        double m_zNReducedSqr = pow(mNeutralino, 2.0) / pow(mAxino, 2.0);
        double mHAReducedSqr = pow(mHA, 2.0) / pow(mAxino, 2.0);

        double xcA1 = (1./2.) * sgnNeutralino * sgnAxino * ( zMix1 * sin(beta) - zMix0 * cos(beta) ) * ( gSU2 * V0i[2] - gU1 * V0i[3] );
        double xcA2 = (1./2.) * sgnNeutralino * sgnAxino * ( V0i[1] * sin(beta) - V0i[0] * cos(beta) ) * ( gSU2 * zMix2 - gU1 * zMix3 );
        // TaZG????
        double TaZa = -sgnNeutralino * sgnAxino * ( -zMix0 * sin(beta) + zMix1 * cos(beta) );
        double LMBaZa = xcA1 + xcA2 - cH * mu * TaZa / (sqrt(2.0) * vPQ);
        // extra minus sign?
        axnha = ( 1.0 / ( 16.0 * M_PI ) ) * pow(LMBaZa, 2.0) * abs(mAxino) * sqrt( psLamb( 1.0, m_zNReducedSqr, mHAReducedSqr ) ) * ( ( 1.0 + m_zNReducedSqr - mHAReducedSqr ) - 2.0 * mNeutralino / mAxino );
    }
    return axnha;
}

vector< Models::PartialWidth > AxinoWidthsDFSZ::widthNeutralinoHiggsDFSZ(double cH){
    auto higgsLight = ModelBaseOps::FindConst(particles_, "higgslight");
    auto higgsHeavy = ModelBaseOps::FindConst(particles_, "higgsheavy");
    auto higgsPseudo = ModelBaseOps::FindConst(particles_, "higgspseudoscalar");
    auto neutralino1 = ModelBaseOps::FindConst(particles_, "neutralino1");
    auto neutralino2 = ModelBaseOps::FindConst(particles_, "neutralino2");
    auto neutralino3 = ModelBaseOps::FindConst(particles_, "neutralino3");
    auto neutralino4 = ModelBaseOps::FindConst(particles_, "neutralino4");
    vector< Models::PartialWidth > higgses;
    double beta = atan(model_.Susy.TanBeta);

    double widthHL1 = widthNeutralinoHiggsHLDFSZ(neutralino1.Mass, parent_.Mass, higgsLight.Mass, model_.Susy.MixingAngles.alfah, model_.Susy.MixingAngles.zMix[0][0], model_.Susy.MixingAngles.zMix[0][1], model_.Susy.MixingAngles.zMix[0][2], model_.Susy.MixingAngles.zMix[0][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);
    double widthHL2 = widthNeutralinoHiggsHLDFSZ(neutralino2.Mass, parent_.Mass, higgsLight.Mass, model_.Susy.MixingAngles.alfah, model_.Susy.MixingAngles.zMix[1][0], model_.Susy.MixingAngles.zMix[1][1], model_.Susy.MixingAngles.zMix[1][2], model_.Susy.MixingAngles.zMix[1][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);
    double widthHL3 = widthNeutralinoHiggsHLDFSZ(neutralino3.Mass, parent_.Mass, higgsLight.Mass, model_.Susy.MixingAngles.alfah, model_.Susy.MixingAngles.zMix[2][0], model_.Susy.MixingAngles.zMix[2][1], model_.Susy.MixingAngles.zMix[2][2], model_.Susy.MixingAngles.zMix[2][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);
    double widthHL4 = widthNeutralinoHiggsHLDFSZ(neutralino4.Mass, parent_.Mass, higgsLight.Mass, model_.Susy.MixingAngles.alfah, model_.Susy.MixingAngles.zMix[3][0], model_.Susy.MixingAngles.zMix[3][1], model_.Susy.MixingAngles.zMix[3][2], model_.Susy.MixingAngles.zMix[3][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);

    double widthHH1 = widthNeutralinoHiggsHHDFSZ(neutralino1.Mass, parent_.Mass, higgsHeavy.Mass, model_.Susy.MixingAngles.alfah, model_.Susy.MixingAngles.zMix[0][0], model_.Susy.MixingAngles.zMix[0][1], model_.Susy.MixingAngles.zMix[0][2], model_.Susy.MixingAngles.zMix[0][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);
    double widthHH2 = widthNeutralinoHiggsHHDFSZ(neutralino2.Mass, parent_.Mass, higgsHeavy.Mass, model_.Susy.MixingAngles.alfah, model_.Susy.MixingAngles.zMix[1][0], model_.Susy.MixingAngles.zMix[1][1], model_.Susy.MixingAngles.zMix[1][2], model_.Susy.MixingAngles.zMix[1][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);
    double widthHH3 = widthNeutralinoHiggsHHDFSZ(neutralino3.Mass, parent_.Mass, higgsHeavy.Mass, model_.Susy.MixingAngles.alfah, model_.Susy.MixingAngles.zMix[2][0], model_.Susy.MixingAngles.zMix[2][1], model_.Susy.MixingAngles.zMix[2][2], model_.Susy.MixingAngles.zMix[2][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);
    double widthHH4 = widthNeutralinoHiggsHHDFSZ(neutralino4.Mass, parent_.Mass, higgsHeavy.Mass, model_.Susy.MixingAngles.alfah, model_.Susy.MixingAngles.zMix[3][0], model_.Susy.MixingAngles.zMix[3][1], model_.Susy.MixingAngles.zMix[3][2], model_.Susy.MixingAngles.zMix[3][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);

    double widthHA1 = widthNeutralinoHiggsHADFSZ(neutralino1.Mass, parent_.Mass, higgsPseudo.Mass, model_.Susy.MixingAngles.alfah, beta, model_.Susy.MixingAngles.zMix[0][0], model_.Susy.MixingAngles.zMix[0][1], model_.Susy.MixingAngles.zMix[0][2], model_.Susy.MixingAngles.zMix[0][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);
    double widthHA2 = widthNeutralinoHiggsHADFSZ(neutralino2.Mass, parent_.Mass, higgsPseudo.Mass, model_.Susy.MixingAngles.alfah, beta, model_.Susy.MixingAngles.zMix[1][0], model_.Susy.MixingAngles.zMix[1][1], model_.Susy.MixingAngles.zMix[1][2], model_.Susy.MixingAngles.zMix[1][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);
    double widthHA3 = widthNeutralinoHiggsHADFSZ(neutralino3.Mass, parent_.Mass, higgsPseudo.Mass, model_.Susy.MixingAngles.alfah, beta, model_.Susy.MixingAngles.zMix[2][0], model_.Susy.MixingAngles.zMix[2][1], model_.Susy.MixingAngles.zMix[2][2], model_.Susy.MixingAngles.zMix[2][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);
    double widthHA4 = widthNeutralinoHiggsHADFSZ(neutralino4.Mass, parent_.Mass, higgsPseudo.Mass, model_.Susy.MixingAngles.alfah, beta, model_.Susy.MixingAngles.zMix[3][0], model_.Susy.MixingAngles.zMix[3][1], model_.Susy.MixingAngles.zMix[3][2], model_.Susy.MixingAngles.zMix[3][3], model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.PQSector.Potential.vPQ, model_.Susy.Couplings.mu, cH);

    higgses.push_back( Models::PartialWidth( parent_, { neutralino1, higgsLight }, widthHL1 ) );
    higgses.push_back( Models::PartialWidth( parent_, { neutralino2, higgsLight }, widthHL2 ) );
    higgses.push_back( Models::PartialWidth( parent_, { neutralino3, higgsLight }, widthHL3 ) );
    higgses.push_back( Models::PartialWidth( parent_, { neutralino4, higgsLight }, widthHL4 ) );

    higgses.push_back( Models::PartialWidth( parent_, { neutralino1, higgsHeavy }, widthHH1 ) );
    higgses.push_back( Models::PartialWidth( parent_, { neutralino2, higgsHeavy }, widthHH2 ) );
    higgses.push_back( Models::PartialWidth( parent_, { neutralino3, higgsHeavy }, widthHH3 ) );
    higgses.push_back( Models::PartialWidth( parent_, { neutralino4, higgsHeavy }, widthHH4 ) );

    higgses.push_back( Models::PartialWidth( parent_, { neutralino1, higgsPseudo }, widthHA1 ) );
    higgses.push_back( Models::PartialWidth( parent_, { neutralino2, higgsPseudo }, widthHA2 ) );
    higgses.push_back( Models::PartialWidth( parent_, { neutralino3, higgsPseudo }, widthHA3 ) );
    higgses.push_back( Models::PartialWidth( parent_, { neutralino4, higgsPseudo }, widthHA4 ) );
    return higgses;
}

Models::PartialWidth AxinoWidthsDFSZ::widthLightCharginoHiggsDFSZ(double cH){
    auto higgsCharged = ModelBaseOps::FindConst(particles_, "higgscharged");
    auto chargino1 = ModelBaseOps::FindConst(particles_, "chargino1");
    auto chargino2 = ModelBaseOps::FindConst(particles_, "chargino2");

    double mW1ReducedSqr = pow(chargino1.Mass, 2.0) / pow(parent_.Mass, 2.0);
    double mW2ReducedSqr = pow(chargino2.Mass, 2.0) / pow(parent_.Mass, 2.0);
    double mHpReducedSqr = pow(higgsCharged.Mass, 2.0) / pow(parent_.Mass, 2.0);
    double beta = atan(model_.Susy.TanBeta);

    double axch = 0.;

    // w1ss
    if (abs(parent_.Mass) > (abs(chargino1.Mass) + abs(higgsCharged.Mass))){
        double a02 = 1.0 / sqrt(2.0) * ( model_.StandardModel.Couplings.gSU2 * V0i[2] + model_.StandardModel.Couplings.gU1 * V0i[3] ) * cos(model_.Susy.MixingAngles.gammaR) - model_.StandardModel.Couplings.gSU2 * V0i[0] * sin(model_.Susy.MixingAngles.gammaR);
        double lmb20 = a02 - copysign(1.0, parent_.Mass) * cH * model_.Susy.Couplings.mu * model_.Susy.TanBeta * cos(model_.Susy.MixingAngles.gammaR) / model_.PQSector.Potential.vPQ;
        double a04 = 1.0 / sqrt(2.0) * ( model_.StandardModel.Couplings.gSU2 * V0i[2] + model_.StandardModel.Couplings.gU1 * V0i[3] ) * cos(model_.Susy.MixingAngles.gammaL) + model_.StandardModel.Couplings.gSU2 * V0i[1] * sin(model_.Susy.MixingAngles.gammaL);
        double lmb40 = a04 + copysign(1.0, parent_.Mass) * cH * model_.Susy.Couplings.mu / model_.Susy.TanBeta * cos(model_.Susy.MixingAngles.gammaL) / model_.PQSector.Potential.vPQ;

        double a1 = 1./2. * ( copysign(1., chargino1.Mass) * cos(beta) * lmb20 - copysign(1., parent_.Mass) * sin(beta) * lmb40 );
        double b1 = 1./2. * ( copysign(1., chargino1.Mass) * cos(beta) * lmb20 + copysign(1., parent_.Mass) * sin(beta) * lmb40 );
        //The factor of two takes care of both charge combinations
        axch = 2.0 * ( 1.0 / (16.0 * M_PI ) ) * abs(parent_.Mass) * sqrt( psLamb(1.0, mW1ReducedSqr, mHpReducedSqr )) 
        * ( (pow(a1, 2.0) + pow(b1, 2.0)) * (1.0 + mW1ReducedSqr - mHpReducedSqr) + 2.0 * ( pow(a1, 2.0) - pow(b1, 2.0) ) * abs(chargino1.Mass) / abs(parent_.Mass));
    }

    auto widthW1 = Models::PartialWidth( parent_, { chargino1, higgsCharged }, axch);
    return widthW1;
}

Models::PartialWidth AxinoWidthsDFSZ::widthHeavyCharginoHiggsDFSZ(double cH){
    auto higgsCharged = ModelBaseOps::FindConst(particles_, "higgscharged");
    auto chargino1 = ModelBaseOps::FindConst(particles_, "chargino1");
    auto chargino2 = ModelBaseOps::FindConst(particles_, "chargino2");

    double mW1ReducedSqr = pow(chargino1.Mass, 2.0) / pow(parent_.Mass, 2.0);
    double mW2ReducedSqr = pow(chargino2.Mass, 2.0) / pow(parent_.Mass, 2.0);
    double mHpReducedSqr = pow(higgsCharged.Mass, 2.0) / pow(parent_.Mass, 2.0);
    double beta = atan(model_.Susy.TanBeta);

    double axch = 0.;

    // w2ss
    if (abs(parent_.Mass) > (abs(chargino2.Mass) + abs(higgsCharged.Mass))){
        double a01 = -1.0 / sqrt(2.0) * ( model_.StandardModel.Couplings.gSU2 * V0i[2] + model_.StandardModel.Couplings.gU1 * V0i[3] ) * sin(model_.Susy.MixingAngles.gammaR) - model_.StandardModel.Couplings.gSU2 * V0i[0] * cos(model_.Susy.MixingAngles.gammaR);
        double lmb10 = a01 + copysign(1.0, parent_.Mass) * cH * model_.Susy.Couplings.mu * model_.Susy.TanBeta * sin(model_.Susy.MixingAngles.gammaR) / model_.PQSector.Potential.vPQ;
        double a03 = -1.0 / sqrt(2.0) * ( model_.StandardModel.Couplings.gSU2 * V0i[2] + model_.StandardModel.Couplings.gU1 * V0i[3] ) * sin(model_.Susy.MixingAngles.gammaL) + model_.StandardModel.Couplings.gSU2 * V0i[1] * cos(model_.Susy.MixingAngles.gammaL);
        double lmb30 = a03 - copysign(1.0, parent_.Mass) * cH * model_.Susy.Couplings.mu / model_.Susy.TanBeta * sin(model_.Susy.MixingAngles.gammaL) / model_.PQSector.Potential.vPQ;
        
        double sgnGammaL = copysign(1.0, pow(tan(model_.Susy.MixingAngles.gammaL), -1.0));
        double sgnGammaR = copysign(1.0, pow(tan(model_.Susy.MixingAngles.gammaR), -1.0));

        double a2 = 1./2. * ( copysign(1., chargino2.Mass) * sgnGammaR * cos(beta) * lmb10 - copysign(1., parent_.Mass) * sgnGammaL * sin(beta) * lmb30 );
        double b2 = 1./2. * ( copysign(1., chargino2.Mass) * sgnGammaR * cos(beta) * lmb10 + copysign(1., parent_.Mass) * sgnGammaL * sin(beta) * lmb30 );
        
        axch = 2.0 * ( 1.0 / (16.0 * M_PI ) ) * abs(parent_.Mass) * sqrt( psLamb(1.0, mW2ReducedSqr, mHpReducedSqr )) 
        * ( (pow(a2, 2.0) + pow(b2, 2.0)) * (1.0 + mW2ReducedSqr - mHpReducedSqr) + 2.0 * ( pow(a2, 2.0) - pow(b2, 2.0) ) * abs(chargino2.Mass) / abs(parent_.Mass));
    }

    auto widthW2 = Models::PartialWidth( parent_, { chargino2, higgsCharged }, axch);
    return widthW2;
}

// from arXiv:1309.5365
vector< Models::PartialWidth > AxinoWidthsDFSZ::widthNeutralinoZBosonDFSZ(){
    auto neutralino1 = ModelBaseOps::FindConst(particles_, "neutralino1");
    auto neutralino2 = ModelBaseOps::FindConst(particles_, "neutralino2");
    auto neutralino3 = ModelBaseOps::FindConst(particles_, "neutralino3");
    auto neutralino4 = ModelBaseOps::FindConst(particles_, "neutralino4");
    auto zBoson = ModelBaseOps::FindConst(particles_, "zboson");
    vector< Models::PartialWidth > neutralinos;

    double width1 = widthNeutralinoZBosonDFSZ(neutralino1.Mass, parent_.Mass, zBoson.Mass, model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.Susy.MixingAngles.zMix[0][0], model_.Susy.MixingAngles.zMix[0][1]);
    double width2 = widthNeutralinoZBosonDFSZ(neutralino2.Mass, parent_.Mass, zBoson.Mass, model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.Susy.MixingAngles.zMix[1][0], model_.Susy.MixingAngles.zMix[1][1]);
    double width3 = widthNeutralinoZBosonDFSZ(neutralino3.Mass, parent_.Mass, zBoson.Mass, model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.Susy.MixingAngles.zMix[2][0], model_.Susy.MixingAngles.zMix[2][1]);
    double width4 = widthNeutralinoZBosonDFSZ(neutralino4.Mass, parent_.Mass, zBoson.Mass, model_.StandardModel.Couplings.gU1, model_.StandardModel.Couplings.gSU2, model_.Susy.MixingAngles.zMix[3][0], model_.Susy.MixingAngles.zMix[3][1]);
    neutralinos.push_back( Models::PartialWidth( parent_, { neutralino1, zBoson }, width1) );
    neutralinos.push_back( Models::PartialWidth( parent_, { neutralino2, zBoson }, width2) );
    neutralinos.push_back( Models::PartialWidth( parent_, { neutralino3, zBoson }, width3) );
    neutralinos.push_back( Models::PartialWidth( parent_, { neutralino4, zBoson }, width4) );
    return neutralinos;
}

double AxinoWidthsDFSZ::widthNeutralinoZBosonDFSZ(double mNeutralino, double mAxino, double mZ, double gU1, double gSU2, double zMix0, double zMix1){
    // local variables
    double axnz = 0.0;
    if (abs(mAxino) > (abs(mNeutralino) + mZ)){
        double mZReducedSqr = pow(mZ, 2.0) / pow(mAxino, 2.0);
        double m_zNReducedSqr = pow(mNeutralino, 2.0) / pow(mAxino, 2.0);
        double sgn = -1.0;
        if (mAxino > 0){
            sgn = 1.0;
        }
        double wio = 0.25 * sqrt(pow(gSU2, 2.0) + pow(gU1, 2.0)) * (zMix0 * V0i[0] - zMix1 * V0i[1]);

        axnz = ( 1.0 / ( 4.0 * M_PI ) ) * sgn * pow(wio, 2.0) * abs(mAxino) * sqrt( psLamb( 1.0, m_zNReducedSqr, mZReducedSqr) ) * ( ( 1.0 + m_zNReducedSqr - 2.0 * mZReducedSqr ) + pow(mZReducedSqr, -1.0) * pow( 1 - m_zNReducedSqr, 2.0 ) + 6.0 * mNeutralino / mAxino);
    }
    return axnz;
}

// from arXiv:1309.5365 Eqn A.77
double AxinoWidthsDFSZ::widthCharginoWBosonDFSZ(double mChargino, double mW, double mAxino, double gSU2, double x0, double y0){
    double axcw = 0.0;
    if (abs(mAxino) > (abs(mChargino) + mW)){
        double mWReducedSqr = pow( mW, 2.0) / pow(mAxino, 2.0);
        double m_wNReducedSqr = pow(mChargino, 2.0) / pow(mAxino, 2.0);
        axcw = (2.0 * (1.0/(16.0 * M_PI)) * abs(mAxino) * pow(gSU2, 2.0) * sqrt( psLamb( 1.0, m_wNReducedSqr, mWReducedSqr ) ) * ( ( pow(x0, 2.0) + pow( y0, 2.0 ) ) * ( ( 1 + m_wNReducedSqr - 2 * mWReducedSqr ) + pow( mWReducedSqr, -1.0 ) * pow( 1 - m_wNReducedSqr, 2.0 ) ) - 6 * ( pow(x0, 2.0) - pow( y0, 2.0 ) ) * abs(mChargino) / abs(mAxino)));
    }
    return axcw;
}

vector< Models::PartialWidth > AxinoWidthsDFSZ::widthCharginoWBosonDFSZ(){
    auto chargino1 = ModelBaseOps::FindConst(particles_, "chargino1");
    auto chargino2 = ModelBaseOps::FindConst(particles_, "chargino2");
    auto wBoson = ModelBaseOps::FindConst(particles_, "wboson");

    // local variables
    double sgnGammaL = copysign(1.0, pow(tan(model_.Susy.MixingAngles.gammaL), -1.0));
    double sgnGammaR = copysign(1.0, pow(tan(model_.Susy.MixingAngles.gammaR), -1.0));
    double sgnAxino = copysign(1.0, parent_.Mass);
    double sgnw1 = copysign(1.0, chargino1.Mass);
    double sgnw2 = copysign(1.0, chargino2.Mass);

    // couplings: from A.78 - A.81
    double x0[2] = {
        (1.0/2.0) * ( sgnw1 * sgnAxino * ( cos(model_.Susy.MixingAngles.gammaR) * V0i[0] / sqrt(2.0) + sin(model_.Susy.MixingAngles.gammaR) * V0i[2] ) - cos(model_.Susy.MixingAngles.gammaL) * V0i[1] / sqrt(2.0) + sin(model_.Susy.MixingAngles.gammaL) * V0i[2] ),
        (1.0/2.0) * ( sgnw2 * sgnAxino * sgnGammaR * ( - sin(model_.Susy.MixingAngles.gammaR) * V0i[0] / sqrt(2.0) + cos(model_.Susy.MixingAngles.gammaR) * V0i[2] ) + sgnGammaL * ( sin(model_.Susy.MixingAngles.gammaL) * V0i[1] / sqrt(2.0) + cos(model_.Susy.MixingAngles.gammaL) * V0i[2] ) )
    };

    double y0[2] = {
        (1.0/2.0) * ( - sgnw1 * sgnAxino * ( cos(model_.Susy.MixingAngles.gammaR) * V0i[0] / sqrt(2.0) + sin(model_.Susy.MixingAngles.gammaR) * V0i[2] ) - cos(model_.Susy.MixingAngles.gammaL) * V0i[1] / sqrt(2.0) + sin(model_.Susy.MixingAngles.gammaL) * V0i[2] ),
        (1.0/2.0) * ( - sgnw2 * sgnAxino * sgnGammaR * ( - sin(model_.Susy.MixingAngles.gammaR) * V0i[0] / sqrt(2.0) + cos(model_.Susy.MixingAngles.gammaR) * V0i[2] ) + sgnGammaL * ( sin(model_.Susy.MixingAngles.gammaL) * V0i[1] / sqrt(2.0) + cos(model_.Susy.MixingAngles.gammaL) * V0i[2] ) )
    };

    vector< Models::PartialWidth > charginos;
    double width1 = widthCharginoWBosonDFSZ(chargino1.Mass, wBoson.Mass, parent_.Mass, model_.StandardModel.Couplings.gSU2, x0[0], y0[0]);
    double width2 = widthCharginoWBosonDFSZ(chargino2.Mass, wBoson.Mass, parent_.Mass, model_.StandardModel.Couplings.gSU2, x0[1], y0[1]);
    charginos.push_back( Models::PartialWidth( parent_, { chargino1, wBoson }, width1 ) );
    charginos.push_back( Models::PartialWidth( parent_, { chargino2, wBoson }, width2 ) );
    return charginos;
}

// from arXiv:1309.5365
double AxinoWidthsDFSZ::widthUpFermionSfermionDFSZ(double mStop, double mTop, double mAxino, double NC, complex<double> au, complex<double> bu){
    // local variables
    double mTReducedSqr = pow( mTop, 2.0) / pow( mAxino, 2.0);
    double axfsf = 0.0;
    // calculate stau width:
    if (abs(mAxino) > (mStop + mTop)){
        double m_stNReducedSqr = pow(mStop, 2.0) / pow(mAxino, 2.0);
        axfsf = NC * abs(mAxino) / ( 16.0 * M_PI ) * sqrt( psLamb( 1.0, m_stNReducedSqr, mTReducedSqr ) ) * ( abs( au * au ) * ( pow( 1.0 + mTop / abs(mAxino), 2.0 ) - m_stNReducedSqr ) + abs( bu * bu ) * ( pow( 1.0 - mTop / abs(mAxino), 2.0 ) - m_stNReducedSqr ) );
    }
    return axfsf;
}

double AxinoWidthsDFSZ::widthDownFermionSfermionDFSZ(double mSbottom, double mBottom, double mAxino, double NC, complex<double> ad, complex<double> bd){
    // local variables
    double mBReducedSqr = pow( mBottom, 2.0) / pow( mAxino, 2.0);
    double axfsf = 0.0;
    // calculate sbottom width:
    if (abs(mAxino) > (mSbottom + mBottom)){
        double m_sbNReducedSqr = pow(mSbottom, 2.0) / pow(mAxino, 2.0);
        axfsf = NC * abs(mAxino) / ( 16.0 * M_PI ) * sqrt( psLamb( 1.0, m_sbNReducedSqr, mBReducedSqr ) ) * ( abs( ad * ad ) * ( pow( 1.0 + mBottom / abs(mAxino), 2.0 ) - m_sbNReducedSqr ) + abs( bd * bd ) * ( pow( 1.0 - mBottom / abs(mAxino), 2.0 ) - m_sbNReducedSqr ) );
    }
    return axfsf;
}

vector< Models::PartialWidth > AxinoWidthsDFSZ::widthFermionSfermionDFSZ(){
    auto sb1 = ModelBaseOps::FindConst(particles_, "sbottom1sq");
    auto sb2 = ModelBaseOps::FindConst(particles_, "sbottom2sq");
    auto st1 = ModelBaseOps::FindConst(particles_, "stop1sq");
    auto st2 = ModelBaseOps::FindConst(particles_, "stop2sq");
    complex<double> im(0.0,1.0);
    complex<double> THaxi;
    if (parent_.Mass > 0){
        THaxi = 1.0;
    }
    else{
        THaxi = im;
    }
    
    // couplings: from A.83 - A.90
    complex<double> AXU = ( -1.0 * (-THaxi) / ( -im * sqrt(2.0) ) ) * ( model_.StandardModel.Couplings.gSU2 * V0i[2] + model_.StandardModel.Couplings.gU1 / 3.0 * V0i[3] );
    complex<double> AXD = ( -1.0 * (-THaxi) / ( -im * sqrt(2.0) ) ) * ( -model_.StandardModel.Couplings.gSU2 * V0i[2] + model_.StandardModel.Couplings.gU1 / 3.0 * V0i[3] );
    complex<double> BXU = ( 4.0 / ( 3.0 * sqrt(2.0) ) * THaxi / im ) * model_.StandardModel.Couplings.gU1 * V0i[3];
    complex<double> BXD = ( -2.0 / ( 3.0 * sqrt(2.0) ) * THaxi / im ) * model_.StandardModel.Couplings.gU1 * V0i[3];

    double yTop = model_.Masses.MSusy.topMSusy * sqrt(2.0) / model_.StandardModel.vEW;
    double yBot = model_.Masses.MSusy.botMSusy * sqrt(2.0) / model_.StandardModel.vEW;

    complex<double> au[2] = {
        (1.0 / 2.0) * ( ( im * AXU - THaxi * yTop * V0i[0] ) * cos( model_.Susy.MixingAngles.thetaT ) - ( im * BXU -(-THaxi) * yTop * V0i[0] ) * sin( model_.Susy.MixingAngles.thetaT ) ),
        (1.0 / 2.0) * ( ( im * AXU - THaxi * yTop * V0i[0] ) * sin( model_.Susy.MixingAngles.thetaT ) + ( im * BXU -(-THaxi) * yTop * V0i[0] ) * cos( model_.Susy.MixingAngles.thetaT ) )
    };
    complex<double> bu[2] = {
        (1.0 / 2.0) * ( ( -im * AXU - THaxi * yTop * V0i[0] ) * cos( model_.Susy.MixingAngles.thetaT ) - ( im * BXU +(-THaxi) * yTop * V0i[0] ) * sin( model_.Susy.MixingAngles.thetaT ) ),
        (1.0 / 2.0) * ( ( -im * AXU - THaxi * yTop * V0i[0] ) * sin( model_.Susy.MixingAngles.thetaT ) + ( im * BXU +(-THaxi) * yTop * V0i[0] ) * cos( model_.Susy.MixingAngles.thetaT ) )
    };
    complex<double> ad[2] = {
        (1.0 / 2.0) * ( ( im * AXD - THaxi * yBot * V0i[1] ) * cos( model_.Susy.MixingAngles.thetaB ) - ( im * BXD -(-THaxi) * yBot * V0i[1] ) * sin( model_.Susy.MixingAngles.thetaB ) ),
        (1.0 / 2.0) * ( ( im * AXD - THaxi * yBot * V0i[1] ) * sin( model_.Susy.MixingAngles.thetaB ) + ( im * BXD -(-THaxi) * yBot * V0i[1] ) * cos( model_.Susy.MixingAngles.thetaB ) )
    };
    complex<double> bd[2] = {
        (1.0 / 2.0) * ( ( -im * AXD - THaxi * yBot * V0i[1] ) * cos( model_.Susy.MixingAngles.thetaB ) - ( im * BXD +(-THaxi) * yBot * V0i[1] ) * sin( model_.Susy.MixingAngles.thetaB ) ),
        (1.0 / 2.0) * ( ( -im * AXD - THaxi * yBot * V0i[1] ) * sin( model_.Susy.MixingAngles.thetaB ) + ( im * BXD +(-THaxi) * yBot * V0i[1] ) * cos( model_.Susy.MixingAngles.thetaB ) )
    };

    vector< Models::PartialWidth > fermionSfermions;
    double widthb1 = widthDownFermionSfermionDFSZ(sb1.Mass, model_.Masses.MSusy.botMSusy, parent_.Mass, model_.StandardModel.NC, ad[0], bd[0]);
    double widthb2 = widthDownFermionSfermionDFSZ(sb2.Mass, model_.Masses.MSusy.botMSusy, parent_.Mass, model_.StandardModel.NC, ad[1], bd[1]);
    double widtht1 = widthUpFermionSfermionDFSZ(st1.Mass, model_.Masses.MSusy.topMSusy, parent_.Mass, model_.StandardModel.NC, au[0], bu[0]);
    double widtht2 = widthUpFermionSfermionDFSZ(st2.Mass, model_.Masses.MSusy.topMSusy, parent_.Mass, model_.StandardModel.NC, au[1], bu[1]);
    
    fermionSfermions.push_back( Models::PartialWidth( parent_, { sb1, ModelBaseOps::FindConst(particles_, "bottomq") }, widthb1) );
    fermionSfermions.push_back( Models::PartialWidth( parent_, { sb2, ModelBaseOps::FindConst(particles_, "bottomq") }, widthb2) );
    fermionSfermions.push_back( Models::PartialWidth( parent_, { st1, ModelBaseOps::FindConst(particles_, "topq") }, widtht1) );
    fermionSfermions.push_back( Models::PartialWidth( parent_, { st2, ModelBaseOps::FindConst(particles_, "topq") }, widtht2) );
    return fermionSfermions;
}
