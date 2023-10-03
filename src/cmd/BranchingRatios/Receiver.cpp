#include <cmd/BranchingRatios/Receiver.h>

BranchingRatioReceiver::BranchingRatioReceiver(Connection& connection, Models::Particle& particle, const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles) : 
    connection_(connection),
    particle_(particle),
    particles_(particles)
{
}

BranchingRatioReceiver::~BranchingRatioReceiver(){
}

void BranchingRatioReceiver::factorySetup(std::shared_ptr< BranchingRatioFactory >& branchingRatioFactory){
    if (particle_.Key == "neutralino1"){
        branchingRatioFactory = std::make_shared< NeutralinoBranchingRatioFactory >(connection_.Log, particle_, connection_.Model, particles_);
    } else if (particle_.Key == "gravitino"){
        branchingRatioFactory = std::make_shared< GravitinoBranchingRatioFactory >(connection_.Log, particle_, connection_.Model, particles_);
    } else if (particle_.Key == "axino"){
        branchingRatioFactory = std::make_shared< AxinoBranchingRatioFactory >(connection_.Log, particle_, connection_.Model, particles_);
    } else if (particle_.Key == "saxion"){
        branchingRatioFactory = std::make_shared< SaxionBranchingRatioFactory >(connection_.Log, particle_, connection_.Model, particles_);
    } else if (particle_.Key == "modulus"){
        branchingRatioFactory = std::make_shared< ModulusBranchingRatioFactory >(connection_.Log, particle_, connection_.Model, particles_);
    } else {
        branchingRatioFactory = std::make_shared< DefaultBranchingRatioFactory >(connection_.Log, particle_, connection_.Model, particles_);
    }
}

void BranchingRatioReceiver::Calculate(){
    // define and configure factories
    std::shared_ptr< BranchingRatioFactory > branchingRatio;
    factorySetup(branchingRatio);

    // calculate branching ratios
    auto br = branchingRatio->create_branching_ratio();
    branchingFraction_ = br->Calculate();
    particle_.Width = branchingFraction_.TotalWidth.Width;
}

BranchingFraction BranchingRatioReceiver::getBranchingFraction(){
    return branchingFraction_;
}
