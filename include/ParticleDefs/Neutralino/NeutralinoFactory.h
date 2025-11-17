#ifndef NeutralinoFactory_h
#define NeutralinoFactory_h

#include <memory>
#include <boost/pool/pool_alloc.hpp>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>
#include <ModelIndependent/BranchingRatios/BranchingRatioFactory.h>
#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <ModelIndependent/CrossSections/CrossSectionFactory.h>
#include <ParticleDefs/Neutralino/NeutralinoBr.h>
#include <ParticleDefs/Neutralino/NeutralinoSigV.h>

class NeutralinoBranchingRatioFactory : public BranchingRatioFactory {
public:
    NeutralinoBranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle >& particles);
    ~NeutralinoBranchingRatioFactory();
    std::shared_ptr< IBranchingRatio > create_branching_ratio() override;
};

class NeutralinoCrossSectionFactory : public CrossSectionFactory {
public:
    std::shared_ptr< ISigmaV > create_cross_section() override;
};


#endif
