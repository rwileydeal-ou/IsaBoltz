#include <io/util/math/Rotations.h>

boost::numeric::ublas::matrix<double> Rotations::Rotate2d(double theta){
    boost::numeric::ublas::matrix<double> rotation(2,2);
    rotation(0,0) = cos(theta);
    rotation(0,1) = -sin(theta);
    rotation(1,0) = sin(theta);
    rotation(1,1) = cos(theta);
    return rotation;
}

boost::numeric::ublas::matrix<double> Rotations::Rotate3d_X(double theta){
    boost::numeric::ublas::matrix<double> rotation(3,3);
    rotation(0,0) = 1.;
    rotation(0,1) = 0.;
    rotation(0,2) = 0.;

    rotation(1,0) = 0.;
    rotation(1,1) = cos(theta);
    rotation(1,2) = -sin(theta);
    
    rotation(2,0) = 0.;
    rotation(2,1) = sin(theta);
    rotation(2,2) = cos(theta);
    return rotation;
}

boost::numeric::ublas::matrix<double> Rotations::Rotate3d_Y(double theta){
    boost::numeric::ublas::matrix<double> rotation(3,3);
    rotation(0,0) = cos(theta);
    rotation(0,1) = 0.;
    rotation(0,2) = sin(theta);

    rotation(1,0) = 0.;
    rotation(1,1) = 1.;
    rotation(1,2) = 0.;

    rotation(2,0) = -sin(theta);
    rotation(2,1) = 0.;
    rotation(2,2) = cos(theta);
    return rotation;
}

boost::numeric::ublas::matrix<double> Rotations::Rotate3d_Z(double theta){
    boost::numeric::ublas::matrix<double> rotation(2,2);
    rotation(0,0) = cos(theta);
    rotation(0,1) = -sin(theta);
    rotation(0,2) = 0.;

    rotation(1,0) = sin(theta);
    rotation(1,1) = cos(theta);
    rotation(1,2) = 0.;

    rotation(2,0) = 0.;
    rotation(2,1) = 0.;
    rotation(2,2) = 1.;
    return rotation;
}
