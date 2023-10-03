#ifndef PartialWidth_h
#define PartialWidth_h

#include <vector>
#include <deque>
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
        std::deque< boost::uuids::uuid > ChildrenIds;
        double Width = 0.;

        PartialWidth(){}
        PartialWidth(Particle const& parent, std::vector<Particle> const& children, double const& width){
            for (auto &c : children){
                ChildrenIds.push_back( c.Id );
            }
            Id = boost::uuids::random_generator()();
            ParentId = parent.Id;
            InputId = parent.InputId;
            ScaleFactorId = boost::uuids::nil_generator()();
            TotalWidthId = boost::uuids::nil_generator()();
            Width = width;
        }
        PartialWidth(boost::uuids::uuid const& parentId, std::vector< boost::uuids::uuid > const& childrenIds, double const& width, boost::uuids::uuid inputId){
            for (auto &c : childrenIds){
                ChildrenIds.push_back( c );
            }
            Id = boost::uuids::random_generator()();
            ParentId = parentId;
            InputId = inputId;
            ScaleFactorId = boost::uuids::nil_generator()();
            TotalWidthId = boost::uuids::nil_generator()();
            Width = width;
        }
        PartialWidth& operator = (const PartialWidth &d){
            for (auto &c : d.ChildrenIds){
                ChildrenIds.push_back( c );
            }
            Id = d.Id;
            ParentId = d.ParentId;
            InputId = d.InputId;
            ScaleFactorId = d.ScaleFactorId;
            TotalWidthId = d.TotalWidthId;
            Width = d.Width;
            return *this;
        }
    };
}

#endif
