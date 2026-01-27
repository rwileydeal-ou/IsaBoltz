#ifndef CallbackScaleFactor_h
#define CallbackScaleFactor_h

#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <Models/ScaleFactor.h>
#include <sql/ICallbackReturn.h>

namespace Callbacks
{
    class ScaleFactor
    {
    private:
    public:
        static int Callback(void *data, int argc, char **argv, char **colName);
        struct CallbackReturnImplementation : public ICallbackReturn
        {
            std::vector<Models::ScaleFactorPoint> ScaleFactors;
        } CallbackReturn;
        
        ScaleFactor(/* args */);
        ~ScaleFactor();
    };
}

#endif
