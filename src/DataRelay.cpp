#include <DataRelay.h>

using namespace std;

void DataRelay::setNonUniversalSugra(ModelBase model){
    auto sugOpts = model.Options.NonUniversalSugra;
    if (sugOpts.Gaugino){
//        (*sugnu_).xnusug[0] = M_1 gaugino at GUT
//        (*sugnu_).xnusug[1] = M_2 gaugino at GUT
//        (*sugnu_).xnusug[2] = M_3 gaugino at GUT
    }
    if (model.Options.NonUniversalSugra.GenOneTwo.GenOneTwo){
        (*sugnu_).xnusug[8] = model.Options.NonUniversalSugra.GenOneTwo.m_eR_GUT; // M_eR at GUT
        (*sugnu_).xnusug[9] = model.Options.NonUniversalSugra.GenOneTwo.m_eL_GUT; // M_eL at GUT
        (*sugnu_).xnusug[10] = model.Options.NonUniversalSugra.GenOneTwo.m_dR_GUT; // M_dR at GUT
        (*sugnu_).xnusug[11] = model.Options.NonUniversalSugra.GenOneTwo.m_uR_GUT; // M_uR at GUT
        (*sugnu_).xnusug[12] = model.Options.NonUniversalSugra.GenOneTwo.m_uL_GUT; // M_uL at GUT
    }
    if (model.Options.NonUniversalSugra.GenThree.GenThree){
        (*sugnu_).xnusug[13] = model.Options.NonUniversalSugra.GenThree.m_lR_GUT; // M_LR at GUT
        (*sugnu_).xnusug[14] = model.Options.NonUniversalSugra.GenThree.m_lL_GUT; // M_LL at GUT
        (*sugnu_).xnusug[15] = model.Options.NonUniversalSugra.GenThree.m_bR_GUT; // M_bR at GUT
        (*sugnu_).xnusug[16] = model.Options.NonUniversalSugra.GenThree.m_tR_GUT; // M_tR at GUT
        (*sugnu_).xnusug[17] = model.Options.NonUniversalSugra.GenThree.m_tL_GUT; // M_tL at GUT
    }
    if (sugOpts.Higgs){
//        (*sugnu_).xnusug[6] = m_Hd at GUT
//        (*sugnu_).xnusug[7] = m_Hu at GUT
    }
    if (sugOpts.RightNeutrino){
//        (*sugxin_).xnrin[0] = M(nu_3)
//        (*sugxin_).xnrin[1] = M_Majorana
//        (*sugxin_).xnrin[2] = A_N
//        (*sugxin_).xnrin[3] = M(NRSS)
    }
    if (sugOpts.Trilinear){
//        (*sugnu_).xnusug[3] = A_tau at GUT
//        (*sugnu_).xnusug[4] = A_b at GUT
//        (*sugnu_).xnusug[5] = A_t at GUT
    }
    if (model.Options.NonUniversalSugra.WeakInput.WeakInput){
        (*sugnu_).xnusug[18] = (float)model.Options.NonUniversalSugra.WeakInput.weakMu; // mu at mWeak
        (*sugnu_).xnusug[19] = (float)model.Options.NonUniversalSugra.WeakInput.weakMA; // m_A at mWeak
        (*sugpas_).mu = model.Options.NonUniversalSugra.WeakInput.weakMu;
        (*sspar_).amha = model.Options.NonUniversalSugra.WeakInput.weakMA;
        (*sspar_).twom1 = -model.Options.NonUniversalSugra.WeakInput.weakMu;
        (*sugnu_).inuhm = 1;
    }
}

void DataRelay::setSugraData(ModelBase& model){
    switch (model.Options.ModelInt)
    {
        case 3:
            setNonUniversalSugra(model);
            break;
        default:
            throw_with_trace( std::logic_error("Model not implemented!") );
    }
}

