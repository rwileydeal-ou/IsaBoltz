#include <io/FileIO.h>
#include <io/Input/FileParser.h>
#include <Models/ModelBase.h>
#include <Models/ScaleFactor.h>

/*
class CheckBBN
{
private:
    struct DataElement{
        double UpperLifetime = 0.;
        double UpperOmega = 0.;
        double LowerLifetime = 0.;
        double LowerOmega = 0.;
    };
    static DataElement findNearestData(std::string infile, double BRhad, double logLifetime);
    static int posNearestBr(double BRhad);
    static double hadronicBranchingRatio(ScaleFactorParticleProperties particle, ScaleFactorParticleProperties radiation, ModelBase model);
    static double radiationPreRelicDensity(ScaleFactorParticleProperties particle, ScaleFactorParticleProperties radiation, ModelBase model);
    static double interpolatedOmega(DataElement data, double logLifetime);
    static void bbnOK(ScaleFactorParticleProperties& particle, ScaleFactorParticleProperties radiation, ModelBase model);
    static std::vector<std::vector<double>> parseBbnData(std::string infile);
public:
    static void CheckConstraint(ModelBase model, ScaleFactor& finalPoint);
    CheckBBN();
    ~CheckBBN();
};
*/