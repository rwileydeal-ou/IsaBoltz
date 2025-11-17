#ifndef SaxionFactory_h
#define SaxionFactory_h

#include <memory>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>
#include <ModelIndependent/BranchingRatios/BranchingRatioFactory.h>
#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <ModelIndependent/CrossSections/CrossSectionFactory.h>
#include <ParticleDefs/Saxion/SaxionBr.h>
#include <ParticleDefs/Saxion/SaxionSigV.h>

class SaxionBranchingRatioFactory : public BranchingRatioFactory {
public:
    SaxionBranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle >& particles);
    ~SaxionBranchingRatioFactory();
    std::shared_ptr< IBranchingRatio > create_branching_ratio() override;
};

class SaxionCrossSectionFactory : public CrossSectionFactory {
public:
    std::shared_ptr< ISigmaV > create_cross_section() override;
};


#endif
