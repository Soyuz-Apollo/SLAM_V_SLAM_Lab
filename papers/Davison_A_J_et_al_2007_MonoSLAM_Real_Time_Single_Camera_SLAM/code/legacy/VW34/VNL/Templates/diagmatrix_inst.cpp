#include <VNL/diagmatrix.tpp>
#include <VNL/complex.h>
#include <VNL/bignum.h>
#include <VNL/rational.h>

VNL_DIAG_MATRIX_INSTANTIATE(int);
VNL_DIAG_MATRIX_INSTANTIATE(float);
VNL_DIAG_MATRIX_INSTANTIATE(double);
VNL_DIAG_MATRIX_INSTANTIATE(long double);
VNL_DIAG_MATRIX_INSTANTIATE(std::complex<double>);
VNL_DIAG_MATRIX_INSTANTIATE(VNL::BigNum);
VNL_DIAG_MATRIX_INSTANTIATE(VNL::Rational);
