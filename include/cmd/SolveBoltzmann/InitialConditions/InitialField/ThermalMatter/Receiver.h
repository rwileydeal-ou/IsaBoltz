#ifndef InitialThermalMatterReceiver_h
#define InitialThermalMatterReceiver_h

#include <memory>
#include <iostream>
#include <boost/math/special_functions.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/ScaleFactor.h>
#include <Models/ParticleEvolution.h>
#include <ModelIndependent/Densities/NumberDensityEq.h>
#include <sql/DbManager.h>
#include <sql/Statements/CrossSection.h>
#include <sql/Callbacks/CrossSection.h>
#include <sql/Filters/CrossSection.h>

class ThermalMatterReceiver : public IReceiver
{
private: 
    Connection& connection_;
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint_;
    Models::Particle particle_;
    Models::ParticleEvolution boltzmannParticleEvolution_;
    double y1Initial(double tempDecouple, double crossSection, double nEq, double hubble, double entropyInitial);
    double y2Initial(double mass, double tempReheat, Models::ParticleStatistics stats);
    double tempDecouple(double crossSection, double nEq, double tempReheat, double hubble);
    SigmaV getCrossSection();
public:
    ~ThermalMatterReceiver();
    ThermalMatterReceiver(Connection& connection, std::shared_ptr< Models::ScaleFactorPoint > initialPoint, const Models::Particle& particle);
    void Calculate() override;
    Models::ParticleEvolution getBoltzmannParticleEvolution();
};

#endif
