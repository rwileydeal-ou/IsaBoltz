#include <iostream>
#include <cmath>
#include <vector>
#include <Models/ModelBase.h>
#include <Models/PartialWidth.h>

class AxinoWidths
{
protected:
    Models::Particle parent_;
    const ModelBase& model_;
    const std::vector< Models::Particle >& particles_;
    Models::PartialWidth widthGluinoGluon(
        const Models::Particle& gluino, 
        const Models::Particle& gluon, 
        double fa, double alphaS
    );
public:
    AxinoWidths(
        const ModelBase& model, 
        const std::vector< Models::Particle >& particles
    );
    ~AxinoWidths();
    std::vector< Models::PartialWidth > Decays;
};
