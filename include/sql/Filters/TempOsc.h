#ifndef FiltersTempOsc_h
#define FiltersTempOsc_h

#include <sql/IFilter.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace Filters{
    class TempOsc : public IFilter
    {
    private:
        /* data */
    public:
        TempOsc(boost::uuids::uuid tempOscId);
        TempOsc(boost::uuids::uuid inputId, boost::uuids::uuid particleId);
        TempOsc(boost::uuids::uuid inputId, boost::uuids::uuid particleId, boost::uuids::uuid scaleFactorId);
        ~TempOsc();
    };
}

#endif
