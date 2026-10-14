#include <VNL/vector.tpp>
#include <VNL/vectorref.tpp>
#include <VNL/bignum.h>
#include <VNL/complex.h>
#include <VNL/rational.h>


// Standard types
VNL_VECTOR_REF_INSTANTIATE(int);
VNL_VECTOR_REF_INSTANTIATE(unsigned int);
VNL_VECTOR_REF_INSTANTIATE(float);
VNL_VECTOR_REF_INSTANTIATE(double);
VNL_VECTOR_REF_INSTANTIATE(signed char);
VNL_VECTOR_REF_INSTANTIATE(unsigned char);
VNL_VECTOR_REF_INSTANTIATE(long);
VNL_VECTOR_REF_INSTANTIATE(unsigned long);
VNL_VECTOR_REF_INSTANTIATE(long double);
VNL_VECTOR_REF_INSTANTIATE(std::complex<double>);

// VNL types
VNL_VECTOR_REF_INSTANTIATE(VNL::BigNum);
VNL_VECTOR_REF_INSTANTIATE(VNL::Rational);

