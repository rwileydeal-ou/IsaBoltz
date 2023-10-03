#include <cmd/SolveBoltzmann/InitialConditions/InitialPoint/Receiver.h>

InitialPointReceiver::InitialPointReceiver(Connection& connection, boost::property_tree::ptree& model_tree) :
    connection_(connection),
    outputTree_(model_tree)
{
    initialPoint_.InputId = connection_.InputId;
}

InitialPointReceiver::~InitialPointReceiver(){
}

void InitialPointReceiver::Calculate(){
    initialPoint_.Temperature = connection_.Model.Cosmology.Temperatures.Reheat;
    initialPoint_.GStar = GStar::Calculate(connection_, connection_.Model.Cosmology.Temperatures.Reheat);
    initialPoint_.GStarEntropic = GStar::CalculateEntropic(connection_, connection_.Model.Cosmology.Temperatures.Reheat);
    initialPoint_.ScaleFactor = 1.;
    
    // This is technically S(TR) / R_0^3, we keep this defn for entropy throughout
    initialPoint_.Entropy = 2. * pow( M_PI, 2.) * initialPoint_.GStarEntropic * pow(  connection_.Model.Cosmology.Temperatures.Reheat, 3. ) / 45.;
    initialPoint_.Hubble = sqrt( pow( M_PI, 2.) * initialPoint_.GStar / 90. ) * pow( connection_.Model.Cosmology.Temperatures.Reheat, 2. ) / connection_.Model.Constants.mPlanck;
    initialPoint_.Ordinal = 0;
}

Models::ScaleFactorPoint InitialPointReceiver::getInitialPoint(){
    return initialPoint_;
}
