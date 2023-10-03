#include <sql/Callbacks/BoltzmannParticleEvolution.h>

using namespace Callbacks;

ParticleEvolution::ParticleEvolution(/* args */)
{
}    
ParticleEvolution::~ParticleEvolution()
{
}

int ParticleEvolution::Callback(void *data, int argc, char **argv, char **colName){
    try{
        CallbackReturnImplementation *cb = (static_cast< CallbackReturnImplementation* > (data) );

        Models::ParticleEvolution particleEvolution;
        particleEvolution.Id = boost::lexical_cast<boost::uuids::uuid>(argv[0]);
        particleEvolution.InputId = boost::lexical_cast<boost::uuids::uuid>(argv[1]);
        particleEvolution.ParticleKey = boost::lexical_cast<std::string>(argv[2]);
        particleEvolution.ParticleId = boost::lexical_cast<boost::uuids::uuid>(argv[3]);
        particleEvolution.ProductionMechanism = static_cast<ParticleProductionMechanism>( std::stoi( argv[4] ) );
        particleEvolution.IsActive = boost::lexical_cast<bool>(argv[5]);
        particleEvolution.Y1 = boost::lexical_cast<double>(argv[6]);
        particleEvolution.Y2 = boost::lexical_cast<double>(argv[7]);

        particleEvolution.NumberDensity = boost::lexical_cast<double>(argv[8]);
        particleEvolution.EquilibriumNumberDensity = boost::lexical_cast<double>(argv[9]);
        particleEvolution.EnergyDensity = boost::lexical_cast<double>(argv[10]);
        particleEvolution.ScaleFactorId = boost::lexical_cast<boost::uuids::uuid>(argv[11]);

        if ( argv[12] != nullptr ){
            particleEvolution.IsOscillating = boost::lexical_cast<bool>(argv[12]);
        }
        if ( argv[13] != nullptr ){
            particleEvolution.IsDecaying = boost::lexical_cast<bool>(argv[13]);
        }

        cb->ParticleEvolutions.push_back(particleEvolution);
    } catch (std::exception& e){
        throw_with_trace(e);
    }
    return 0;
}
