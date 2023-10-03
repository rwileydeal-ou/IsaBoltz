#ifndef CallbackParticleEvolution_h
#define CallbackParticleEvolution_h

#include <deque>
#include <boost/lexical_cast.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <Models/ParticleEvolution.h>
#include <sql/ICallbackReturn.h>
#include <io/Logging/Logger.h>

namespace Callbacks
{
    class ParticleEvolution
    {
    private:
    public:
        static int Callback(void *data, int argc, char **argv, char **colName);
        struct CallbackReturnImplementation : public ICallbackReturn
        {
            std::deque<Models::ParticleEvolution, boost::pool_allocator<Models::ParticleEvolution>> ParticleEvolutions;
        } CallbackReturn;
        
        ParticleEvolution(/* args */);
        ~ParticleEvolution();
    };
}

#endif
