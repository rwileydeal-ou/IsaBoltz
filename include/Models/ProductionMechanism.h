#ifndef ProductionMechanism_h
#define ProductionMechanism_h

#include <ostream>

enum class ParticleProductionMechanism{
    NONE=-1,
    THERMAL=0,
    COHERENT_OSCILLATION=1,
    RADIATION=2,
    ANY=3
};

inline std::string ToString(ParticleProductionMechanism s)
{
    switch (s) {
        case ParticleProductionMechanism::NONE: return "-1";
        case ParticleProductionMechanism::THERMAL :   return "0";
        case ParticleProductionMechanism::COHERENT_OSCILLATION: return "1";
        case ParticleProductionMechanism::RADIATION: return "2";
        case ParticleProductionMechanism::ANY: return "3";
    }
    return "Unknown";
}

inline std::ostream& operator<<(
    std::ostream& os,
    ParticleProductionMechanism s
){
    return os << ToString(s);
}

#endif
