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
        command_.UpdateData( x, t );
        command_.Execute();
        dxdt = command_.getDxDt();

    }

};

#endif 
