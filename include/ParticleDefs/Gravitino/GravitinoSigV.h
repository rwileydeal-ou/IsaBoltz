#include <ModelIndependent/CrossSections/ISigmaV.h>
#include <io/Logging/Logger.h>

class GravitinoSigV : public ISigmaV
{
public:
    SigmaV Calculate(const Models::Particle& gravitino, const ModelBase& model, double T, std::shared_ptr< DataRelay > fortInterface) override;
    GravitinoSigV(/* args */);
    ~GravitinoSigV();
};
 