



// Some extras we'll need for correct instantiation of hybrid types
#include <VNL/bignum.h>
#include <VNL/rational.h>
#include <VNL/complex.h>

// The main classes
#include <VNL/cvector.tpp>



// System types
VNL_C_VECTOR_INSTANTIATE_ordered(int);
VNL_C_VECTOR_INSTANTIATE_ordered(float);
VNL_C_VECTOR_INSTANTIATE_ordered(double);
VNL_C_VECTOR_INSTANTIATE_ordered(long);
VNL_C_VECTOR_INSTANTIATE_ordered(long double);
VNL_C_VECTOR_INSTANTIATE_ordered(signed char);
VNL_C_VECTOR_INSTANTIATE_ordered(unsigned int);
VNL_C_VECTOR_INSTANTIATE_ordered(unsigned long);
VNL_C_VECTOR_INSTANTIATE_ordered(unsigned char);

// The standard complex type has no > and < operators, so use
// the unordered version
VNL_C_VECTOR_INSTANTIATE_unordered(std::complex<double>);

// VNL types
VNL_C_VECTOR_INSTANTIATE_ordered(VNL::BigNum);
VNL_C_VECTOR_INSTANTIATE_ordered(VNL::Rational);
