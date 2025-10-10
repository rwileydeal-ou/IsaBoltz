#include <ModelIndependent/BranchingRatios/IBranchingRatio.h>

using namespace std;

IBranchingRatio::IBranchingRatio(Logger& logger, const Models::Particle& parent, const ModelBase& model, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles) 
    : logger_(logger),
    model_(model),
    parent_(parent),
    particles_(particles)
{

}
IBranchingRatio::~IBranchingRatio(){}

Models::TotalWidth IBranchingRatio::calculateTotalWidth(deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> >& partialWidths){
    double totalDecayWidth = 0.;
    Models::TotalWidth totalWidth;
    if ( partialWidths.size() == 0 ){
        logger_.Warn("No partial widths detected!");
        return totalWidth;
    }

    totalWidth.InputId = partialWidths.front().InputId;
    totalWidth.ParentId = partialWidths.front().ParentId;
    totalWidth.ScaleFactorId = partialWidths.front().ScaleFactorId;

    for(auto &width : partialWidths){
        if ( width.Width < 0. ){
            logger_.Warn("Detected negative partial decay width!");
        }
        totalDecayWidth += width.Width;
        width.TotalWidthId = totalWidth.Id;
        totalWidth.PartialWidthIds.push_back( width.Id );
    }
    if (totalDecayWidth < 0.){
        logger_.Warn("Detected negative total decay width!");
    }
    totalWidth.Width = totalDecayWidth;

    return totalWidth;
}

void IBranchingRatio::handleInvalidModel(){
    throw std::logic_error("Select valid axion model!");
}
