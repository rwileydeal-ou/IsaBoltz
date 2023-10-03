#ifndef FiltersTempDecay_h
#define FiltersTempDecay_h

#include <sql/IFilter.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace Filters{
    class TempDecay : public IFilter
    {
    private:
        /* data */
    public:
        TempDecay(boost::uuids::uuid tempDecayId);
        TempDecay(boost::uuids::uuid inputId, boost::uuids::uuid particleId);
        TempDecay(boost::uuids::uuid inputId, boost::uuids::uuid particleId, boost::uuids::uuid scaleFactorId);
        ~TempDecay();
    };
}

#endif