void DataRelay::Retrieve(ModelBase& model){
    if (model.PQSector.AxionModel == DFSZ){
        model.PQSector.nDW = 6;
    } else if (model.PQSector.AxionModel == KSVZ){
        model.PQSector.nDW = 1;
    }
    // set masses
    retrieveMasses(model.Masses, model, model.PQSector);
    retrieveMixingAngles(model.Susy);
    retrieveCouplings(model.Susy);
    model.RunningParameters = (*trhpars_);
}

void DataRelay::retrieveMasses(ParticleMasses& masses, ModelBase& model, PecceiQuinnParams& pqParams){    
    ModelBaseOps::Find(model.Particles, "gluino").Mass = (*sugmg_).mss[0];
    ModelBaseOps::Find(model.Particles, "suplsq").Mass = (*sugmg_).mss[1];
    ModelBaseOps::Find(model.Particles, "suprsq").Mass = (*sugmg_).mss[2];
    ModelBaseOps::Find(model.Particles, "sdownlsq").Mass = (*sugmg_).mss[3];
    ModelBaseOps::Find(model.Particles, "sdownrsq").Mass = (*sugmg_).mss[4];
    ModelBaseOps::Find(model.Particles, "sstrangelsq").Mass = (*sugmg_).mss[5];
    ModelBaseOps::Find(model.Particles, "sstrangersq").Mass = (*sugmg_).mss[6];
    ModelBaseOps::Find(model.Particles, "scharmlsq").Mass = (*sugmg_).mss[7];
    ModelBaseOps::Find(model.Particles, "scharmrsq").Mass = (*sugmg_).mss[8];
    ModelBaseOps::Find(model.Particles, "sbottom1sq").Mass = (*sugmg_).mss[9];
    ModelBaseOps::Find(model.Particles, "sbottom2sq").Mass = (*sugmg_).mss[10];
    ModelBaseOps::Find(model.Particles, "stop1sq").Mass = (*sugmg_).mss[11];
    ModelBaseOps::Find(model.Particles, "stop2sq").Mass = (*sugmg_).mss[12];
    ModelBaseOps::Find(model.Particles, "sneutrinoselectronl").Mass = (*sugmg_).mss[13];
    ModelBaseOps::Find(model.Particles, "sneutrinosmuonl").Mass = (*sugmg_).mss[14];
    ModelBaseOps::Find(model.Particles, "sneutrinostaul").Mass = (*sugmg_).mss[15];
    ModelBaseOps::Find(model.Particles, "selectronl").Mass = (*sugmg_).mss[16];
    ModelBaseOps::Find(model.Particles, "selectronr").Mass = (*sugmg_).mss[17];
    ModelBaseOps::Find(model.Particles, "smuonl").Mass = (*sugmg_).mss[18];
    ModelBaseOps::Find(model.Particles, "smuonr").Mass = (*sugmg_).mss[19];
    ModelBaseOps::Find(model.Particles, "stau1").Mass = (*sugmg_).mss[20];
    ModelBaseOps::Find(model.Particles, "stau2").Mass = (*sugmg_).mss[21];
    ModelBaseOps::Find(model.Particles, "neutralino1").Mass = (*sugmg_).mss[22];
    ModelBaseOps::Find(model.Particles, "neutralino2").Mass = (*sugmg_).mss[23];
    ModelBaseOps::Find(model.Particles, "neutralino3").Mass = (*sugmg_).mss[24];
    ModelBaseOps::Find(model.Particles, "neutralino4").Mass = (*sugmg_).mss[25];
    ModelBaseOps::Find(model.Particles, "chargino1").Mass = (*sugmg_).mss[26];
    ModelBaseOps::Find(model.Particles, "chargino2").Mass = (*sugmg_).mss[27];
    ModelBaseOps::Find(model.Particles, "higgslight").Mass = (*sugmg_).mss[28];
    ModelBaseOps::Find(model.Particles, "higgsheavy").Mass = (*sugmg_).mss[29];
    ModelBaseOps::Find(model.Particles, "higgspseudoscalar").Mass = (*sugmg_).mss[30];
    ModelBaseOps::Find(model.Particles, "higgscharged").Mass = (*sugmg_).mss[31];

    masses.MSusy.topMSusy = (*sspar_).mtq;
    masses.MSusy.botMSusy = (*sspar_).mbq;
    masses.MSusy.tauMSusy = (*sspar_).mlq;

    ModelBaseOps::Find(model.Particles, "axion").Mass = 0.0062 * min( 1., 0.018 * pow( model.Cosmology.Temperatures.Hadronization / model.Cosmology.Temperatures.Reheat, 4. ) * pqParams.nDW / pqParams.Fa);
}

