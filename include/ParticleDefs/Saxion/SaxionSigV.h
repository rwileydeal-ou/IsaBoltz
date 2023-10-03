#include <memory>
#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <ParticleDefs/Common/DFSZ/SigVDFSZ.h>

class SaxionSigV : public ISigmaV
{
public:
    SigmaV Calculate(const Models::Particle& saxion, const ModelBase& model, double T, std::shared_ptr< DataRelay > fortInterface) override;
    SaxionSigV(/* args */);
    ~SaxionSigV();
};
 