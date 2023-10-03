#ifndef ScaleFactorPoint_h
#define ScaleFactorPoint_h

#include <vector>
#include <algorithm>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Models
{
    struct ScaleFactorPoint{
        boost::uuids::uuid Id;
        boost::uuids::uuid InputId;
        double Temperature = 0.;
        long double ScaleFactor = 0.;
        long double Entropy = 0.;
        long double Hubble = 0.;
        long double GStar = 0.;
        long double GStarEntropic = 0.;
        int Ordinal = 0;

        ScaleFactorPoint(){
            Id = boost::uuids::random_generator()();
            InputId = boost::uuids::nil_generator()();
        }
        ScaleFactorPoint(const ScaleFactorPoint& s){
            Id = s.Id;
            InputId = s.InputId;
            Temperature = s.Temperature;
            ScaleFactor = s.ScaleFactor;
            Entropy = s.Entropy;
            Hubble = s.Hubble;
            GStar = s.GStar;
            GStarEntropic = s.GStarEntropic;
            Ordinal = s.Ordinal;
        }
        ScaleFactorPoint& operator = (const ScaleFactorPoint& s){
            Id = s.Id;
            InputId = s.InputId;
            Temperature = s.Temperature;
            ScaleFactor = s.ScaleFactor;
            Entropy = s.Entropy;
            Hubble = s.Hubble;
            GStar = s.GStar;
            GStarEntropic = s.GStarEntropic;
            Ordinal = s.Ordinal;

            return *this;
        }
    };
}

#endif
