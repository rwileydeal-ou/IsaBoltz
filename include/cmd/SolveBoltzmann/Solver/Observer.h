#ifndef BoltzmannObserver_h
#define BoltzmannObserver_h

#include <memory>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/lexical_cast.hpp>
#include <cmd/SolveBoltzmann/StepBuilder/Command.h>

struct BoltzmannObserver
{
    typedef boost::numeric::ublas::vector< double > state_type;
    BoltzmannStepBuilderCommand& command_;
    Connection& connection_;
    int ordinal_;
    double previousT;

    BoltzmannObserver( BoltzmannStepBuilderCommand& command, Connection& connection ) :
        command_(command),
        connection_(connection)
    { 
        ordinal_ = 0;
        previousT = 0;
    }

    void operator()( const state_type &x , double t )
    {
        if ( t - previousT >= 0.1 || t == 0. || command_.ForcePost() ){
            command_.SetResult();
            int builderOrdinal = command_.getCurrentOrdinal();
            if ( builderOrdinal > ordinal_ ){
                command_.resetCurrentOrdinal( ordinal_ );
                command_.UpdateScaleFactor();
            }
            connection_.Log.Info("Current R/R_0: " + boost::lexical_cast<std::string>( std::exp( t ) ));
            if ( ordinal_ % 1 == 0 ){
                command_.Post();
            }
            previousT = t;
            ++ordinal_;
        }
    }
};

#endif
