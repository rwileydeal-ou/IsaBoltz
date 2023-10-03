#ifndef InputParams_h
#define InputParams_h

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

struct InputParams
{
    boost::uuids::uuid Id;

    InputParams(boost::uuids::uuid id){
        Id = id;
    }
};

#endif
