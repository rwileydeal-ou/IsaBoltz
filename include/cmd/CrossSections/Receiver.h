#ifndef CrossSectionReceiver_h
#define CrossSectionReceiver_h

#include <iostream>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <ModelIndependent/CrossSections/CrossSectionFactory.h>
#include <ParticleDefs/Axino/AxinoFactory.h>
#include <ParticleDefs/Axion/AxionFactory.h>
#include <ParticleDefs/Gravitino/GravitinoFactory.h>
#include <ParticleDefs/Modulus/ModulusFactory.h>
#include <ParticleDefs/Neutralino/NeutralinoFactory.h>
#include <ParticleDefs/Saxion/SaxionFactory.h>

class CrossSectionReceiver : public IReceiver
{
private: 
    Connection& connection_;
    Models::Particle particle_;
    SigmaV crossSection_;
    std::shared_ptr< ISigmaV > crossSectionFactory_;
    std::shared_ptr< DataRelay > fortranInterface_;
    std::shared_ptr< double > temperature_;
    bool isCohOscField_;
    void factorySetup(
        std::shared_ptr< CrossSectionFactory >& crossSectionFactory, 
        const Models::Particle& p, 
        bool isCohOscField
    );
public:
    ~CrossSectionReceiver();
    CrossSectionReceiver(
        Connection& connection, 
        Models::Particle& particle, 
        std::shared_ptr< double > temperature, 
        bool isCohOscField, 
        std::shared_ptr< DataRelay > fortranInterface
    );
    void Calculate() override;
    void UpdateInputs( 
        std::shared_ptr< double > temperature, 
        Models::Particle& particle 
    );
    SigmaV getThermalCrossSection();
};

#endif
