#include <iostream>
#include <cmath>
#include <deque>
#include <Models/ModelBase.h>
#include <Models/PartialWidth.h>
#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>

class SaxionWidths
{
protected:
    const ModelBase& model_;
    Models::Particle parent_;
    const std::deque< Models::Particle >& particles_;
    Models::PartialWidth widthGluonGluon(Models::Particle const& gluon, double alphaS, double fa);
    Models::PartialWidth widthGluinoGluino(Models::Particle const& gluino, double alphaS, double fa);
    Models::PartialWidth widthAxionAxion(Models::Particle const& axion, double xi, double fa);
    Models::PartialWidth widthAxinoAxino(Models::Particle const& axino, double xi, double fa);
public:
    SaxionWidths(const ModelBase& model, const std::deque< Models::Particle >& particles);
    ~SaxionWidths();
    std::deque<Models::PartialWidth> Decays;
}; 
