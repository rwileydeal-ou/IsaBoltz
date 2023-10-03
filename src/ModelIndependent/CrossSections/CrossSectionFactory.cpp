#include <ModelIndependent/CrossSections/CrossSectionFactory.h>

std::shared_ptr< ISigmaV > DefaultCrossSectionFactory::create_cross_section(){ return std::make_shared< DefaultSigmaV >(); }

std::shared_ptr< ISigmaV > CrossSectionFromFileFactory::create_cross_section(){ return std::make_shared< CrossSectionFromFile >( "sigmav.csv" ); }
