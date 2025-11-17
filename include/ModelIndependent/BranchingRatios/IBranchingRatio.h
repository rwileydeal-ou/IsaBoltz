#ifndef IBranchingRatio_h
#define IBranchingRatio_h

#include <deque>
#include <Models/ModelBase.h>
#include <Models/PartialWidth.h>
#include <Models/TotalWidth.h>
#include <ModelIndependent/BranchingRatios/BranchingFraction.h>
#include <ModelIndependent/BranchingRatios/Utility.h>
#include <io/Logging/Logger.h>

class IBranchingRatio
{
protected:
    Logger& logger_;
    const ModelBase& model_;
    const Models::Particle& parent_;
    const std::deque< Models::Particle >& particles_;
    Models::TotalWidth calculateTotalWidth(std::deque< Models::PartialWidth >& partialWidths);
    void handleInvalidModel(); 
public:
    virtual BranchingFraction Calculate() = 0;
    IBranchingRatio(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle >& particles);
    ~IBranchingRatio();
};

class DefaultBranchingRatio : public IBranchingRatio{
public: 
    DefaultBranchingRatio(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle >& particles) 
        : IBranchingRatio(logger, parent, model, particles){}
    ~DefaultBranchingRatio(){}
    BranchingFraction Calculate() override{
        auto br = BranchingFraction();
        br.TotalWidth.ParentId = parent_.Id;
        br.TotalWidth.InputId = parent_.InputId;
        return br;
    }
};

#endif
