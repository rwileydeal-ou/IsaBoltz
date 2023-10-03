#include <io/util/math/Pauli.h>

boost::numeric::ublas::matrix<std::complex<double>> Pauli::PauliX(){
    boost::numeric::ublas::matrix<std::complex<double>> pauli(2,2);
    pauli(0,0) = 0;
    pauli(0,1) = 1;
    pauli(1,0) = 1;
    pauli(1,1) = 0;
    return pauli;
}

boost::numeric::ublas::matrix<std::complex<double>> Pauli::PauliY(){
    std::complex<double> i(0.,1.);
    boost::numeric::ublas::matrix<std::complex<double>> pauli(2,2);
    pauli(0,0) = 0;
    pauli(0,1) = -i;
    pauli(1,0) = 0;
    pauli(1,1) = i;
    return pauli;
}

boost::numeric::ublas::matrix<std::complex<double>> Pauli::PauliZ(){
    boost::numeric::ublas::matrix<std::complex<double>> pauli(2,2);
    pauli(0,0) = 1;
    pauli(0,1) = 0;
    pauli(1,0) = 0;
    pauli(1,1) = -1;
    return pauli;
}