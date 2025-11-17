#ifndef CallbackTempOsc_h
#define CallbackTempOsc_h

#include <deque>
#include <boost/lexical_cast.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <Models/TempOsc.h>
#include <sql/ICallbackReturn.h>

namespace Callbacks
{
    class TempOsc
    {
    private:
    public:
        static int Callback(void *data, int argc, char **argv, char **colName);
        struct CallbackReturnImplementation : public ICallbackReturn
        {
            std::deque<Models::TempOscillation> TempOscs;
        } CallbackReturn;
        
        TempOsc(/* args */);
        ~TempOsc();
    };
}

#endif
