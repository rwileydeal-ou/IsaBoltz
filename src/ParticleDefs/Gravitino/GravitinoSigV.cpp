#include <ParticleDefs/Gravitino/GravitinoSigV.h>

using namespace std;

GravitinoSigV::GravitinoSigV(/* args */){}
GravitinoSigV::~GravitinoSigV(){ }

SigmaV GravitinoSigV::Calculate(
    const Models::Particle& gravitino, 
    const ModelBase& model, 
    double T, 
    std::shared_ptr< DataRelay > fortInterface
){
    double trhg1 = model.RunningParameters.trhg1v[0];
    double trhg2 = model.RunningParameters.trhg2v[0];
    double trhg3 = model.RunningParameters.trhg3v[0];
    double trhm1 = model.RunningParameters.trhm1v[0];
    double trhm2 = model.RunningParameters.trhm2v[0];
    double trhm3 = model.RunningParameters.trhm3v[0];
    double trhq = model.RunningParameters.tqv[0];
    double drh = abs(model.RunningParameters.tqv[0] - T);

    for (int i = 0; i < 10000 - 1; ++i){
        if (model.RunningParameters.tqv[i+1] == 0.){ 
            continue; 
        }
        if (drh >= abs(model.RunningParameters.tqv[i+1] - T) ){
            drh = abs(model.RunningParameters.tqv[i+1] - T);
            trhg1 = model.RunningParameters.trhg1v[i+1];
            trhg2 = model.RunningParameters.trhg2v[i+1];
            trhg3 = model.RunningParameters.trhg3v[i+1];
            trhm1 = model.RunningParameters.trhm1v[i+1];
            trhm2 = model.RunningParameters.trhm2v[i+1];
            trhm3 = model.RunningParameters.trhm3v[i+1];
            trhq = model.RunningParameters.tqv[i+1];
        }
    }
 
    double crossSec = ( 1.37/ pow(model.Constants.mPlanck * sqrt(8 * M_PI), 2.) ) 
    * ( 
        72. * log(1.271/( trhg3 )) * pow(trhg3, 2.) 
        * ( 
            1. + pow(trhm3, 2.) / (3. * pow(gravitino.Mass, 2.))
        ) 
        + 
        27. * log(1.312/( trhg2 )) * pow(trhg2, 2.) 
        * ( 
            1. + pow(trhm2, 2.) / (3. * pow(gravitino.Mass, 2.))
        ) 
        + 
        11. * log(1.266/( trhg1 * sqrt(3./5.) )) * pow(trhg1 * sqrt(3./5.), 2.) 
        * (
            1. + pow(trhm1, 2.) / (3. * pow(gravitino.Mass, 2.))
        )
    );

    return SigmaV( gravitino, T, crossSec);
}
