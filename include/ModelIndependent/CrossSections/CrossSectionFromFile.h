#ifndef CrossSectionFromFile_h
#define CrossSectionFromFile_h

#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <ModelIndependent/CrossSections/SigmaV.h>
#include <ModelIndependent/CrossSections/ISigmaV.h>

class CrossSectionFromFile : public ISigmaV
{
public:
    explicit CrossSectionFromFile(std::string fileName);
    ~CrossSectionFromFile();
    SigmaV Calculate(const Models::Particle& particle,
                     const ModelBase& model,
                     double T,
                     std::shared_ptr<DataRelay> fortInterface) override;

private:
    std::string fileName_;
    std::vector<double> temps_;
    std::vector<double> sigmas_;
};

#endif
