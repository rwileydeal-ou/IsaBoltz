#ifndef AxinoFactory_h
#define AxinoFactory_h

#include <memory>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>
#include <ModelIndependent/BranchingRatios/BranchingRatioFactory.h>
#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <ModelIndependent/CrossSections/CrossSectionFactory.h>
#include <ParticleDefs/Axino/AxinoBr.h>
#include <ParticleDefs/Axino/AxinoSigV.h>

class AxinoBranchingRatioFactory : public BranchingRatioFactory {
public:
    AxinoBranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle >& particles);
    ~AxinoBranchingRatioFactory();
    std::shared_ptr< IBranchingRatio > create_branching_ratio() override;
};

class AxinoCrossSectionFactory : public CrossSectionFactory {
public:
    std::shared_ptr< ISigmaV > create_cross_section() override;
};


#endif
