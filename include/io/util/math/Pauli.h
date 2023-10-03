#ifndef Pauli_h
#define Pauli_h

#include <boost/numeric/ublas/matrix.hpp>
#include <complex>

class Pauli
{
private:
    /* data */
public:
    static boost::numeric::ublas::matrix<std::complex<double>> PauliX();
    static boost::numeric::ublas::matrix<std::complex<double>> PauliY();
    static boost::numeric::ublas::matrix<std::complex<double>> PauliZ();
    Pauli(/* args */);
    ~Pauli();
};

#endif
