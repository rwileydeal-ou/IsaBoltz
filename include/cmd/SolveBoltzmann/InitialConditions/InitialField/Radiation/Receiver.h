#ifndef InitialRadiationReceiver_h
#define InitialRadiationReceiver_h

#include <memory>
#include <iostream>
#include <boost/math/special_functions.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/ScaleFactor.h>
#include <Models/ParticleEvolution.h>
#include <io/Exceptions/NotImplemented.h>
#include <ModelIndependent/Densities/NumberDensityEq.h>

class RadiationReceiver : public IReceiver
{
private: 
    Connection& connection_;
    std::shared_ptr< Models::ScaleFactorPoint > initialPoint_;
    Models::Particle particle_;
    Models::ParticleEvolution boltzmannParticleEvolution_;
public:
    ~RadiationReceiver();
    RadiationReceiver(Connection& connection, std::shared_ptr< Models::ScaleFactorPoint > initialPoint, const Models::Particle& particle);
    void Calculate() override;
    Models::ParticleEvolution getBoltzmannParticleEvolution();
};

#endif
