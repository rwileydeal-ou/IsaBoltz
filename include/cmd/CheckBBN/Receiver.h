#ifndef CheckBBNReceiver_h
#define CheckBBNReceiver_h

#include <iostream>
#include <io/FileIO.h>
#include <io/Input/FileParser.h>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <Models/CheckBBN.h>
#include <Models/TotalWidth.h>
#include <Models/PartialWidth.h>
#include <sql/DbManager.h>
#include <sql/Statements/ScaleFactor.h>
#include <sql/Callbacks/ScaleFactor.h>
#include <sql/Filters/ScaleFactor.h>
#include <sql/Statements/TotalWidth.h>
#include <sql/Callbacks/TotalWidth.h>
#include <sql/Filters/TotalWidth.h>
#include <sql/Statements/PartialWidth.h>
#include <sql/Callbacks/PartialWidth.h>
#include <sql/Filters/PartialWidth.h>
#include <sql/Statements/Particle.h>
#include <sql/Callbacks/Particle.h>
#include <sql/Filters/Particle.h>
#include <sql/Statements/BoltzmannParticleEvolution.h>
#include <sql/Callbacks/BoltzmannParticleEvolution.h>
#include <sql/Filters/BoltzmannParticleEvolution.h>

class CheckBBNReceiver : public IReceiver
{
private: 
    Connection& connection_;
    Models::Particle particle_;
    Models::ParticleEvolution particleEvolution_;
    Models::CheckBBN checkBBN_;
    Models::TotalWidth totalWidth_;
    Models::TotalWidth pullTotalWidth();
    Models::ScaleFactorPoint scaleFactor_;
    std::vector< Models::Particle > hadrons_;
    std::vector< Models::Particle > pullHadrons();
    std::vector< Models::PartialWidth > partialWidths_;
    std::vector< Models::PartialWidth > pullPartialWidths();

    unsigned int indexBr();
    double hadronicBranchingRatio();
    double relicDensityBeforeDecay();
    double interpolatedOmega(std::string infile, double logLifetime);
    
    std::vector<std::vector<double>> parseBbnData(std::string infile);

public:
    ~CheckBBNReceiver();
    CheckBBNReceiver(
        Connection& connection, 
        Models::Particle& particle, 
        const Models::ParticleEvolution& particleEvolution, 
        const Models::ScaleFactorPoint& scaleFactor, 
        const Models::TotalWidth& totalWidth
    );
    void Calculate() override;
    Models::CheckBBN getCheckBBN();
};

#endif
