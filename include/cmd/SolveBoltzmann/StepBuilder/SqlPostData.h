#ifndef BoltzSqlData_h
#define BoltzSqlData_h

#include <deque>
#include <boost/pool/pool_alloc.hpp>
#include <Models/ParticleEvolution.h>
#include <Models/Particle.h>
#include <Models/ScaleFactor.h>
#include <Models/PartialWidth.h>
#include <Models/TotalWidth.h>
#include <cmd/SolveBoltzmann/StepBuilder/ParticleData.h>

struct BoltzSqlData
{
    std::deque< ParticleData> ParticleDatas;
    std::deque< Models::PartialWidth > PartialWidths;
    std::deque< Models::TotalWidth > TotalWidths;
    std::deque< Models::ScaleFactorPoint > ScaleFactors;
};

#endif  
