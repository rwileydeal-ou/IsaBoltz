#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

class Rotations
{
private:
    /* data */
public:
    static boost::numeric::ublas::matrix<double> Rotate2d(double theta);
    static boost::numeric::ublas::matrix<double> Rotate3d_X(double theta);
    static boost::numeric::ublas::matrix<double> Rotate3d_Y(double theta);
    static boost::numeric::ublas::matrix<double> Rotate3d_Z(double theta);
    Rotations(/* args */);
    ~Rotations();
};
