// This is vxl/VNL/sparse_matrix_linear_system.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "sparsematrixlinearsystem.h"
#include <assert.h>
#include <VNL/copy.h>

namespace VNL {

template <>
void SparseMatrixLinearSystem<double>::GetRHS(VNL::Vector<double>& b) const
{
  b = b_;
}
template <>
void SparseMatrixLinearSystem<double>::TransposeMultiply(VNL::Vector<double> const& b, VNL::Vector<double> & x) const
{
  A_.pre_mult(b,x);
}

template <>
void SparseMatrixLinearSystem<float>::GetRHS(VNL::Vector<double>& b) const
{
   VNL::Copy(b_, b);
}

template <>
void SparseMatrixLinearSystem<float>::TransposeMultiply(VNL::Vector<double> const& b, VNL::Vector<double> & x) const
{
  static VNL::Vector<float> x_float;
  static VNL::Vector<float> b_float;

  if (x_float.size() != x.size()) x_float = VNL::Vector<float> (x.size());
  if (b_float.size() != b.size()) b_float = VNL::Vector<float> (b.size());

  VNL::Copy(b, b_float);
  A_.pre_mult(b_float,x_float);
  VNL::Copy(x_float, x);
}

template <>
void SparseMatrixLinearSystem<double>::Multiply(VNL::Vector<double> const& x, VNL::Vector<double> & b) const
{
  A_.mult(x,b);
}


template <>
void SparseMatrixLinearSystem<float>::Multiply(VNL::Vector<double> const& x, VNL::Vector<double> & b) const
{
  static VNL::Vector<float> x_float;
  static VNL::Vector<float> b_float;

  if (x_float.size() != x.size()) x_float = VNL::Vector<float> (x.size());
  if (b_float.size() != b.size()) b_float = VNL::Vector<float> (b.size());

  VNL::Copy(x, x_float);
  A_.mult(x_float,b_float);
  VNL::Copy(b_float, b);
}

}; //namespace VNL

template<class T>
void VNL::SparseMatrixLinearSystem<T>::ApplyPreconditioner(VNL::Vector<double> const& x, VNL::Vector<double> & px) const
{
  assert(x.size() == px.size());

  if (jacobi_precond_.size() == 0) {
    VNL::Vector<T> tmp(GetNumberOfUnknowns());
    A_.diag_AtA(tmp);
    const_cast<VNL::Vector<double> &>(jacobi_precond_) = VNL::Vector<double> (tmp.size());
    for (unsigned int i=0; i < tmp.size(); ++i)
      const_cast<VNL::Vector<double> &>(jacobi_precond_)[i] = 1.0 / double(tmp[i]);
  }

  px = VNL::DotProduct(x,jacobi_precond_);
}

namespace VNL {
  template class SparseMatrixLinearSystem<double>;
  template class SparseMatrixLinearSystem<float>;
};
