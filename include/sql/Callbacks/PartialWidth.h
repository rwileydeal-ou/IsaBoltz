#ifndef CallbackPartialWidth_h
#define CallbackPartialWidth_h

#include <deque>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <Models/PartialWidth.h>
#include <sql/ICallbackReturn.h>

namespace Callbacks
{
    class PartialWidth
    {
    private:
    public:
        static int Callback(void *data, int argc, char **argv, char **colName);
        struct CallbackReturnImplementation : public ICallbackReturn
        {
            std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > PartialWidths;
        } CallbackReturn;
        
        PartialWidth(/* args */);
        ~PartialWidth();
    };
}

#endif
