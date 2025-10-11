#ifndef DataRelay_h
#define DataRelay_h

#include <Models/ModelBase.h>
#include <io/Logging/Logger.h>
#include <cmath>
#include <bitset>
#include <iostream>
#include <stdexcept>
#include <dlfcn.h>
#include <chrono>

struct Sugxin{
    float xisain[24], xsugin[7], xgmin[14], xnrin[4], xamin[11];
};

struct Sugmg{
    float mss[32], gss[31], mgutss, ggutss, agutss, ftgut, fbgut, ftagut, fngut;
};

struct Sspar{
    bool gorge;
    float amglss, amulss, amurss, amdlss, amdrss, amslss, amsrss, amclss, amcrss, amblss, ambrss, amb1ss, amb2ss, amtlss, amtrss, amt1ss, amt2ss, amelss, amerss, ammlss, ammrss, amllss, amlrss, aml1ss, aml2ss, amn1ss, amn2ss, amn3ss, twom1, rv2v1, amz1ss, amz2ss, amz3ss, amz4ss, zmixss[4][4], amw1ss, amw2ss, gammal, gammar, amhl, amhh, amha, amhc, alfah, aat, thetat, aab, thetab, aal, thetal, amgvss, mtq, mbq, mlq, fbma, vuq, vdq, sgnm3;
};

struct Sugpas{
    float xtanb, msusy, amt, mgut, mu, g2, gp, v, vp, xw, a1mz, a2mz, asmz, ftamz, fbmz, b, sin2b, ftmt, g3mt, vev, higfrz, fnmz, amnrmj;
    int nogood, ial3un, itachy, mhpneg, mhlneg, mhcneg, msqneg, igutst;
    float asm3, vumt, vdmt, asmtp, asmss, m3q, mhdsq, mhusq, mhdsmg, mhusmg, mumg, bmg, ft2z1, fb2z1, fl2z1, sigdmx, sigumx, c5max, chsmax;
};

struct Sugnu{
    float xnusug[20], mds, m10s;
    int inuhm, idterm;
};

class DataRelay
{
private:
    Logger& logger_;
    double __previousXF;
    double __functionalIntegralResult;
    double __previousFunctionalIntegralResult;
    int nsteps_;
    void retrieveMasses(ParticleMasses& masses, ModelBase& model, PecceiQuinnParams& pqParams);
    void retrieveMixingAngles(SusyParams& susy);
    void retrieveCouplings(SusyParams& susy);
    void setNonUniversalSugra(ModelBase model);

    void *handle;
    void (*definedata_)();
    void (*modsugra_)(float *m0, float *mhf, float *a0, float *tanb, float *sgnmu, float *mt, int *imodel);
    void (*sugra_)(float *m0, float *mhf, float *a0, float *tanb, float *sgnmu, float *mt, int *imodel);
    void (*ssmssm_)(float *xmg, float *xmu, float *xmha, float *xtanb, float *xmq1, float *xmdr, float *xmur, float *xml1, float *xmer, float *xmq2, float *xmsr, float *xmcr, float *xml2, float *xmmr, float *xmq3, float *xmbr, float *xmtr, float *xml3, float *xmlr, float *xat, float *xab, float *xal, float *xm1, float *xm2, float *xmt, int *iallow, int *imodel, int *imhl, int *imhc, int *imsq);
    void (*findlsp_)(int *idlsp);
    void (*printnogood_)(int *imodel);

    // FUNC_INT translation
    double (*modfunc_int_)(double *xfinput);

    // IsaTools
    void (*isares_)(int *iprt);
    void (*isared_)(int *iprt);
    void (*isaamu_)(float *srv2v1, float *salemi, float *sgammal, float *sgammar, float *stwom1, float *saal, float *sammlss, float *sammrss, float *samz1ss, float *samz2ss, float *samz3ss, float *samz4ss, float *samw1ss, float *samw2ss, float *samn2ss, float (*szmixss)[4][4], int *iprt, double *damu);
    void (*isabsg_)(int *imodel, float *m0, float *mhf, float *a0, double *bfbsg, int *iwr);
    void (*isabtn_)(float *tanb, float *mhc, float *mu, float *mgl, float *mb1, float *mb2, float *r, float *bfbtn);

    // Tests
    void (*sstest_)(int *iallow);
    
