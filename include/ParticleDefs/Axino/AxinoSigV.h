#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <ParticleDefs/Common/DFSZ/SigVDFSZ.h>

class AxinoSigV : public ISigmaV
{
public:
    SigmaV Calculate(const Models::Particle& axino, const ModelBase& model, double T, std::shared_ptr< DataRelay > fortInterface) override;
    AxinoSigV(/* args */);
    ~AxinoSigV();
};
 