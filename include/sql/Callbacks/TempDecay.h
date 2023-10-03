#ifndef CallbackTempDecay_h
#define CallbackTempDecay_h

#include <deque>
#include <boost/lexical_cast.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <Models/TempDecay.h>
#include <sql/ICallbackReturn.h>

namespace Callbacks
{
    class TempDecay
    {
    private:
    public:
        static int Callback(void *data, int argc, char **argv, char **colName);
        struct CallbackReturnImplementation : public ICallbackReturn
        {
            std::deque<Models::TempDecay, boost::pool_allocator<Models::TempDecay>> TempDecays;
        } CallbackReturn;
        
        TempDecay(/* args */);
        ~TempDecay();
    };
}

#endif
