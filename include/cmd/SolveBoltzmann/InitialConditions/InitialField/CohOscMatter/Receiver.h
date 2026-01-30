#ifndef InitialCohOscMatterReceiver_h
#define InitialCohOscMatterReceiver_h

#include <iostream>
#include <memory>
#include <boost/math/special_functions.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/ParticleEvolution.h>
#include <Models/ScaleFactor.h>
#include <io/Exceptions/NotImplemented.h>
#include <ModelIndependent/Densities/NumberDensityEq.h>

class CohOscMatterReceiver : public IReceiver
{
private: 
    Connection& connection_;
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint_;
    Models::Particle particle_;
    Models::ParticleEvolution boltzmannParticleEvolution_;
    double y1Initial(bool isOn, double mass, double entropyInitial, double rho0);
    double y2Initial(double mass, double tempReheat);
    double tempOscillate(double mass, double tempReheat, double hubble);
    double rho0(
        Models::Particle& particle, 
        const ModelBase& model, 
        std::shared_ptr< Models::ScaleFactorPoint > initialPoint
    );
public:
    ~CohOscMatterReceiver();
    CohOscMatterReceiver(
        Connection& connection, 
        std::shared_ptr< Models::ScaleFactorPoint > initialPoint, 
        const Models::Particle& particle
    );
    void Calculate() override;
    Models::ParticleEvolution getBoltzmannParticleEvolution();
};

#endif
