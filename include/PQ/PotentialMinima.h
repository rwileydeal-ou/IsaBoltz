#include <complex>
#include <Models/ModelBase.h>
#include <io/Logging/Logger.h>

class PotentialMinima
{
private:
    typedef std::complex<double> dcmplx;
    static dcmplx cck_potential_solver(ModelBase model, dcmplx x);
    static PecceiQuinnPotential cck_potential(ModelBase model, double vX);
public:
    static void Minimize( ModelBase& model, Logger& logger );
    PotentialMinima();
    ~PotentialMinima();
};