void DataRelay::retrieveMixingAngles(SusyParams& susy){
    susy.MixingAngles.alfah = (*sspar_).alfah;
    susy.MixingAngles.gammaL = (*sspar_).gammal;
    susy.MixingAngles.gammaR = (*sspar_).gammar;
    susy.MixingAngles.thetaB = (*sspar_).thetab;
    susy.MixingAngles.thetaT = (*sspar_).thetat;
    susy.MixingAngles.thetaL = (*sspar_).thetal;
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 4; ++j){
            susy.MixingAngles.zMix[i][j] = (*sspar_).zmixss[i][j];
        }
    }
}

void DataRelay::retrieveCouplings(SusyParams& susy){
    susy.Couplings.g_1 = (*sugmg_).gss[0];
    susy.Couplings.g_2 = (*sugmg_).gss[1];
    susy.Couplings.g_3 = (*sugmg_).gss[2];
    susy.Couplings.y_tau = (*sugmg_).gss[3];
    susy.Couplings.y_b = (*sugmg_).gss[4];
    susy.Couplings.y_t = (*sugmg_).gss[5];
    susy.Couplings.M_1 = (*sugmg_).gss[6];
    susy.Couplings.M_2 = (*sugmg_).gss[7];
    susy.Couplings.M_3 = (*sugmg_).gss[8];
    susy.Couplings.A_tau = (*sugmg_).gss[9];
    susy.Couplings.A_b = (*sugmg_).gss[10];
    susy.Couplings.A_t = (*sugmg_).gss[11];
    susy.Couplings.M_hdSqr = (*sugmg_).gss[12];
    susy.Couplings.M_huSqr = (*sugmg_).gss[13];
    susy.Couplings.M_erSqr = (*sugmg_).gss[14];
    susy.Couplings.M_elSqr = (*sugmg_).gss[15];
    susy.Couplings.M_dnrSqr = (*sugmg_).gss[16];
    susy.Couplings.M_uprSqr = (*sugmg_).gss[17];
    susy.Couplings.M_uplSqr = (*sugmg_).gss[18];
    susy.Couplings.M_taurSqr = (*sugmg_).gss[19];
    susy.Couplings.M_taulSqr = (*sugmg_).gss[20];
    susy.Couplings.M_btrSqr = (*sugmg_).gss[21];
    susy.Couplings.M_tprSqr = (*sugmg_).gss[22];
    susy.Couplings.M_tplSqr = (*sugmg_).gss[23];
    susy.Couplings.mu = (*sugmg_).gss[24];
    susy.Couplings.B = (*sugmg_).gss[25];
    susy.Couplings.Y_N = (*sugmg_).gss[26];
    susy.Couplings.M_nr = (*sugmg_).gss[27];
    susy.Couplings.A_n = (*sugmg_).gss[28];
    susy.Couplings.vdq = (*sugmg_).gss[29];
    susy.Couplings.vuq = (*sugmg_).gss[30];

    susy.Couplings.g_GUT = (*sugmg_).ggutss;
    susy.Couplings.m_GUT = (*sugmg_).mgutss;
    susy.Couplings.alpha_GUT = (*sugmg_).agutss;
}

