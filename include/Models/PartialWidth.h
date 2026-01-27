#ifndef PartialWidth_h
#define PartialWidth_h

#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Models/Particle.h>

namespace Models{
    struct PartialWidth
    {
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        boost::uuids::uuid TotalWidthId;
        boost::uuids::uuid ParentId;
        boost::uuids::uuid ScaleFactorId;
        std::vector< boost::uuids::uuid > ChildrenIds;
        double Width = 0.;

        PartialWidth(){}
        PartialWidth(
            Particle const& parent, 
            std::vector<Particle> const& children, 
            double const& width
        ) :
            Id(boost::uuids::random_generator()()),
            InputId(parent.InputId),
            TotalWidthId(boost::uuids::nil_generator()()),
            ParentId(parent.Id),
            ScaleFactorId(boost::uuids::nil_generator()()),
            Width(width)
        {
            for (auto &c : children){
                ChildrenIds.push_back( c.Id );
            }
        }
        PartialWidth(
            boost::uuids::uuid const& parentId, 
            std::vector< boost::uuids::uuid > const& childrenIds, 
            double const& width, 
            boost::uuids::uuid inputId
        ) :
            Id(boost::uuids::random_generator()()),
            InputId(inputId),
            TotalWidthId(boost::uuids::nil_generator()()),
            ParentId(parentId),
            ScaleFactorId(boost::uuids::nil_generator()()),
            ChildrenIds(childrenIds),
            Width(width)
        {
        }
    };
}

#endif
