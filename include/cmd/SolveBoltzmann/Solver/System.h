#ifndef BoltzmannSystem_h
#define BoltzmannSystem_h

#include <memory>
#include <cmd/SolveBoltzmann/StepBuilder/Command.h>

class System{
private:
    BoltzmannStepBuilderCommand& command_;
public:
    typedef boost::numeric::ublas::vector< double > state_type;
    System( BoltzmannStepBuilderCommand& command ) : 
        command_(command)
    {

    }
    void operator() ( const state_type &x , state_type &dxdt , const double& t ){
        if ( !( command_.IsInitialized() ) ){
            // We need both the Boltzmann equations and the Jacobian of the system
            // Only need to run the command once, then get the result to plug into Boost integrator
            command_.UpdateData( x, t );
            command_.Execute();
        } else{
            command_.Reset();
        }

        dxdt = command_.getDxDt();

    }

};

#endif 
