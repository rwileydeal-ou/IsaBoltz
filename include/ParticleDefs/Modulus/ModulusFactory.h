#ifndef ModulusFactory_h
#define ModulusFactory_h

#include <memory>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>
#include <ModelIndependent/BranchingRatios/BranchingRatioFactory.h>
#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <ModelIndependent/CrossSections/CrossSectionFactory.h>
#include <ParticleDefs/Modulus/ModulusBr.h>

class ModulusBranchingRatioFactory : public BranchingRatioFactory {
public:
    ModulusBranchingRatioFactory(
        Logger& logger, 
        const Models::Particle& parent, 
        const ModelBase& model, 
        const std::vector< Models::Particle >& particles
    );
    ~ModulusBranchingRatioFactory();
    std::shared_ptr< IBranchingRatio > create_branching_ratio() override;
};

#endif
