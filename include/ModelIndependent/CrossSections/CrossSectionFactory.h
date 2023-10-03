#ifndef CrossSectionFactory_h
#define CrossSectionFactory_h

#include <memory>
#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <ModelIndependent/CrossSections/CrossSectionFromFile.h>

class CrossSectionFactory{
public:
    virtual std::shared_ptr< ISigmaV > create_cross_section() = 0;
};

class DefaultCrossSectionFactory : public CrossSectionFactory {
public:
    std::shared_ptr< ISigmaV > create_cross_section() override;
};

class CrossSectionFromFileFactory : public CrossSectionFactory {
public:
    std::shared_ptr< ISigmaV > create_cross_section() override;
};

#endif
