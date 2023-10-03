#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <ParticleDefs/Common/DFSZ/SigVDFSZ.h>

class AxionSigV : public ISigmaV
{
public:
    SigmaV Calculate(const Models::Particle& axion, const ModelBase& model, double T, std::shared_ptr< DataRelay > fortInterface) override;
    AxionSigV(/* args */);
    ~AxionSigV();
};
 