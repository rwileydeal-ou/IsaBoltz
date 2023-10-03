#ifndef BoltzmannData_h
#define BoltzmannData_h

#include <deque>
#include <vector>
#include <Models/ParticleEvolution.h>
#include <Models/Particle.h>
#include <Models/ScaleFactor.h>
#include <Models/PartialWidth.h>
#include <Models/TotalWidth.h>
#include <cmd/SolveBoltzmann/StepBuilder/ParticleData.h>

struct BoltzmannData
{
    const std::vector< ParticleData, boost::pool_allocator<ParticleData> >& ParticleDatas;
    const Models::ScaleFactorPoint& CurrentPoint; 
    const Models::ScaleFactorPoint& ReheatPoint;
    const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& Particles;
    const std::map< std::string, std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > >& PartialWidths;
    
    BoltzmannData(
        const std::vector< ParticleData, boost::pool_allocator<ParticleData> >& particleDatas,
        const Models::ScaleFactorPoint& currentPoint,
        const Models::ScaleFactorPoint& reheatPoint,
        const std::deque< Models::Particle, boost::pool_allocator<Models::Particle> >& particles,
        const std::map< std::string, std::deque< Models::PartialWidth, boost::pool_allocator<Models::PartialWidth> > >& partialWidths
    ) :
    ParticleDatas(particleDatas),
    CurrentPoint(currentPoint),
    ReheatPoint(reheatPoint),
    Particles(particles),
    PartialWidths(partialWidths)
    {
    }

};

#endif  
