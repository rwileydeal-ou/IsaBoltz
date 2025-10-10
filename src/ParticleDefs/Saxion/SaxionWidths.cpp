#include <ParticleDefs/Saxion/SaxionWidths.h>

SaxionWidths::SaxionWidths(const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles)
    : model_(model),
    particles_(particles)
{
    parent_ = ModelBaseOps::FindConst(particles_, "saxion");
    auto axino = ModelBaseOps::FindConst(particles_, "axino");
    auto axion = ModelBaseOps::FindConst(particles_, "axion");
    auto gluino = ModelBaseOps::FindConst(particles_, "gluino");
    auto gluon = ModelBaseOps::FindConst(particles_, "gluon");

    Decays.push_back(widthGluonGluon(gluon, model_.StandardModel.Couplings.alphaS, model_.PQSector.Fa));
    Decays.push_back(widthGluinoGluino(gluino, model_.StandardModel.Couplings.alphaS, model_.PQSector.Fa));
    Decays.push_back(widthAxionAxion(axion, model_.PQSector.Xi, model_.PQSector.Fa));
    Decays.push_back(widthAxinoAxino(axino, model_.PQSector.Xi, model_.PQSector.Fa));
}
SaxionWidths::~SaxionWidths(){}
 
Models::PartialWidth SaxionWidths::widthGluonGluon(const Models::Particle& gluon, double alphaS, double fa){
    double ggWidth = pow(alphaS, 2.) * pow(parent_.Mass, 3.) / (32. * pow(M_PI, 3.) * pow(fa, 2.) );
    return Models::PartialWidth(parent_, { gluon, gluon }, ggWidth);
}
Models::PartialWidth SaxionWidths::widthGluinoGluino(const Models::Particle& gluino, double alphaS, double fa){
    double glglWidth = 0.;
    if (abs(gluino.Mass) > 0 && parent_.Mass > (2. * abs(gluino.Mass))){
        glglWidth = pow(alphaS, 2.) * parent_.Mass * pow(gluino.Mass, 2.) / (8. * pow(M_PI, 3.) * pow(fa, 2.) ) * pow(1. - 4. * pow(gluino.Mass, 2.) / pow(parent_.Mass, 2.), 3./2.);
    }
    return Models::PartialWidth(parent_, { gluino, gluino }, glglWidth);
}
Models::PartialWidth SaxionWidths::widthAxionAxion(const Models::Particle& axion, double xi, double fa){
    double aaWidth = pow(xi, 2.) * pow(parent_.Mass, 3.) / ( 32. * M_PI * pow(fa, 2.) );
    return Models::PartialWidth(parent_, { axion, axion }, aaWidth);
}
Models::PartialWidth SaxionWidths::widthAxinoAxino(const Models::Particle& axino, double xi, double fa){
    double axaxWidth = 0.;
    if (parent_.Mass > ( 2. * axino.Mass )){
        axaxWidth = pow(xi, 2.) * parent_.Mass * pow(axino.Mass, 2.) * pow( 1. - 4. * pow(axino.Mass, 2.) / pow(parent_.Mass, 2.), 3./2. ) / (4. * M_PI * pow(fa, 2.));
    }
    return Models::PartialWidth(parent_, { axino, axino }, axaxWidth);
}
