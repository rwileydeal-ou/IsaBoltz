#ifndef FiltersScaleFactor_h
#define FiltersScaleFactor_h

#include <sql/IFilter.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace Filters{
    class ScaleFactor : public IFilter
    {
    private:
        /* data */
    public:
        ScaleFactor(boost::uuids::uuid uuid, WhereUUID whereUUID);
        ScaleFactor(int ordinal, boost::uuids::uuid inputId);
        ~ScaleFactor();
    };
}

#endif
