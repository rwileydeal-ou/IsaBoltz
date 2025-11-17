#ifndef CallbackParticle_h
#define CallbackParticle_h

#include <deque>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <Models/Particle.h>
#include <sql/ICallbackReturn.h>

namespace Callbacks
{
    class Particle
    {
    private:
    public:
        static int Callback(void *data, int argc, char **argv, char **colName);
        struct CallbackReturnImplementation : public ICallbackReturn
        {
            std::deque< Models::Particle > Particles;
        } CallbackReturn;
        
        Particle(/* args */);
        ~Particle();
    };
}

#endif
