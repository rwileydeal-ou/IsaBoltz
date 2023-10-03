#ifndef TotalWidth_h
#define TotalWidth_h

#include <vector>
#include <deque>
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
        std::deque< boost::uuids::uuid > PartialWidthIds;
        double Width = 0.;

        TotalWidth(){
            Id = boost::uuids::random_generator()();
        }
        TotalWidth(Particle const& parent, std::vector< boost::uuids::uuid > const& partialWidthIds, double const& width){
            for (auto &c : partialWidthIds){
                PartialWidthIds.push_back( c );
            }
            Id = boost::uuids::random_generator()();
            ParentId = parent.Id;
            InputId = parent.InputId;
            ScaleFactorId = boost::uuids::nil_generator()();
            Width = width;
        }
        TotalWidth(boost::uuids::uuid const& parentId, std::vector< boost::uuids::uuid > const& partialWidthIds, double const& width, boost::uuids::uuid inputId){
            for (auto &c : partialWidthIds){
                PartialWidthIds.push_back( c );
            }
            Id = boost::uuids::random_generator()();
            ParentId = parentId;
            InputId = inputId;
            ScaleFactorId = boost::uuids::nil_generator()();
            Width = width;
        }
        TotalWidth& operator = (const TotalWidth &d){
            for (auto &c : d.PartialWidthIds){
                PartialWidthIds.push_back( c );
            }
            Id = d.Id;
            ParentId = d.ParentId;
            InputId = d.InputId;
            ScaleFactorId = d.ScaleFactorId;
            Width = d.Width;
            return *this;
        }
    };
}

#endif