void DataRelay::RunSugra(ModelBase& model){
    float tanB = (float)model.Susy.TanBeta;
    int imodel = 1;
    auto top = (float)ModelBaseOps::FindConst(model.Particles, "topq").Mass;
    float m0 = model.Masses.M0;
    float mHf = model.Masses.MHF;
    float a0 = model.A0;
    float sgnMu = model.SgnMu;
    logger_.Info("Running Sugra...");

    setSugraData(model);
    modsugra_(&m0, &mHf, &a0, &tanB, &sgnMu, &top, &imodel);

    // check that things went good
    model.IsajetError = (*sugpas_).nogood;
    if (model.IsajetError == 1){
        logger_.Error("BAD POINT: TACHYONIC PARTICLES!");
    } else if (model.IsajetError == 2){
        logger_.Error("BAD POINT: NO EW SYMMETRY BREAKING!");
    } else if (model.IsajetError == 3){
        logger_.Error("BAD POINT: M(H_P)^2<0!");
    } else if (model.IsajetError == 4){
        logger_.Error("BAD POINT: YUKAWA>10!");
    } else if (model.IsajetError == 5){ // AND IMODEL==1
        logger_.Error("SUGRA BAD POINT: Z1SS NOT LSP!");
    } else if (model.IsajetError == 7){
        logger_.Error("BAD POINT: X1 EWSB BAD!");
    } else if (model.IsajetError == 8){
        logger_.Error("BAD POINT: MHL^2<0!");
    } else if (model.IsajetError == -1){
        logger_.Error("BAD POINT: NO RGE SOLUTION FOUND");
    }
}

void DataRelay::InitializeMssmSpectra(ModelBase& model, int& iallow){
    int imhl;
    int imhc;
    int imsq;
    int imodel = 1;
    auto top = (float)ModelBaseOps::FindConst(model.Particles, "topq").Mass;
    logger_.Info("Initializing Mssm Mass Spectra...");
    ssmssm_(&(*sugxin_).xisain[0], &(*sugxin_).xisain[1], &(*sugxin_).xisain[2], &(*sugxin_).xisain[3], &(*sugxin_).xisain[4],
        &(*sugxin_).xisain[5], &(*sugxin_).xisain[6], &(*sugxin_).xisain[7], &(*sugxin_).xisain[8], &(*sugxin_).xisain[9],
        &(*sugxin_).xisain[10], &(*sugxin_).xisain[11], &(*sugxin_).xisain[12], &(*sugxin_).xisain[13], &(*sugxin_).xisain[14],
        &(*sugxin_).xisain[15], &(*sugxin_).xisain[16], &(*sugxin_).xisain[17], &(*sugxin_).xisain[18], &(*sugxin_).xisain[19],
        &(*sugxin_).xisain[20], &(*sugxin_).xisain[21], &(*sugxin_).xisain[22], &(*sugxin_).xisain[23],
        &top, &iallow, &imodel, &imhl, &imhc, &imsq);

    // get Delta_EW: TODO: figure out about mZ if not taking SM value
    model.DeltaEW = 2. * (*sugpas_).c5max / pow(91.17, 2.);
}

void DataRelay::FindLsp(ModelBase& model){
    int idlsp;
    logger_.Info("Verifying LSP...");
    findlsp_(&idlsp);
    if (idlsp != 23){
        throw std::logic_error("Z1SS not LSP!");
    }

    if ((*sugpas_).nogood != 0){
        throw_with_trace( std::logic_error("No good") );
    }

}

void DataRelay::TestParameters(ModelBase& model, int& iallow){
    if (iallow != 0){
        logger_.Warn("MSSM WARNING: Z1SS IS NOT LSP");
    }
    sstest_(&iallow);
    readIAllow(iallow);
}

