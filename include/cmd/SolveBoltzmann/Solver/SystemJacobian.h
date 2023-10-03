#ifndef BoltzmannSystemJacobian_h
#define BoltzmannSystemJacobian_h

#include <memory>
#include <cmd/SolveBoltzmann/StepBuilder/Command.h>

class SystemJacobian{
private:
    BoltzmannStepBuilderCommand& command_;
public:
    typedef boost::numeric::ublas::vector< double > state_type;
    typedef boost::numeric::ublas::matrix< double > matrix_type;
    SystemJacobian( BoltzmannStepBuilderCommand& command ) : 
        command_(command)
    {

    }
    void operator() ( const state_type &x , matrix_type &J , const double& t , state_type &dfdt ){
        if ( !( command_.IsInitialized() ) ){
            // We need both the Boltzmann equations and the Jacobian of the system
            // Only need to run the command once, then get the result to plug into Boost integrator
            command_.UpdateData( x, t );
            command_.Execute();
        } else{
            command_.Reset();
        }

        J = command_.getJacobian();

    }

};

#endif
