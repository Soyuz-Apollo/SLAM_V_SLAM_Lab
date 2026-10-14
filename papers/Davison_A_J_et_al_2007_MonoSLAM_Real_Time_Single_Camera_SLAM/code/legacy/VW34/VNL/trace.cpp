// This is vxl/VNL/trace.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author fsm@robots.ox.ac.uk
*/

#include "trace.h"

#include <VNL/matrix.h>

template <class T>
T VNL::Trace(VNL::Matrix<T> const &M) {
  T sum(0);
  unsigned N = (M.Rows() < M.Cols() ? M.Rows() : M.Cols());
  for (unsigned i=0; i<N; ++i)
    sum += M(i, i);
  return sum;
}

namespace VNL {

#define VNL_TRACE_INSTANTIATE(T) \
template T Trace(VNL::Matrix<T > const &)

VNL_TRACE_INSTANTIATE(float);
VNL_TRACE_INSTANTIATE(double);

};
