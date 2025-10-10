#include <ParticleDefs/Axino/AxinoWidths.h>

using namespace std;

AxinoWidths::AxinoWidths(const ModelBase& model, const deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles)
    : model_(model),
    particles_(particles)
{
    parent_ = ModelBaseOps::FindConst(particles_, "axino");
    auto gluino = ModelBaseOps::FindConst(particles_, "gluino");
    auto gluon = ModelBaseOps::FindConst(particles_, "gluon");
    Decays.push_back(widthGluinoGluon(gluino, gluon, model_.PQSector.Fa, model_.StandardModel.Couplings.alphaS));
}
AxinoWidths::~AxinoWidths(){}

Models::PartialWidth AxinoWidths::widthGluinoGluon(const Models::Particle& gluino, const Models::Particle& gluon, double fa, double alphaS){
    double tgl = 0;
    if (abs(gluino.Mass) < abs(parent_.Mass)){
        tgl = 8.0 * pow(alphaS, 2.0) * pow( abs(parent_.Mass), 3.0 ) * pow( (1.0 - pow( (gluino.Mass / parent_.Mass), 2.0) ), 3.0 ) / (128.0 * pow( M_PI, 3.0) * pow( fa, 2.0));
    }
    auto width = Models::PartialWidth(parent_, { gluino, gluon }, tgl);
    return width;
}
