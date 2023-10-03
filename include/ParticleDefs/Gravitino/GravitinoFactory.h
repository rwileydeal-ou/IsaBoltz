#ifndef GravitinoFactory_h
#define GravitinoFactory_h

#include <memory>
#include <boost/pool/pool_alloc.hpp>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>
#include <ModelIndependent/BranchingRatios/BranchingRatioFactory.h>
#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <ModelIndependent/CrossSections/CrossSectionFactory.h>
#include <ParticleDefs/Gravitino/GravitinoBr.h>
#include <ParticleDefs/Gravitino/GravitinoSigV.h>

class GravitinoBranchingRatioFactory : public BranchingRatioFactory {
public:
    GravitinoBranchingRatioFactory(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles);
    ~GravitinoBranchingRatioFactory();
    std::shared_ptr< IBranchingRatio > create_branching_ratio() override;
};

class GravitinoCrossSectionFactory : public CrossSectionFactory {
public:
    std::shared_ptr< ISigmaV > create_cross_section() override;
};


#endif
