#ifndef CrossSectionFromFile_h
#define CrossSectionFromFile_h

#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <ModelIndependent/CrossSections/SigmaV.h>
#include <ModelIndependent/CrossSections/ISigmaV.h>

class CrossSectionFromFile : public ISigmaV
{
private:
    std::string fileName_;
public:
    SigmaV Calculate(const Models::Particle& neutralino, const ModelBase& model, double T, std::shared_ptr< DataRelay > fortInterface) override;
    CrossSectionFromFile( std::string fileName );
    ~CrossSectionFromFile();
};

#endif