void DataRelay::RunIsaAmu(int& isaVal, double& isaValDbl){
    float alemi = 128;
    float gM_eL = std::sqrt( ( *sugmg_ ).gss[16] );
    float gM_eR = std::sqrt( ( *sugmg_ ).gss[15] );
    isaamu_(
        &(*sspar_).rv2v1, &alemi, &(*sspar_).gammal, &(*sspar_).gammar, 
        &(*sspar_).twom1, &(*sspar_).aal, 
        &gM_eL, &gM_eR, 
        &(*sspar_).amz1ss, &(*sspar_).amz2ss, &(*sspar_).amz3ss, &(*sspar_).amz4ss, 
        &(*sspar_).amw1ss, &(*sspar_).amw2ss, 
        &(*sspar_).amn2ss, &(*sspar_).zmixss, &isaVal, &isaValDbl
    );
}

void DataRelay::RunIsaBsg(int& isaVal, int& imodel, const ModelBase& model){
    float m0 = model.Masses.M0;
    float mHf = model.Masses.MHF;
    float a0 = model.A0;
    double dbfbsg;
    isabsg_(&imodel, &m0, &mHf, &a0, &dbfbsg, &isaVal);
}

void DataRelay::RunIsaBtn(const ModelBase& model){
    float r;
    float bfbtn;
    float tanB = model.Susy.TanBeta;

    isabtn_(
        &tanB, &(*sugmg_).mss[32], &(*sugpas_).mu, &(*sugmg_).mss[1], 
        &(*sugmg_).mss[10], &(*sugmg_).mss[11], &r, &bfbtn
    );
}

void DataRelay::RunIsaRes(int& isaVal){
    isares_(&isaVal);
}

void DataRelay::RunIsaRed(int& isaVal){
    isared_(&isaVal);
}

// in SSTEST subroutine, summed 2^n gives unique error code
// can get error quickly by converting to binary representation
void DataRelay::readIAllow(int iallow){
    std::bitset<8> binaryIallow(iallow);
    if (binaryIallow[0] == 1){
        logger_.Warn("Z1 is not LSP");
    }
    if (binaryIallow[1] == 1){
        logger_.Warn("Gamma(Z -> Z1SS Z1SS) < GAMINV");
    }
    if (binaryIallow[2] == 1){
        logger_.Warn("Z -> charginos allowed");
    }
    if (binaryIallow[3] == 1){
        logger_.Warn("BF(Z -> Z1SS Z2SS)>10^5");
    }
    if (binaryIallow[4] == 1){
        logger_.Warn("Z -> squarks, sleptons");
    }
    if (binaryIallow[5] == 1){
        logger_.Warn("BF(Z -> Z* HL0) < B(Z -> Z* H(M=MHSM))");
    }
    if (binaryIallow[6] == 1){
        logger_.Warn("BR(Z -> HL0 HA0) > 0");
    }
    if (binaryIallow[7] == 1){
        logger_.Warn("M(H+) > M(Z)/2");
    }
}

bool DataRelay::Reload(double xF){
    if (__functionalIntegralResult != 0. && __previousXF != 0. && ( abs(xF - __previousXF) / abs(__previousXF) < 0.1 && nsteps_ < 10 )){
        return false;
    }
    return true;
}

double DataRelay::CalculateFunctionalIntegral(double& xF){
    logger_.Debug("Calculating functional integral");

    // Bessel functions beyond around here underflow - can get drop off of sig.v, so freeze value at well defined point
    if (xF < 0.01){
        double test = abs( __functionalIntegralResult - __previousFunctionalIntegralResult ) / abs(__previousFunctionalIntegralResult);
        if ( test < 0.02 ){
            return __functionalIntegralResult;
        }
    } 

    // basically cache the previous func_int result
    // if we don't need to reload it, just return the old value
    if (!Reload(xF)){
        nsteps_ += 1;
        return __functionalIntegralResult;
    }
    nsteps_ = 0;
    __previousFunctionalIntegralResult = __functionalIntegralResult;
    double xFinal = xF;
    auto a = modfunc_int_(&xFinal);
    // store result for later
    __previousXF = xF;
    __functionalIntegralResult = a;

    if (std::isnan(a) || std::isinf(a)){
        logger_.Error("Divergent functional integration!");
    }
    return a;
}
