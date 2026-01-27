#ifndef CallbackTotalWidth_h
#define CallbackTotalWidth_h

#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <sql/ICallbackReturn.h>
#include <Models/TotalWidth.h>

namespace Callbacks
{
    class TotalWidth
    {
    private:
    public:
        static int Callback(void *data, int argc, char **argv, char **colName);
        struct CallbackReturnImplementation : public ICallbackReturn
        {
            std::vector< Models::TotalWidth > TotalWidths;
        } CallbackReturn;
        
        TotalWidth(/* args */);
        ~TotalWidth();
    };
}

#endif
