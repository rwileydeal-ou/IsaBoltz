#ifndef FiltersTotalWidth_h
#define FiltersTotalWidth_h

#include <sql/IFilter.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace Filters{
    class TotalWidth : public IFilter
    {
    private:
        /* data */
    public:
        TotalWidth(boost::uuids::uuid totalWidthId);
        TotalWidth(boost::uuids::uuid id, WhereUUID whereUUID);
        TotalWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId);
        TotalWidth(boost::uuids::uuid inputId, boost::uuids::uuid parentId, boost::uuids::uuid scaleFactorId);
        ~TotalWidth();
    };
}

#endif
