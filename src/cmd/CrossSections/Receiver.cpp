#include <cmd/CrossSections/Receiver.h>

using namespace std;

CrossSectionReceiver::CrossSectionReceiver(Connection& connection, Models::Particle& particle, std::shared_ptr< double > temperature, bool isCohOscField, std::shared_ptr< DataRelay > fortranInterface) :
    connection_(connection)
{
    particle_ = particle;
    fortranInterface_ = fortranInterface;
    temperature_ = temperature;
    isCohOscField_ = isCohOscField;
}

CrossSectionReceiver::~CrossSectionReceiver(){
}

void CrossSectionReceiver::factorySetup(std::shared_ptr< CrossSectionFactory >& crossSectionFactory, const Models::Particle& p, bool isCohOscField){
    if (p.Key == "neutralino1"){
        if(true){
            crossSectionFactory = std::make_shared< CrossSectionFromFileFactory >();
        } else{
            crossSectionFactory = std::make_shared< NeutralinoCrossSectionFactory >();
        }
    } else if (p.Key == "gravitino"){
        crossSectionFactory = std::make_shared< GravitinoCrossSectionFactory >();
    } else if (p.Key == "axino"){
        crossSectionFactory = std::make_shared< AxinoCrossSectionFactory >();
    } else if (p.Key == "axion" && !isCohOscField){
        crossSectionFactory = std::make_shared< AxionCrossSectionFactory >();
    } else if (p.Key == "saxion" && !isCohOscField){
        crossSectionFactory = std::make_shared< SaxionCrossSectionFactory >();
    } else{
        crossSectionFactory = std::make_shared< DefaultCrossSectionFactory >();
    }
}

void CrossSectionReceiver::Calculate(){
    // define and configure factories
    std::shared_ptr< CrossSectionFactory > crossSection;
    factorySetup(crossSection, particle_, isCohOscField_);

    // calculate branching ratios
    auto a = crossSection -> create_cross_section();
    crossSection_ = a -> Calculate( 
        particle_, connection_.Model, *temperature_, fortranInterface_
    );
    crossSection_.InputId = connection_.InputId;
}

SigmaV CrossSectionReceiver::getThermalCrossSection(){
    return crossSection_;
}
