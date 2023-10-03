#ifndef AxionFactory_h
#define AxionFactory_h

#include <memory>
#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <ModelIndependent/CrossSections/CrossSectionFactory.h>
#include <ParticleDefs/Axion/AxionSigV.h>

class AxionCrossSectionFactory : public CrossSectionFactory {
public:
    std::shared_ptr< ISigmaV > create_cross_section() override;
};


#endif
