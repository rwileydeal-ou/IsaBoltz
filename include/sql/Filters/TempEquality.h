#ifndef FiltersTempEquality_h
#define FiltersTempEquality_h

#include <sql/IFilter.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace Filters{
    class TempEquality : public IFilter
    {
    private:
        /* data */
    public:
        TempEquality(boost::uuids::uuid tempEqualityId);
        TempEquality(boost::uuids::uuid inputId, boost::uuids::uuid particleId);
        TempEquality(boost::uuids::uuid inputId, boost::uuids::uuid particleId, boost::uuids::uuid scaleFactorId);
        ~TempEquality();
    };
}

#endif
