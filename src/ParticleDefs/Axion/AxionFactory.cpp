#include <ParticleDefs/Axion/AxionFactory.h>

std::shared_ptr< ISigmaV > AxionCrossSectionFactory::create_cross_section(){ return std::make_shared<AxionSigV>(); }
