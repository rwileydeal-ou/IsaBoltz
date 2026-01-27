#ifndef BranchingRatioReceiver_h
#define BranchingRatioReceiver_h

#include <vector>
#include <iostream>
#include <boost/pool/pool_alloc.hpp>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <ModelIndependent/BranchingRatios/BranchingRatioFactory.h>
#include <ParticleDefs/Axino/AxinoFactory.h>
#include <ParticleDefs/Axion/AxionFactory.h>
#include <ParticleDefs/Gravitino/GravitinoFactory.h>
#include <ParticleDefs/Modulus/ModulusFactory.h>
#include <ParticleDefs/Neutralino/NeutralinoFactory.h>
#include <ParticleDefs/Saxion/SaxionFactory.h>
#include <sql/DbManager.h>
#include <sql/Statements/PartialWidth.h>
#include <sql/Statements/TotalWidth.h>

class BranchingRatioReceiver : public IReceiver
{
private: 
    Connection& connection_;
    Models::Particle& particle_;
    BranchingFraction branchingFraction_;
    const std::vector< Models::Particle >& particles_;
    void factorySetup(std::shared_ptr< BranchingRatioFactory >& branchingRatioFactory);
public:
    ~BranchingRatioReceiver();
    BranchingRatioReceiver(
        Connection& connection, 
        Models::Particle& particle, 
        const std::vector< Models::Particle >& particles
    );
    void Calculate() override;
    BranchingFraction getBranchingFraction();
};

#endif
