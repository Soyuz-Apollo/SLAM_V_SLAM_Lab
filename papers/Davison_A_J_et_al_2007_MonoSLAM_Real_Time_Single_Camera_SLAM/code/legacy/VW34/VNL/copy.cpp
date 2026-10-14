// This is vxl/VNL/copy.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author fsm@robots.ox.ac.uk
*/
#include <VNL/VC6WarningsFix.h>
#include "copy.h"
#include <assert.h>
#include <complex>
#include <VNL/vector.h>
#include <VNL/matrix.h>
#include <VNL/diagmatrix.h>

//-------------------------------------------------------------------

template <class S, class T>
void VNL::Copy(S const *src, T *dst, unsigned n)
{
  for (unsigned int i=0; i<n; ++i)
    dst[i] = T(src[i]);
}

template <class S, class T>
void VNL::Copy(S const &src, T &dst)
{
  assert(src.size() == dst.size());
  VNL::Copy(src.begin(), dst.begin(), src.size());
}

//------------------------------------------------------------------------

// C arrays
#define VNL_COPY_INSTANTIATE0(S, T) \
namespace VNL{template void Copy(S const *, T *, unsigned );};

VNL_COPY_INSTANTIATE0(float, double);
VNL_COPY_INSTANTIATE0(double, float);
VNL_COPY_INSTANTIATE0(double, long double);
VNL_COPY_INSTANTIATE0(long double, double);

#define macro(S, D) \
namespace VNL { \
template <> \
void Copy(std::complex<S> const *src, std::complex<D> *dst, unsigned n) \
{ \
  for (unsigned int i=0; i<n; ++i) \
    dst[i] = std::complex<D>(std::real(src[i]), std::imag(src[i])); \
}; \
};
macro(float, double);
macro(double, float);
macro(double, long double);
macro(long double, double);
#undef macro

// vnl_* containers
#define VNL_COPY_INSTANTIATE(S, T) \
namespace VNL { \
template void Copy(VNL::Vector<S > const &, VNL::Vector<T > &); \
template void Copy(VNL::Matrix<S > const &, VNL::Matrix<T > &); \
template void Copy(VNL::DiagMatrix<S > const &, VNL::DiagMatrix<T > &); \
};

#define VNL_COPY_INSTANTIATE_twoway(S, T) \
VNL_COPY_INSTANTIATE(S, T); \
VNL_COPY_INSTANTIATE(T, S)

VNL_COPY_INSTANTIATE_twoway(float, double);
VNL_COPY_INSTANTIATE_twoway(double, long double);
VNL_COPY_INSTANTIATE_twoway(std::complex<float>, std::complex<double>);
VNL_COPY_INSTANTIATE_twoway(std::complex<double>, std::complex<long double>);
