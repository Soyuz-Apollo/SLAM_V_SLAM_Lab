#include <VNL/VC6WarningsFix.h>
#include <VNL/vector.tpp>
#include <VNL/vectorref.tpp>
#include <VNL/bignum.h>
#include <VNL/complex.h>
#include <VNL/rational.h>

// Standard types
VNL_VECTOR_INSTANTIATE(int);
VNL_VECTOR_INSTANTIATE(unsigned int);
VNL_VECTOR_INSTANTIATE(float);
VNL_VECTOR_INSTANTIATE(double);
VNL_VECTOR_INSTANTIATE(signed char);
VNL_VECTOR_INSTANTIATE(unsigned char);
VNL_VECTOR_INSTANTIATE(long);
VNL_VECTOR_INSTANTIATE(unsigned long);
VNL_VECTOR_INSTANTIATE(long double);
// as complex angles don't make much sense, need a slightly different one
VNL_VECTOR_INSTANTIATE_COMPLEX(std::complex<double>);

// VNL types
VNL_VECTOR_INSTANTIATE(VNL::BigNum);
VNL_VECTOR_INSTANTIATE(VNL::Rational);


