#ifndef TotalWidth_h
#define TotalWidth_h

#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/Particle.h>

namespace Models{
    struct TotalWidth
    {
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        boost::uuids::uuid ParentId;
        boost::uuids::uuid ScaleFactorId;
        std::vector< boost::uuids::uuid > PartialWidthIds;
        double Width = 0.;

        TotalWidth()
        :
            Id(boost::uuids::random_generator()())
        {
        }
        TotalWidth(
            Particle const& parent, 
            std::vector< boost::uuids::uuid > const& partialWidthIds, 
            double const& width
        ) :
            Id(boost::uuids::random_generator()()),
            InputId(parent.InputId),
            ParentId(parent.Id),
            ScaleFactorId(boost::uuids::nil_generator()()),
            PartialWidthIds(partialWidthIds),
            Width(width)
        {
            for (auto &c : partialWidthIds){
                PartialWidthIds.push_back( c );
            }
        }
        TotalWidth(
            boost::uuids::uuid const& parentId, 
            std::vector< boost::uuids::uuid > const& partialWidthIds, 
            double const& width, 
            boost::uuids::uuid inputId
        ) :
            Id(boost::uuids::random_generator()()),
            InputId(inputId),
            ParentId(parentId),
            ScaleFactorId(boost::uuids::nil_generator()()),
            PartialWidthIds(partialWidthIds),
            Width(width)
        {
        }
    };
}

#endif
