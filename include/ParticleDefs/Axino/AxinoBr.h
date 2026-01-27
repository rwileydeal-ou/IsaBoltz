#ifndef AxinoBr_h
#define AxinoBr_h

#include <vector>
#include <Models/PartialWidth.h>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>
#include <ParticleDefs/Axino/AxinoWidthsDFSZ.h>

class AxinoBr : public IBranchingRatio
{
private:
    std::vector< Models::PartialWidth > handleDFSZ();
public:
    AxinoBr(
        Logger& logger, 
        const Models::Particle& parent, 
        const ModelBase& model, 
        const std::vector< Models::Particle >& particles
    );
    ~AxinoBr();
    BranchingFraction Calculate() override;
    /*
        FOR FUTURE REFERENCE - ALSO HAS:
c...Add up decays to R-odd particles (in MSSM only):
C      AXODD = AXNH + AXCH + TGL
      AXNH = AXNHH + AXNHL + AXNHA
      AXODD = AXNH + AXCH
      AXTEMP = AXNZ + AXCW + AXFSF  !!!!!!********to be FIXED: not classified as ODD or EVEN, Hasan
    */
};

#endif
