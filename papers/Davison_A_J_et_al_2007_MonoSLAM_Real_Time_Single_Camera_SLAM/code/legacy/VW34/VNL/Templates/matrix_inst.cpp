#ifdef _WIN32
#pragma warning(disable:4275) // disable C4275 warning
#endif

// Some extras we'll need for correct instantiation of hybrid types
#include <VNL/bignum.h>
#include <VNL/rational.h>
#include <VNL/complex.h>

// The main classes
#include <VNL/matrixref.h>
#include <VNL/matrix.tpp>


// Standard types
VNL_MATRIX_INSTANTIATE(double);
VNL_MATRIX_INSTANTIATE(float);
VNL_MATRIX_INSTANTIATE(int);
VNL_MATRIX_INSTANTIATE(long);
VNL_MATRIX_INSTANTIATE(long double);
VNL_MATRIX_INSTANTIATE(signed char);
VNL_MATRIX_INSTANTIATE(unsigned char);
VNL_MATRIX_INSTANTIATE(std::complex<double>);
VNL_MATRIX_INSTANTIATE(unsigned int);
VNL_MATRIX_INSTANTIATE(unsigned long);

// VNL types
VNL_MATRIX_INSTANTIATE(VNL::BigNum);
VNL_MATRIX_INSTANTIATE(VNL::Rational);


