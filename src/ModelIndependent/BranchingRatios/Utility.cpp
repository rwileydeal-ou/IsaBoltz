#include <ModelIndependent/BranchingRatios/Utility.h>

Utility::Utility(/* args */){}
Utility::~Utility(){}

double Utility::SqrtLambda(double x, double y, double z){
    double result = pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0) - 2.0 * ( x * y + y * z + z * x);
    if (result <= 0.){
        return 0.;
    }
    result = sqrt(result);
    return result;
}
