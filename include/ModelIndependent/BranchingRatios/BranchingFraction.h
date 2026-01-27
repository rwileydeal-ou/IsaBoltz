#ifndef BranchingFraction_h
#define BranchingFraction_h

#include <vector>
#include <Models/TotalWidth.h>
#include <Models/PartialWidth.h>
#include <Models/Particle.h>
#include <boost/pool/pool_alloc.hpp>

struct BranchingFraction
{
    Models::TotalWidth TotalWidth;
    std::vector< Models::PartialWidth > PartialWidths;
    
    BranchingFraction(){
    }
};

#endif
