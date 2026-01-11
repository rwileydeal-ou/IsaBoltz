#ifndef BoltzmannUtilPartialWidth_h
#define BoltzmannUtilPartialWidth_h

#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <Models/PartialWidth.h>
#include <Models/Particle.h>

struct BoltzmannPartialWidth : Models::PartialWidth
{
    std::vector<Models::Charge> Charges;
    BoltzmannPartialWidth(){
    }

    BoltzmannPartialWidth( 
        const Models::PartialWidth& partialWidth ,
        std::vector<Models::Charge> const& charges
    ) :
        Charges( charges ),
        PartialWidth( partialWidth )
    {
    }
};


#endif
