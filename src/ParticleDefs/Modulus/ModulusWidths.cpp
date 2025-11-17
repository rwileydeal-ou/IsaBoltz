#include <ParticleDefs/Modulus/ModulusWidths.h>

using namespace std;

ModulusWidths::ModulusWidths(const ModelBase& model, const std::deque< Models::Particle >& particles) 
    : model_(model),
    particles_(particles)
{

    parent_ = ModelBaseOps::FindConst(particles_, "modulus");

    auto neutralinoNeutralino = widthNeutralinos();
    auto charginoChargino = widthCharginos();
    auto vBosonvBoson = widthVBosonVBoson();
    auto diHiggs = widthHiggsHiggs();
    auto squarkSquark = widthSquarkSquark();
    auto sleptonSlepton = widthSleptonSlepton();
    auto diQuark = widthQuarkQuark();
    auto diLepton = widthLeptonLepton();
    auto diGluino = widthGluinoGluino();
    auto diAxionic = widthDiAxionic();

    Decays.insert(Decays.end(), neutralinoNeutralino.begin(), neutralinoNeutralino.end());
    Decays.insert(Decays.end(), charginoChargino.begin(), charginoChargino.end());
    Decays.insert(Decays.end(), vBosonvBoson.begin(), vBosonvBoson.end());
    Decays.insert(Decays.end(), diHiggs.begin(), diHiggs.end());
    Decays.insert(Decays.end(), squarkSquark.begin(), squarkSquark.end());
    Decays.insert(Decays.end(), sleptonSlepton.begin(), sleptonSlepton.end());
    Decays.insert(Decays.end(), diQuark.begin(), diQuark.end());
    Decays.insert(Decays.end(), diLepton.begin(), diLepton.end());
    Decays.insert(Decays.end(), diAxionic.begin(), diAxionic.end());
    Decays.push_back(diGluino);
    if ( model_.ModulusCouplings.GravitinoCouplings.LambdaG != 0. ){
        auto diGravitino = widthDiGravitino();
        Decays.push_back(diGravitino);
    }
}

ModulusWidths::~ModulusWidths(){}

Models::PartialWidth ModulusWidths::widthDiAlp(double lambdaAlp, const Models::Particle& alp, double mPlanck){
    double mAlp = alp.Mass;
    double mPhi = parent_.Mass;
    double massRatioSqr = pow( mAlp / mPhi, 2. );
    double prefactor = pow( lambdaAlp, 2. ) * pow( mPhi, 3. ) / ( 32. * M_PI * pow( mPlanck, 2. ) ) * pow( 1. - 2. * massRatioSqr, 2. );
    double width = prefactor * Utility::SqrtLambda( 1., massRatioSqr, massRatioSqr );

    return Models::PartialWidth( parent_, { alp, alp }, width );
}

Models::PartialWidth ModulusWidths::widthDiAxion(double lambdaPQ, const Models::Particle& axion, double mPlanck){
    double mAx = axion.Mass;
    double mPhi = parent_.Mass;
    double massRatioSqr = pow( mAx / mPhi, 2. );
    double prefactor = pow( lambdaPQ, 2. ) * pow( mPhi, 3. ) / ( 16. * M_PI * pow( mPlanck, 2. ) ) * pow( 1. - 2. * massRatioSqr, 2. );
    double width = prefactor * Utility::SqrtLambda( 1., massRatioSqr, massRatioSqr );

    return Models::PartialWidth( parent_, { axion, axion }, width );
}

Models::PartialWidth ModulusWidths::widthDiSaxion(double lambdaPQ, const Models::Particle& saxion, double mPlanck){
    double mSax = saxion.Mass;
    double mPhi = parent_.Mass;
    double massRatioSqr = pow( mSax / mPhi, 2. );
    double prefactor = pow( lambdaPQ, 2. ) * pow( mPhi, 3. ) / ( 16. * M_PI * pow( mPlanck, 2. ) ) * pow( 1. + 2. * massRatioSqr, 2. );
    double width = prefactor * Utility::SqrtLambda( 1., massRatioSqr, massRatioSqr );

    return Models::PartialWidth( parent_, { saxion, saxion }, width );
}

Models::PartialWidth ModulusWidths::widthDiAxino(double lambdaPQ, const Models::Particle& axino, double mPlanck){
    double mAx = axino.Mass;
    double mPhi = parent_.Mass;
    double massRatioSqr = pow( mAx / mPhi, 2. );
    double prefactor = pow( lambdaPQ, 2. ) * pow( mAx, 2. ) * mPhi / ( 2. * M_PI * pow( mPlanck, 2. ) ) * ( 1. - 4. * massRatioSqr );
    double width = prefactor * Utility::SqrtLambda( 1., massRatioSqr, massRatioSqr );

    return Models::PartialWidth( parent_, { axino, axino }, width );
}

deque< Models::PartialWidth > ModulusWidths::widthDiAxionic(){
    deque< Models::PartialWidth > widths;

    double lambdaPQ = model_.ModulusCouplings.AxionicCouplings.LambdaAx;
    if ( lambdaPQ != 0. ){
        auto axion = ModelBaseOps::FindConst(particles_, "axion");
        auto saxion = ModelBaseOps::FindConst(particles_, "saxion");
        auto axino = ModelBaseOps::FindConst(particles_, "axino");

        widths.push_back( widthDiAxion( lambdaPQ, axion, model_.Constants.mPlanck ) );
        widths.push_back( widthDiSaxion( lambdaPQ, saxion, model_.Constants.mPlanck ) );
        widths.push_back( widthDiAxino( lambdaPQ, axino, model_.Constants.mPlanck ) );
    }

    double lambdaAlp = model_.ModulusCouplings.AxionicCouplings.LambdaALP;
    if ( lambdaAlp != 0. ){
        auto alp = ModelBaseOps::FindConst(particles_, "alp");
        widths.push_back( widthDiAlp( lambdaAlp, alp, model_.Constants.mPlanck ) );
    }

    return widths;
}

