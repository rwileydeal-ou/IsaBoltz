#include <iostream>
#include <cmath>
#include <deque>
#include <Models/ModelBase.h>
#include <Models/PartialWidth.h>

class AxinoWidths
{
protected:
    Models::Particle parent_;
    const ModelBase& model_;
    const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles_;
    Models::PartialWidth widthGluinoGluon(const Models::Particle& gluino, const Models::Particle& gluon, double fa, double alphaS);
public:
    AxinoWidths(const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles);
    ~AxinoWidths();
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > Decays;
};
