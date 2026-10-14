#include <VNL/det.tpp>
#include <VNL/complex.h>
#include <VNL/bignum.h>
#include <VNL/rational.h>


// Standard types
VNL_DET_INSTANTIATE(float);
VNL_DET_INSTANTIATE(double);
VNL_DET_INSTANTIATE(long double);
VNL_DET_INSTANTIATE(std::complex<double>);

// VNL types
VNL_DET_INSTANTIATE(VNL::BigNum);
VNL_DET_INSTANTIATE(VNL::Rational);