Models::PartialWidth ModulusWidths::widthDiNeutralino(double lambdaEff, const Models::Particle& neutralino, double mPlanck){
    double mModulus = parent_.Mass;
    double mZ = neutralino.Mass;
    double massRatioSqr = pow( mZ / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaEff * mZ, 2. ) * mModulus / ( 8. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * (1. - 4. * massRatioSqr) * Utility::SqrtLambda( 1., massRatioSqr, massRatioSqr );

    return Models::PartialWidth( parent_, { neutralino, neutralino }, width  );
}

Models::PartialWidth ModulusWidths::widthNeutralinoINeutralinoJ(double lambdaEff, const Models::Particle& neutralinoI, const Models::Particle& neutralinoJ, double mPlanck){
    double mModulus = parent_.Mass;
    double mZi = abs(neutralinoI.Mass);
    double mZj = abs(neutralinoJ.Mass);
    double massRatioSqrI = pow( mZi / mModulus, 2. );
    double massRatioSqrJ = pow( mZj / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaEff, 2. ) * ( pow( mZi, 2. ) + pow( mZj, 2. ) ) * mModulus / ( 16. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * ( 1. - pow( ( mZj + mZi ) / mModulus, 2. ) ) * Utility::SqrtLambda( 1., massRatioSqrI, massRatioSqrJ );

    return Models::PartialWidth( parent_, { neutralinoI, neutralinoJ }, width  );
}

std::deque< Models::PartialWidth > ModulusWidths::widthNeutralinos(){
    auto neutralino1 = ModelBaseOps::FindConst(particles_, "neutralino1");
    auto neutralino2 = ModelBaseOps::FindConst(particles_, "neutralino2");
    auto neutralino3 = ModelBaseOps::FindConst(particles_, "neutralino3");
    auto neutralino4 = ModelBaseOps::FindConst(particles_, "neutralino4");

    double lambdaHu = model_.ModulusCouplings.HiggsCouplings.LambdaHu;
    double lambdaHd = model_.ModulusCouplings.HiggsCouplings.LambdaHd;
    double lambdaU1 = model_.ModulusCouplings.GaugeCouplings.LambdaU1;
    double lambdaSU2 = model_.ModulusCouplings.GaugeCouplings.LambdaSU2;

    deque< Models::PartialWidth > widths;
    for ( int i = 0; i < 4; ++i ){
        double lambdaEff = lambdaHu * pow( model_.Susy.MixingAngles.zMix[i][0], 2. ) + lambdaHd * pow( model_.Susy.MixingAngles.zMix[i][1], 2. )
        + lambdaSU2 * pow(model_.Susy.MixingAngles.zMix[i][2], 2.) + lambdaU1 * pow(model_.Susy.MixingAngles.zMix[i][3], 2.);

        widths.push_back(widthDiNeutralino(lambdaEff, neutralino1, model_.Constants.mPlanck));
    }

    std::deque< Models::Particle > neutralinos = { neutralino1, neutralino2, neutralino3, neutralino4 };
    for ( int i = 0; i < 4; ++i ){
        for ( int j = 0; j < 4; ++j ){
            // we only want i != j
            if ( i == j ){
                continue;
            }

            // there are additional theta's in original lagrangian, which are defined so m_neutralino > 0, so use Isajet output to insert the correct signs
            // depending on which neutralinos have m < 0, there may be constructive or destructive interference between groups of couplings
            double thI = copysign(1., neutralinos[i].Mass);
            double thJ = copysign(1., neutralinos[j].Mass);

            double lambdaEff = lambdaHu * model_.Susy.MixingAngles.zMix[i][0] * model_.Susy.MixingAngles.zMix[j][0] 
            + lambdaHd * model_.Susy.MixingAngles.zMix[i][1] * model_.Susy.MixingAngles.zMix[j][1]
            + thI * thJ
            * ( 
                lambdaSU2 * model_.Susy.MixingAngles.zMix[i][2] * model_.Susy.MixingAngles.zMix[j][2] 
                + lambdaU1 * model_.Susy.MixingAngles.zMix[i][3] * model_.Susy.MixingAngles.zMix[j][3]
            );
            widths.push_back( widthNeutralinoINeutralinoJ(lambdaEff, neutralinos[i], neutralinos[j], model_.Constants.mPlanck));
        }
    }

    return widths;
}

Models::PartialWidth ModulusWidths::widthDiChargino(double lambdaEff, double gV, double gA, const Models::Particle& chargino, double mPlanck){
    double mModulus = parent_.Mass;
    double mW = chargino.Mass;
    double massRatioSqr = pow( mW / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaEff * mW, 2. ) * mModulus / ( 16. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * ( pow(gV, 2.) * ( 1. - 4. * massRatioSqr ) + pow(gA, 2.) ) * Utility::SqrtLambda( 1., massRatioSqr, massRatioSqr );

    return Models::PartialWidth( parent_, { chargino, chargino }, width  );
}

Models::PartialWidth ModulusWidths::widthChargino1Chargino2(double lambdaEff, double gV, double gA, const Models::Particle& chargino1, const Models::Particle& chargino2, double mPlanck){
    double mModulus = parent_.Mass;
    double mW1 = chargino1.Mass;
    double mW2 = chargino2.Mass;
    double massRatioSqr1 = pow( mW1 / mModulus, 2. );
    double massRatioSqr2 = pow( mW2 / mModulus, 2. );

    double vecMass = pow( (mW2 + mW1) / mModulus, 2. ); 
    double axialMass = pow( (mW2 - mW1) / mModulus, 2. ); 

    // calculate order of magnitude
    double prefactor = pow( lambdaEff, 2. ) * ( pow( mW1, 2. ) + pow( mW2, 2. ) ) * mModulus / ( 16. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * ( pow(gV, 2.) * ( 1. - vecMass ) + pow(gA, 2.) * ( 1. - axialMass ) ) * Utility::SqrtLambda( 1., massRatioSqr1, massRatioSqr2 );

    return Models::PartialWidth( parent_, { chargino1, chargino2 }, width  );
}

std::deque< Models::PartialWidth > ModulusWidths::widthCharginos(){
    auto chargino1 = ModelBaseOps::FindConst(particles_, "chargino1");
    auto chargino2 = ModelBaseOps::FindConst(particles_, "chargino2");

    double lambdaSU2 = model_.ModulusCouplings.GaugeCouplings.LambdaSU2;
    double lambdaHu = model_.ModulusCouplings.HiggsCouplings.LambdaHu;
    double lambdaHd = model_.ModulusCouplings.HiggsCouplings.LambdaHd;

    double gammaL = model_.Susy.MixingAngles.gammaL;
    double gammaR = model_.Susy.MixingAngles.gammaR;
    // See e.g. WSS 8.58
    double thX = copysign(1., 1./tan(gammaL));
    double thY = copysign(1., 1./tan(gammaR));

    double xc = 0.5 * ( pow( sin( gammaL ), 2. ) + pow( sin(gammaR), 2. ) );
    double yc = 0.5 * ( pow( sin( gammaL ), 2. ) - pow( sin(gammaR), 2. ) );
    double xs = 0.5 * ( pow( cos( gammaL ), 2. ) + pow( cos(gammaR), 2. ) );
    double ys = 0.5 * ( pow( cos( gammaL ), 2. ) - pow( cos(gammaR), 2. ) );

    double x = 0.5 * ( copysign(1., chargino1.Mass * chargino2.Mass ) * thX * sin( gammaL ) * cos( gammaL ) + thY * sin( gammaR ) * cos( gammaR ) );
    double y = 0.5 * ( copysign(1., chargino1.Mass * chargino2.Mass ) * thX * sin( gammaL ) * cos( gammaL ) - thY * sin( gammaR ) * cos( gammaR ) );

    deque< Models::PartialWidth > widths;
    double gVW1 = 2. * lambdaSU2 * xc + ( lambdaHu + lambdaHd ) * xs;
    double gAW1 = 2. * lambdaSU2 * yc + ( lambdaHu + lambdaHd ) * ys;
    widths.push_back(widthDiChargino(1., gVW1, gAW1, chargino1, model_.Constants.mPlanck));

    double gVW2 = 2. * lambdaSU2 * xs + ( lambdaHu + lambdaHd ) * xc;
    double gAW2 = 2. * lambdaSU2 * ys + ( lambdaHu + lambdaHd ) * yc;    
    widths.push_back(widthDiChargino(1., gVW2, gAW2, chargino2, model_.Constants.mPlanck));

    double lambdaEff = ( 2. * lambdaSU2 - lambdaHu - lambdaHd );
    widths.push_back(widthChargino1Chargino2(lambdaEff, x, y, chargino1, chargino2, model_.Constants.mPlanck));
    return widths;
}

Models::PartialWidth ModulusWidths::widthGluinoGluino(double lambdaSU3, const Models::Particle& gluino, double mPlanck){
    double mModulus = parent_.Mass;
    double mG = gluino.Mass;
    double massRatioSqr = pow( mG / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaSU3 * mG, 2. ) * mModulus / ( M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * ( 1. - 4. * massRatioSqr ) * Utility::SqrtLambda( 1., massRatioSqr, massRatioSqr );

    return Models::PartialWidth( parent_, { gluino, gluino }, width  );
}

Models::PartialWidth ModulusWidths::widthGluinoGluino(){
    auto gluino = ModelBaseOps::FindConst(particles_, "gluino");
    double lambdaSU3 = model_.ModulusCouplings.GaugeCouplings.LambdaSU3;

    return widthGluinoGluino(lambdaSU3, gluino, model_.Constants.mPlanck);
}

Models::PartialWidth ModulusWidths::widthWBosonWBoson(double lambdaSU2, const Models::Particle& wBoson, double mPlanck){
    double mModulus = parent_.Mass;
    double mW = wBoson.Mass;
    double massRatioSqr = pow( mW / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaSU2, 2. ) * pow( mModulus, 3. ) / ( 16. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * ( 1. - 4. * massRatioSqr + 6. * pow( massRatioSqr, 2. ) ) * Utility::SqrtLambda( 1., massRatioSqr, massRatioSqr );

    return Models::PartialWidth( parent_, { wBoson, wBoson }, width  );
}

Models::PartialWidth ModulusWidths::widthZBosonZBoson(double lambdaU1, double lambdaSU2, double sin2tw, const Models::Particle& zBoson, double mPlanck){
    double mModulus = parent_.Mass;
    double mZ = zBoson.Mass;
    double massRatioSqr = pow( mZ / mModulus, 2. );
    double effCoupling = lambdaSU2 * (1 - sin2tw) + lambdaU1 * sin2tw;
    // calculate order of magnitude
    double prefactor = pow( effCoupling, 2. ) * pow( mModulus, 3. ) / ( 32. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * ( 1. - 4. * massRatioSqr + 6. * pow( massRatioSqr, 2. ) ) * Utility::SqrtLambda( 1., massRatioSqr, massRatioSqr );

    return Models::PartialWidth( parent_, { zBoson, zBoson }, width  );
}

Models::PartialWidth ModulusWidths::widthPhotonPhoton(double lambdaU1, double lambdaSU2, double sin2tw, const Models::Particle& photon, double mPlanck){
    double mModulus = parent_.Mass;

    double effCoupling = lambdaSU2 * sin2tw + lambdaU1 * ( 1 - sin2tw );
    // calculate width
    double width = pow( effCoupling, 2. ) * pow( mModulus, 3. ) / ( 32. * M_PI * pow( mPlanck, 2. ) );

    return Models::PartialWidth( parent_, { photon, photon }, width  );
}

Models::PartialWidth ModulusWidths::widthZBosonPhoton(double lambdaU1, double lambdaSU2, double sin2tw, const Models::Particle& zBoson, const Models::Particle& photon, double mPlanck){
    double mModulus = parent_.Mass;
    double mZ = zBoson.Mass;
    double massRatioSqr = pow( mZ / mModulus, 2. );
    double effCoupling = lambdaSU2 - lambdaU1;
    // calculate order of magnitude
    double prefactor = sin2tw * (1 - sin2tw) * pow( effCoupling, 2. ) * pow( mModulus, 3. ) / ( 16. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * pow( 1. - massRatioSqr, 2. ) * Utility::SqrtLambda( 1., 0., massRatioSqr );

    return Models::PartialWidth( parent_, { zBoson, photon }, width  );
}

Models::PartialWidth ModulusWidths::widthGluonGluon(double lambdaSU3, const Models::Particle& gluon, double mPlanck){
    double mModulus = parent_.Mass;

    double effCoupling = lambdaSU3;
    // calculate width
    double width = pow( effCoupling, 2. ) * pow( mModulus, 3. ) / ( 4. * M_PI * pow( mPlanck, 2. ) );

    return Models::PartialWidth( parent_, { gluon, gluon }, width  );
}

deque< Models::PartialWidth > ModulusWidths::widthVBosonVBoson(){
    auto wBoson = ModelBaseOps::FindConst(particles_, "wboson");
    auto zBoson = ModelBaseOps::FindConst(particles_, "zboson");
    auto photon = ModelBaseOps::FindConst(particles_, "photon");
    auto gluon = ModelBaseOps::FindConst(particles_, "gluon");

    double lambdaU1 = model_.ModulusCouplings.GaugeCouplings.LambdaU1;
    double lambdaSU2 = model_.ModulusCouplings.GaugeCouplings.LambdaSU2;
    double lambdaSU3 = model_.ModulusCouplings.GaugeCouplings.LambdaSU3;
    
    deque< Models::PartialWidth > widths;
    widths.push_back(widthZBosonZBoson(lambdaU1, lambdaSU2, model_.StandardModel.MixingAngles.SinSqrThetaW, zBoson, model_.Constants.mPlanck));
    widths.push_back(widthWBosonWBoson(lambdaSU2, wBoson, model_.Constants.mPlanck));
    widths.push_back(widthPhotonPhoton(lambdaU1, lambdaSU2, model_.StandardModel.MixingAngles.SinSqrThetaW, photon, model_.Constants.mPlanck));
    widths.push_back(widthZBosonPhoton(lambdaU1, lambdaSU2, model_.StandardModel.MixingAngles.SinSqrThetaW, zBoson, photon, model_.Constants.mPlanck));
    widths.push_back(widthGluonGluon(lambdaSU3, gluon, model_.Constants.mPlanck));
    return widths;
}

Models::PartialWidth ModulusWidths::widthLightHiggsLightHiggs(double lambdaEff, const Models::Particle& lightHiggs, double mPlanck){
    double mModulus = parent_.Mass;
    double mh = lightHiggs.Mass;
    double massRatioSqr = pow( mh / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaEff, 2. ) * pow( mModulus, 3. ) / ( 64. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    return Models::PartialWidth( parent_, { lightHiggs, lightHiggs }, width  );
}

Models::PartialWidth ModulusWidths::widthHeavyHiggsHeavyHiggs(double lambdaEff, const Models::Particle& heavyHiggs, double mPlanck){
    double mModulus = parent_.Mass;
    double mh = heavyHiggs.Mass;
    double massRatioSqr = pow( mh / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaEff, 2. ) * pow( mModulus, 3. ) / ( 64. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    return Models::PartialWidth( parent_, { heavyHiggs, heavyHiggs }, width  );
}

Models::PartialWidth ModulusWidths::widthLightHiggsHeavyHiggs(double lambdaEff, const Models::Particle& lightHiggs, const Models::Particle& heavyHiggs, double mPlanck){
    double mModulus = parent_.Mass;
    double mhl = lightHiggs.Mass;
    double mhh = heavyHiggs.Mass;
    double massRatioSqrHL = pow( mhl / mModulus, 2. );
    double massRatioSqrHH = pow( mhh / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaEff, 2. ) * pow( mModulus, 3. ) / ( 128. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqrHL, massRatioSqrHH);

    return Models::PartialWidth( parent_, { lightHiggs, heavyHiggs }, width  );
}

Models::PartialWidth ModulusWidths::widthPseudoHiggsPseudoHiggs(double lambdaEff, const Models::Particle& pseudoHiggs, double mPlanck){
    double mModulus = parent_.Mass;
    double mh = pseudoHiggs.Mass;
    double massRatioSqr = pow( mh / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaEff, 2. ) * pow( mModulus, 3. ) / ( 64. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    return Models::PartialWidth( parent_, { pseudoHiggs, pseudoHiggs }, width  );
}

Models::PartialWidth ModulusWidths::widthChargedHiggsChargedHiggs(double lambdaEff, const Models::Particle& chargedHiggs, double mPlanck){
    double mModulus = parent_.Mass;
    double mh = chargedHiggs.Mass;
    double massRatioSqr = pow( mh / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaEff, 2. ) * pow( mModulus, 3. ) / ( 32. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    return Models::PartialWidth( parent_, { chargedHiggs, chargedHiggs }, width  );
}

deque< Models::PartialWidth > ModulusWidths::widthHiggsHiggs(){
    auto higgsLight = ModelBaseOps::FindConst(particles_, "higgslight");
    auto higgsHeavy = ModelBaseOps::FindConst(particles_, "higgsheavy");
    auto higgsPseudo = ModelBaseOps::FindConst(particles_, "higgspseudoscalar");
    auto higgsCharged = ModelBaseOps::FindConst(particles_, "higgscharged");

    double lambdaGM = model_.ModulusCouplings.HiggsCouplings.LambdaGM;
    double lambdaHu = model_.ModulusCouplings.HiggsCouplings.LambdaHu;
    double lambdaHd = model_.ModulusCouplings.HiggsCouplings.LambdaHd;
    double alpha = model_.Susy.MixingAngles.alfah;
    double beta = atan( model_.Susy.TanBeta );

    deque< Models::PartialWidth > widths;
    double lambdaEff_hh = lambdaGM * sin( 2. * alpha ) + 2. * ( lambdaHu * pow( cos( alpha ), 2. ) + lambdaHd * pow( sin( alpha ), 2. ) ) * pow( higgsLight.Mass / parent_.Mass, 2. );
    widths.push_back(widthLightHiggsLightHiggs(lambdaEff_hh, higgsLight, model_.Constants.mPlanck));

    double lambdaEff_HH = lambdaGM * sin( 2. * alpha ) - 2. * ( lambdaHu * pow( sin( alpha ), 2. ) + lambdaHd * pow( cos( alpha ), 2. ) ) * pow( higgsHeavy.Mass / parent_.Mass, 2. );
    widths.push_back(widthHeavyHiggsHeavyHiggs(lambdaEff_HH, higgsHeavy, model_.Constants.mPlanck));

    double lambdaEff_hH = 2. * lambdaGM * cos( 2. * alpha ) + ( lambdaHd - lambdaHu ) * sin( 2. * alpha ) * ( pow( higgsLight.Mass / parent_.Mass, 2. ) + pow( higgsHeavy.Mass / parent_.Mass, 2. ) );
    widths.push_back(widthLightHiggsHeavyHiggs(lambdaEff_hH, higgsLight, higgsHeavy, model_.Constants.mPlanck));

    double lambdaEff_AA = lambdaGM * sin( 2. * beta ) - 2. * ( lambdaHu * pow( cos( beta ), 2. ) + lambdaHd * pow( sin( beta ), 2. ) ) * pow( higgsPseudo.Mass / parent_.Mass, 2. );
    widths.push_back(widthPseudoHiggsPseudoHiggs(lambdaEff_AA, higgsPseudo, model_.Constants.mPlanck));

    double lambdaEff_HplsHmns = lambdaGM * sin( 2. * beta ) - 2. * ( lambdaHu * pow( cos( beta ), 2. ) + lambdaHd * pow( sin( beta ), 2. ) ) * pow( higgsCharged.Mass / parent_.Mass, 2. );
    widths.push_back(widthChargedHiggsChargedHiggs(lambdaEff_HplsHmns, higgsCharged, model_.Constants.mPlanck));
    return widths;
}

Models::PartialWidth ModulusWidths::widthSquarkLSquarkL(double lambdaQ, const Models::Particle& squarkL, double mPlanck){
    double mModulus = parent_.Mass;
    double mSq = squarkL.Mass;
    double massRatioSqr = pow( mSq / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaQ, 2. ) * pow( mSq, 4. ) / ( 8. * M_PI * mModulus * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    // include color factor of 3 in width
    return Models::PartialWidth( parent_, { squarkL, squarkL }, 3. * width  );
}

Models::PartialWidth ModulusWidths::widthSquarkRSquarkR(double lambdaUD, const Models::Particle& squarkR, double mPlanck){
    double mModulus = parent_.Mass;
    double mSq = squarkR.Mass;
    double massRatioSqr = pow( mSq / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaUD, 2. ) * pow( mSq, 4. ) / ( 8. * M_PI * mModulus * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    // include color factor of 3 in width
    return Models::PartialWidth( parent_, { squarkR, squarkR }, 3. * width  );
}

Models::PartialWidth ModulusWidths::widthSquark1Squark1(double lambdaUD, double lambdaQ3, double thetaMix, const Models::Particle& squark1, double mPlanck){
    double mModulus = parent_.Mass;
    double mSq = squark1.Mass;
    double massRatioSqr = pow( mSq / mModulus, 2. );
    double effCoupling = pow( lambdaQ3 * pow( cos( thetaMix ), 2. ) + lambdaUD * pow( sin( thetaMix ), 2. ) , 2. );
    // calculate order of magnitude
    double prefactor = effCoupling * pow( mSq, 4. ) / ( 8. * M_PI * mModulus * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    // include color factor of 3 in width
    return Models::PartialWidth( parent_, { squark1, squark1 }, 3. * width  );
}

Models::PartialWidth ModulusWidths::widthSquark2Squark2(double lambdaUD, double lambdaQ3, double thetaMix, const Models::Particle& squark2, double mPlanck){
    double mModulus = parent_.Mass;
    double mSq = squark2.Mass;
    double massRatioSqr = pow( mSq / mModulus, 2. );
    double effCoupling = pow( lambdaQ3 * pow( sin( thetaMix ), 2. ) + lambdaUD * pow( cos( thetaMix ), 2. ) , 2. );
    // calculate order of magnitude
    double prefactor = effCoupling * pow( mSq, 4. ) / ( 8. * M_PI * mModulus * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    // include color factor of 3 in width
    return Models::PartialWidth( parent_, { squark2, squark2 }, 3. * width  );
}

Models::PartialWidth ModulusWidths::widthSquark1Squark2(double lambdaUD, double lambdaQ3, double thetaMix, const Models::Particle& squark1, const Models::Particle& squark2, double mPlanck){
    double mModulus = parent_.Mass;
    double mSq1 = squark1.Mass;
    double mSq2 = squark2.Mass;
    double massRatioSqr1 = pow( mSq1 / mModulus, 2. );
    double massRatioSqr2 = pow( mSq2 / mModulus, 2. );
    double effCoupling = pow( ( lambdaUD - lambdaQ3) * sin( 2. * thetaMix ), 2. );
    // calculate order of magnitude
    double prefactor = effCoupling * pow( pow( mSq1, 2. ) + pow( mSq2, 2. ), 2. ) / ( 32. * M_PI * mModulus * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr1, massRatioSqr2);

    // include color factor of 3 in width
    return Models::PartialWidth( parent_, { squark1, squark2 }, 3. * width  );
}

deque< Models::PartialWidth > ModulusWidths::widthSquarkSquark(){
    // Left-hand squarks
    auto upl = ModelBaseOps::FindConst(particles_, "suplsq");
    auto dnl = ModelBaseOps::FindConst(particles_, "sdownlsq");
    auto stl = ModelBaseOps::FindConst(particles_, "sstrangelsq");
    auto chl = ModelBaseOps::FindConst(particles_, "scharmlsq");
    // Right-hand squarks
    auto upr = ModelBaseOps::FindConst(particles_, "suprsq");
    auto dnr = ModelBaseOps::FindConst(particles_, "sdownrsq");
    auto str = ModelBaseOps::FindConst(particles_, "sstrangersq");
    auto chr = ModelBaseOps::FindConst(particles_, "scharmrsq");
    // Mixed-state squarks
    auto sb1 = ModelBaseOps::FindConst(particles_, "sbottom1sq");
    auto sb2 = ModelBaseOps::FindConst(particles_, "sbottom2sq");
    auto st1 = ModelBaseOps::FindConst(particles_, "stop1sq");
    auto st2 = ModelBaseOps::FindConst(particles_, "stop2sq");

    double lambdaQ1 = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaQ1;
    double lambdaQ2 = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaQ2;
    double lambdaQ3 = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaQ3;

    double lambdaUu = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaUu;
    double lambdaUc = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaUc;
    double lambdaUt = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaUt;
    double lambdaDd = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaDd;
    double lambdaDs = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaDs;
    double lambdaDb = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaDb;

    double thetaT = model_.Susy.MixingAngles.thetaT;
    double thetaB = model_.Susy.MixingAngles.thetaB;

    deque< Models::PartialWidth > widths;
    // add all left-hand squarks (1st & 2nd gen.)
    widths.push_back(widthSquarkLSquarkL(lambdaQ1, upl, model_.Constants.mPlanck));
    widths.push_back(widthSquarkLSquarkL(lambdaQ1, dnl, model_.Constants.mPlanck));
    widths.push_back(widthSquarkLSquarkL(lambdaQ2, stl, model_.Constants.mPlanck));
    widths.push_back(widthSquarkLSquarkL(lambdaQ2, chl, model_.Constants.mPlanck));
    // add all right-hand squarks (1st & 2nd gen.)
    widths.push_back(widthSquarkRSquarkR(lambdaUu, upr, model_.Constants.mPlanck));
    widths.push_back(widthSquarkRSquarkR(lambdaUc, chr, model_.Constants.mPlanck));
    widths.push_back(widthSquarkRSquarkR(lambdaDd, dnr, model_.Constants.mPlanck));
    widths.push_back(widthSquarkRSquarkR(lambdaDs, str, model_.Constants.mPlanck));
    // add stops (t1, t2) and mixed t1 t2 states
    widths.push_back(widthSquark1Squark1(lambdaUt, lambdaQ3, thetaT, st1, model_.Constants.mPlanck));
    widths.push_back(widthSquark2Squark2(lambdaUt, lambdaQ3, thetaT, st2, model_.Constants.mPlanck));
    widths.push_back(widthSquark1Squark2(lambdaUt, lambdaQ3, thetaT, st1, st2, model_.Constants.mPlanck));
    // add sbottoms (b1, b2) and mixed b1 b2 states
    widths.push_back(widthSquark1Squark1(lambdaDb, lambdaQ3, thetaB, sb1, model_.Constants.mPlanck));
    widths.push_back(widthSquark2Squark2(lambdaDb, lambdaQ3, thetaB, sb2, model_.Constants.mPlanck));
    widths.push_back(widthSquark1Squark2(lambdaDb, lambdaQ3, thetaB, sb1, sb2, model_.Constants.mPlanck));

    return widths;
}

Models::PartialWidth ModulusWidths::widthSleptonLRSleptonLR(double lambdaLE, const Models::Particle& sleptonLR, double mPlanck){
    double mModulus = parent_.Mass;
    double mSl = sleptonLR.Mass;
    double massRatioSqr = pow( mSl / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaLE, 2. ) * pow( mSl, 4. ) / ( 8. * M_PI * mModulus * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    return Models::PartialWidth( parent_, { sleptonLR, sleptonLR }, width  );
}

Models::PartialWidth ModulusWidths::widthSlepton1Slepton1(double lambdaL, double lambdaE3, double thetaMix, const Models::Particle& slepton1, double mPlanck){
    double mModulus = parent_.Mass;
    double mSl1 = slepton1.Mass;
    double massRatioSqr = pow( mSl1 / mModulus, 2. );
    double effCoupling = pow( lambdaE3 * pow( sin(thetaMix), 2. ) + lambdaL * pow( cos(thetaMix), 2. ), 2. );
    // calculate order of magnitude
    double prefactor = effCoupling * pow( mSl1, 4. ) / ( 8. * M_PI * mModulus * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    return Models::PartialWidth( parent_, { slepton1, slepton1 }, width  );
}

Models::PartialWidth ModulusWidths::widthSlepton2Slepton2(double lambdaL, double lambdaE3, double thetaMix, const Models::Particle& slepton2, double mPlanck){
    double mModulus = parent_.Mass;
    double mSl2 = slepton2.Mass;
    double massRatioSqr = pow( mSl2 / mModulus, 2. );
    double effCoupling = pow( lambdaE3 * pow( cos(thetaMix), 2. ) + lambdaL * pow( sin(thetaMix), 2. ), 2. );
    // calculate order of magnitude
    double prefactor = effCoupling * pow( mSl2, 4. ) / ( 8. * M_PI * mModulus * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    return Models::PartialWidth( parent_, { slepton2, slepton2 }, width  );
}

Models::PartialWidth ModulusWidths::widthSlepton1Slepton2(double lambdaL, double lambdaE3, double thetaMix, const Models::Particle& slepton1, const Models::Particle& slepton2, double mPlanck){
    double mModulus = parent_.Mass;
    double mSl1 = slepton1.Mass;
    double mSl2 = slepton2.Mass;
    double massRatioSqr1 = pow( mSl1 / mModulus, 2. );
    double massRatioSqr2 = pow( mSl2 / mModulus, 2. );
    double effCoupling = pow( ( lambdaE3 - lambdaL ) * sin(2. * thetaMix), 2. );
    // calculate order of magnitude
    double prefactor = effCoupling * pow( pow( mSl1, 2. ) + pow( mSl2, 2. ), 2. ) / ( 32. * M_PI * mModulus * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr1, massRatioSqr2);

    return Models::PartialWidth( parent_, { slepton1, slepton2 }, width  );
}

deque< Models::PartialWidth > ModulusWidths::widthSleptonSlepton(){
    // sneutrinos
    auto nuel = ModelBaseOps::FindConst(particles_, "sneutrinoselectronl");
    auto numu = ModelBaseOps::FindConst(particles_, "sneutrinosmuonl");
    auto nutau = ModelBaseOps::FindConst(particles_, "sneutrinostaul");
    // left-hand sleptons
    auto sel = ModelBaseOps::FindConst(particles_, "selectronl");
    auto smul = ModelBaseOps::FindConst(particles_, "smuonl");
    // right-hand sleptons
    auto ser = ModelBaseOps::FindConst(particles_, "selectronr");
    auto smur = ModelBaseOps::FindConst(particles_, "smuonr");
    // staus 1 & 2, and mixed state
    auto stau1 = ModelBaseOps::FindConst(particles_, "stau1");
    auto stau2 = ModelBaseOps::FindConst(particles_, "stau2");

    double lambdaLe = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaLe;
    double lambdaLm = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaLm;
    double lambdaLt = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaLt;
    double lambdaE1 = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaE1;
    double lambdaE2 = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaE2;
    double lambdaE3 = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaE3;

    double thetaTau = model_.Susy.MixingAngles.thetaL;

    deque< Models::PartialWidth > widths;
    widths.push_back(widthSleptonLRSleptonLR(lambdaLe, nuel, model_.Constants.mPlanck));
    widths.push_back(widthSleptonLRSleptonLR(lambdaLm, numu, model_.Constants.mPlanck));
    widths.push_back(widthSleptonLRSleptonLR(lambdaLt, nutau, model_.Constants.mPlanck));

    widths.push_back(widthSleptonLRSleptonLR(lambdaLe, sel, model_.Constants.mPlanck));
    widths.push_back(widthSleptonLRSleptonLR(lambdaLm, smul, model_.Constants.mPlanck));
    widths.push_back(widthSleptonLRSleptonLR(lambdaE1, ser, model_.Constants.mPlanck));
    widths.push_back(widthSleptonLRSleptonLR(lambdaE2, smur, model_.Constants.mPlanck));

    widths.push_back(widthSlepton1Slepton1(lambdaLt, lambdaE3, thetaTau, stau1, model_.Constants.mPlanck));
    widths.push_back(widthSlepton2Slepton2(lambdaLt, lambdaE3, thetaTau, stau2, model_.Constants.mPlanck));
    widths.push_back(widthSlepton1Slepton2(lambdaLt, lambdaE3, thetaTau, stau1, stau2, model_.Constants.mPlanck));

    return widths;
}

Models::PartialWidth ModulusWidths::widthQuarkQuark(double lambdaUD, double lambdaQ, const Models::Particle& quark, double mPlanck){
    double mModulus = parent_.Mass;
    double mQ = quark.Mass;
    double massRatioSqr = pow( mQ / mModulus, 2. );
    double effCoupling = pow( lambdaQ + lambdaUD, 2. );
    // calculate order of magnitude
    double prefactor = effCoupling * pow( mQ, 2. ) * mModulus * ( 1. - 4. * massRatioSqr) / ( 16. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    // include color factor of 3
    return Models::PartialWidth( parent_, { quark, quark }, 3. * width  );
}

deque< Models::PartialWidth > ModulusWidths::widthQuarkQuark(){
    // up-type quarks
    auto upQ = ModelBaseOps::FindConst(particles_, "upq");
    auto charmQ = ModelBaseOps::FindConst(particles_, "charmq");
    auto topQ = ModelBaseOps::FindConst(particles_, "topq");
    // down-type quarks
    auto downQ = ModelBaseOps::FindConst(particles_, "downq");
    auto strangeQ = ModelBaseOps::FindConst(particles_, "strangeq");
    auto bottomQ = ModelBaseOps::FindConst(particles_, "bottomq");

    double lambdaUu = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaUu;
    double lambdaUc = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaUc;
    double lambdaUt = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaUt;

    double lambdaDd = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaDd;
    double lambdaDs = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaDs;
    double lambdaDb = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaDb;

    double lambdaQ1 = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaQ1;
    double lambdaQ2 = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaQ2;
    double lambdaQ3 = model_.ModulusCouplings.QuarkSquarkCouplings.LambdaQ3;

    deque< Models::PartialWidth > widths;
    widths.push_back(widthQuarkQuark(lambdaUu, lambdaQ1, upQ, model_.Constants.mPlanck));
    widths.push_back(widthQuarkQuark(lambdaUc, lambdaQ2, charmQ, model_.Constants.mPlanck));
    widths.push_back(widthQuarkQuark(lambdaUt, lambdaQ3, topQ, model_.Constants.mPlanck));
    widths.push_back(widthQuarkQuark(lambdaDd, lambdaQ1, downQ, model_.Constants.mPlanck));
    widths.push_back(widthQuarkQuark(lambdaDs, lambdaQ2, strangeQ, model_.Constants.mPlanck));
    widths.push_back(widthQuarkQuark(lambdaDb, lambdaQ3, bottomQ, model_.Constants.mPlanck));
    return widths;
}

Models::PartialWidth ModulusWidths::widthChargedDiLepton(double lambdaL, double lambdaE, const Models::Particle& lepton, double mPlanck){
    double mModulus = parent_.Mass;
    double mL = lepton.Mass;
    double massRatioSqr = pow( mL / mModulus, 2. );
    double effCoupling = pow( lambdaL + lambdaE, 2. );
    // calculate order of magnitude
    double prefactor = effCoupling * pow( mL, 2. ) * mModulus * ( 1. - 4. * massRatioSqr) / ( 16. * M_PI * pow( mPlanck, 2. ) );
    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    return Models::PartialWidth( parent_, { lepton, lepton }, width  );
}

deque< Models::PartialWidth > ModulusWidths::widthLeptonLepton(){
    // charged leptons
    auto el = ModelBaseOps::FindConst(particles_, "electron");
    auto mu = ModelBaseOps::FindConst(particles_, "muon");
    auto tau = ModelBaseOps::FindConst(particles_, "tau");

    double lambdaLe = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaLe;
    double lambdaLm = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaLm;
    double lambdaLt = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaLt;

    double lambdaE1 = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaE1;
    double lambdaE2 = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaE2;
    double lambdaE3 = model_.ModulusCouplings.LeptonSleptonCouplings.LambdaE3;

    deque< Models::PartialWidth > widths;
    widths.push_back(widthChargedDiLepton(lambdaLe, lambdaE1, el, model_.Constants.mPlanck));
    widths.push_back(widthChargedDiLepton(lambdaLm, lambdaE2, mu, model_.Constants.mPlanck));
    widths.push_back(widthChargedDiLepton(lambdaLt, lambdaE3, tau, model_.Constants.mPlanck));
    return widths;
}

// We're taking this from Nakamura hep-ph/0602081, Eq. 26
Models::PartialWidth ModulusWidths::widthDiGravitino(double lambdaG, const Models::Particle& gravitino, double mPlanck){
    double mModulus = parent_.Mass;
    double mG = gravitino.Mass;
    double massRatioSqr = pow( mG / mModulus, 2. );
    // calculate order of magnitude
    double prefactor = pow( lambdaG, 2. ) * pow( mModulus, 3. ) / ( 288. * M_PI * pow( mPlanck, 2. ) );
//    double prefactor = pow( lambdaG, 2. ) * pow( mModulus, 3. ) * massRatioSqr / ( 288. * M_PI * pow( mPlanck, 2. ) );

    // now stitch in the rest of the width
    double width = prefactor * Utility::SqrtLambda(1., massRatioSqr, massRatioSqr);

    return Models::PartialWidth( parent_, { gravitino, gravitino }, width  );
}

Models::PartialWidth ModulusWidths::widthDiGravitino(){
    // charged leptons
    auto gravitino = ModelBaseOps::FindConst(particles_, "gravitino");

    double lambdaG = model_.ModulusCouplings.GravitinoCouplings.LambdaG;
    return widthDiGravitino(lambdaG, gravitino, model_.Constants.mPlanck);
}
