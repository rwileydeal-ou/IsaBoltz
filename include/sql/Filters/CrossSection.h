#ifndef FiltersCrossSection_h
#define FiltersCrossSection_h

#include <sql/IFilter.h>
#include <Models/ProductionMechanism.h> 
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace Filters{
    class CrossSection : public IFilter
    {
    private:
        /* data */
    public:
        CrossSection(boost::uuids::uuid crossSectionId);
        CrossSection(boost::uuids::uuid inputId, boost::uuids::uuid particleId, boost::uuids::uuid scaleFactorId);
        CrossSection(ParticleProductionMechanism productionMechanism, boost::uuids::uuid scaleFactorId);
        ~CrossSection();
    };
}

#endif
