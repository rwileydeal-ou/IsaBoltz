#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <io/Logging/Logger.h>
#include <DataRelay.h>

class NeutralinoSigV : public ISigmaV
{
public:
    SigmaV Calculate(const Models::Particle& neutralino, const ModelBase& model, double T, std::shared_ptr< DataRelay > fortInterface) override;
    NeutralinoSigV();
    ~NeutralinoSigV();
};
 