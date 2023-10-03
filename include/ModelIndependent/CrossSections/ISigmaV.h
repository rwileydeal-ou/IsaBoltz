#ifndef ISigmaV_h
#define ISigmaV_h

#include <memory>
#include <Models/ModelBase.h>
#include <DataRelay.h>
#include <ModelIndependent/CrossSections/SigmaV.h>

class ISigmaV {
protected:
    void handleInvalidModel();
public:
    ISigmaV();
    ~ISigmaV();
    virtual SigmaV Calculate(const Models::Particle& particle, const ModelBase& model, double T, std::shared_ptr< DataRelay > fortInterface) = 0;
};

class DefaultSigmaV : public ISigmaV {
public:
    DefaultSigmaV(){}
    ~DefaultSigmaV(){}
    SigmaV Calculate(const Models::Particle& particle, const ModelBase& model, double T, std::shared_ptr< DataRelay > fortInterface ) override{
        return SigmaV(particle, T, 0.);
    }
};

#endif
