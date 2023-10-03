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
    std::deque< ParticleData, boost::pool_allocator<ParticleData> > ParticleDatas;
    std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > PartialWidths;
    std::deque< Models::TotalWidth, boost::pool_allocator<Models::TotalWidth> > TotalWidths;
    std::deque< Models::ScaleFactorPoint, boost::pool_allocator<Models::ScaleFactorPoint> > ScaleFactors;
};

#endif  
