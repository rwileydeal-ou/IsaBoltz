#include <cmd/CrossSections/Receiver.h>

using namespace std;

CrossSectionReceiver::CrossSectionReceiver(
    Connection& connection, 
    Models::Particle& particle, 
    std::shared_ptr< double > temperature, 
    bool isCohOscField, 
    std::shared_ptr< DataRelay > fortranInterface
) :
    connection_(connection)
{
    particle_ = particle;
    fortranInterface_ = fortranInterface;
    temperature_ = temperature;
    isCohOscField_ = isCohOscField;

    // define and configure factories
    std::shared_ptr< CrossSectionFactory > crossSectionFactorySetup;
    factorySetup(crossSectionFactorySetup, particle_, isCohOscField_);
    crossSectionFactory_ = crossSectionFactorySetup -> create_cross_section();
}

CrossSectionReceiver::~CrossSectionReceiver(){
}

void CrossSectionReceiver::factorySetup(
    std::shared_ptr< CrossSectionFactory >& crossSectionFactory, 
    const Models::Particle& p, 
    bool isCohOscField
){
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
    // calculate branching ratios
    crossSection_ = crossSectionFactory_ -> Calculate( 
        particle_, connection_.Model, *temperature_, fortranInterface_
    );
    crossSection_.InputId = connection_.InputId;
}

void CrossSectionReceiver::UpdateInputs( 
    std::shared_ptr< double > temperature, 
    Models::Particle& particle 
){
    temperature_ = temperature;
    particle_ = particle;
}

SigmaV CrossSectionReceiver::getThermalCrossSection(){
    return crossSection_;
}
