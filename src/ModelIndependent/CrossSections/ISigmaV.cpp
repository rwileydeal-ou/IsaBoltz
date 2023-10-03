#include <ModelIndependent/CrossSections/ISigmaV.h>

ISigmaV::ISigmaV(){}
ISigmaV::~ISigmaV(){}

void ISigmaV::handleInvalidModel(){
    throw std::logic_error("Select valid axion model!");
}
