#ifndef FiltersParticle_h
#define FiltersParticle_h

#include <sql/IFilter.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace Filters{
    class Particle : public IFilter
    {
    private:
        /* data */
    public:
        Particle(boost::uuids::uuid particleId, WhereUUID whereUUID);
        Particle(std::string particleKey, boost::uuids::uuid inputId, SqlComparator comparison = SqlComparator::Equals);
        Particle(std::string particleKey, boost::uuids::uuid particleId, boost::uuids::uuid inputId);
        ~Particle();
    };
}

#endif
