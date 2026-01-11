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
        J = command_.getJacobian();
    }
    void operator() (const state_type &x, matrix_type &J, const double& t)
    {
        state_type dummy(x.size());
        this->operator()(x, J, t, dummy);
    }
};

#endif
