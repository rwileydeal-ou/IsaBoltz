#ifndef BranchingFraction_h
#define BranchingFraction_h

#include <deque>
#include <Models/TotalWidth.h>
#include <Models/PartialWidth.h>
#include <Models/Particle.h>
#include <boost/pool/pool_alloc.hpp>

struct BranchingFraction
{
    Models::TotalWidth TotalWidth;
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > PartialWidths;
    
    BranchingFraction(const BranchingFraction &b){
        TotalWidth = b.TotalWidth;
        for (auto &d : b.PartialWidths){
            PartialWidths.push_back(d);
        }
    }
    BranchingFraction& operator = (const BranchingFraction &b){
        TotalWidth = b.TotalWidth;
        for (auto &d : b.PartialWidths){
            PartialWidths.push_back(d);
        }
        return *this;
    }
    BranchingFraction(){
    }
};

#endif