    void readIAllow(int iallow);
    void setSugraData(ModelBase& model);
public:
    void RunIsaAmu(int& isaVal, double& isaValDbl);
    void RunIsaBsg(int& isaVal, int& imodel, const ModelBase& model);
    void RunIsaBtn(const ModelBase& model);
    void RunIsaRes(int& isaVal);
    void RunIsaRed(int& isaVal);
    void RunSugra(ModelBase& model);
    void InitializeMssmSpectra(ModelBase& model, int& iallow);
    void FindLsp(ModelBase& model);
    void TestParameters(ModelBase& model, int& iallow);
    double CalculateFunctionalIntegral(double& xF);
    bool Reload(double xF);

    // common blocks
    Sugxin *sugxin_;
    Sugmg *sugmg_;
    Sugpas *sugpas_;
    Sugnu *sugnu_;
    Sspar *sspar_;
    TrhPars *trhpars_;

    void Retrieve(ModelBase& model);
    DataRelay(Logger& logger) : 
        logger_(logger)
    {
        __previousXF = 0;
        __functionalIntegralResult = 0.;
        __previousFunctionalIntegralResult = 0.;
        nsteps_ = 0;
        // first initialize data so fortran can read stuff
        handle = dlopen("./bin/fortLib.so", RTLD_NOW);
        if (!handle){
            logger_.Error(dlerror());
            throw std::exception();
        }
        definedata_ = (void (*)())dlsym(handle, "definedata_");
        modsugra_ = (void (*)(float *m0, float *mhf, float *a0, float *tanb, float *sgnmu, float *mt, int *imodel))dlsym(handle, "modsugra_");
        sugra_ = (void (*)(float *m0, float *mhf, float *a0, float *tanb, float *sgnmu, float *mt, int *imodel))dlsym(handle, "sugra_");
        ssmssm_ = (void (*)(float *xmg, float *xmu, float *xmha, float *xtanb, float *xmq1, float *xmdr, float *xmur, float *xml1, float *xmer, float *xmq2, float *xmsr, float *xmcr, float *xml2, float *xmmr, float *xmq3, float *xmbr, float *xmtr, float *xml3, float *xmlr, float *xat, float *xab, float *xal, float *xm1, float *xm2, float *xmt, int *iallow, int *imodel, int *imhl, int *imhc, int *imsq))dlsym(handle, "ssmssm_");
        findlsp_ = (void (*)(int *id))dlsym(handle, "findlsp_");
        printnogood_ = (void (*)(int *imodel))dlsym(handle, "printnogood_");

        isares_ = (void (*)(int *iprt))dlsym(handle, "isares_");
        isared_ = (void (*)(int *iprt))dlsym(handle, "isared_");
        isaamu_ = (void (*)(float *srv2v1, float *salemi, float *sgammal, float *sgammar, float *stwom1, float *saal, float *sammlss, float *sammrss, float *samz1ss, float *samz2ss, float *samz3ss, float *samz4ss, float *samw1ss, float *samw2ss, float *samn2ss, float (*szmixss)[4][4], int *iprt, double *damu))dlsym(handle, "isaamu_");
        isabsg_ = (void (*)(int *imodel, float *m0, float *mhf, float *a0, double *bfbsg, int *iwr))dlsym(handle, "isabsg_");
        isabtn_ = (void (*)(float *tanb, float *mhc, float *mu, float *mgl, float *mb1, float *mb2, float *r, float *bfbtn))dlsym(handle, "isabtn_");

        sstest_ = (void (*)(int *iallow))dlsym(handle, "sstest_");

        modfunc_int_ = (double (*)(double *xfinput))dlsym(handle, "modfunc_int_");

        sugnu_ = (Sugnu*) dlsym(handle, "sugnu_");
        sugxin_ = (Sugxin*) dlsym(handle, "sugxin_");
        sugmg_ = (Sugmg*) dlsym(handle, "sugmg_");
        sugpas_ = (Sugpas*) dlsym(handle, "sugpas_");
        sspar_ = (Sspar*) dlsym(handle, "sspar_");
        trhpars_ = (TrhPars*) dlsym(handle, "trhpars_");
        definedata_();
    };
    ~DataRelay(){
        dlclose(handle);
    };
};

#endif
