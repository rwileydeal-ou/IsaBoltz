#ifndef CallbackCrossSection_h
#define CallbackCrossSection_h

#include <deque>
#include <boost/lexical_cast.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <ModelIndependent/CrossSections/SigmaV.h>
#include <sql/ICallbackReturn.h>

namespace Callbacks
{
    class CrossSection
    {
    private:
    public:
        static int Callback(void *data, int argc, char **argv, char **colName);
        struct CallbackReturnImplementation : public ICallbackReturn
        {
            std::deque<SigmaV, boost::pool_allocator<SigmaV>> crossSections;
        } CallbackReturn;
        
        CrossSection(/* args */);
        ~CrossSection();
    };
}

#endif
