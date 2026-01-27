#ifndef CallbackTempEquality_h
#define CallbackTempEquality_h

#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <Models/TempEquality.h>
#include <sql/ICallbackReturn.h>

namespace Callbacks
{
    class TempEquality
    {
    private:
    public:
        static int Callback(void *data, int argc, char **argv, char **colName);
        struct CallbackReturnImplementation : public ICallbackReturn
        {
            std::vector<Models::TempEquality> TempEqualities;
        } CallbackReturn;
        
        TempEquality(/* args */);
        ~TempEquality();
    };
}

#endif
