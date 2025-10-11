/*
#include <io/FileIO.h>
#include <io/Input/FileParser.h>
#include <Models/ModelBase.h>
#include <Models/ParticleEvolution.h>


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
    static double hadronicBranchingRatio(const Models::ParticleEvolution& particle, const Models::ParticleEvolution& radiation, const ModelBase& model);
    static double radiationPreRelicDensity(const Models::ParticleEvolution& particle, const Models::ParticleEvolution& radiation, const ModelBase& model);
    static double interpolatedOmega(DataElement data, double logLifetime);
    static void bbnOK(Models::ParticleEvolution& particle, const Models::ParticleEvolution& radiation, const ModelBase& model);
    static std::vector<std::vector<double>> parseBbnData(std::string infile);
public:
    static void CheckConstraint(const ModelBase& model, const ScaleFactor& finalPoint);
    CheckBBN();
    ~CheckBBN();
};
*/
