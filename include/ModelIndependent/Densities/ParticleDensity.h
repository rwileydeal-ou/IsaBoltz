#ifndef ParticleDensity_h
#define ParticleDensity_h

struct ParticleDensity
{
    int PdgCode = 0.;
    bool Active;
    long double NumberDensity = 0.;
    long double NumberDensityEquilibrium = 0.;
    long double RhoN = 0.;
    long double Rho = 0.;
    long double PreRelicDensity = 0.;
    long double RelicDensity = 0.;
    
    ParticleDensity(const int pdgCode, bool isActive, long double nDens, long double nEquil, long double rhoN, long double rho){
        PdgCode = pdgCode;
        Active = isActive;
        NumberDensity = nDens;
        NumberDensityEquilibrium = nEquil;
        RhoN = rhoN; 
        Rho = rho;
    }
    ParticleDensity(){}
};


#endif 
