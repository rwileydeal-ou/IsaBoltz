#ifndef FiltersPartialWidth_h
#define FiltersPartialWidth_h

#include <sql/IFilter.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace Filters{
    class PartialWidth : public IFilter
    {
    private:
        /* data */
    public:
        PartialWidth(boost::uuids::uuid partialWidthId);
        PartialWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId);
        PartialWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId, boost::uuids::uuid scaleFactorId);
        PartialWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId, std::vector< boost::uuids::uuid > childrenIds);
        PartialWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId, std::vector< boost::uuids::uuid > childrenIds, boost::uuids::uuid scaleFactorId);
        ~PartialWidth();
    };
}

#endif
