// This is vxl/VNL/algo/symmetric_eigensystem.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// vnl_symmetric_eigensystem
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 29 Aug 96
//
//-----------------------------------------------------------------------------
#include <VNL/VC6WarningsFix.h>
#include <assert.h>
#include <cmath> // for sqrt(double)
#include <iostream>
#include <VNL/copy.h>
#include <VNL/NetLib/netlib.h> // rs_()

#include "symmetriceigensystem.h"

bool VNL::SymmetricEigensystemCompute(VNL::Matrix<float> const & A,
                                       VNL::Matrix<float>       & V,
                                       VNL::Vector<float>       & D)
{
  VNL::Matrix<double> Ad(A.Rows(), A.Columns());
  VNL::Matrix<double> Vd(V.Rows(), V.Columns());
  VNL::Vector<double> Dd(D.size());
  VNL::Copy(A, Ad);
  bool f = VNL::SymmetricEigensystemCompute(Ad, Vd, Dd);
  VNL::Copy(Vd, V);
  VNL::Copy(Dd, D);
  return f;
}

bool VNL::SymmetricEigensystemCompute(VNL::Matrix<double> const & A,
                                       VNL::Matrix<double>       & V,
                                       VNL::Vector<double>       & D)
{
  A.assert_finite();

  // The fortran code does not like it if V or D are
  // undersized. I expect they probably should not be
  // oversized either. - IMS
  assert(V.Rows() == A.Rows());
  assert(V.Columns() == A.Rows());
  assert(D.size() == A.Rows());

  int n = A.Rows();
  VNL::Vector<double> work1(n);
  VNL::Vector<double> work2(n);
  VNL::Vector<double> Vvec(n*n);

  int want_eigenvectors = 1;
  int ierr = 0;

  // No need to transpose A, cos it's symmetric...
  VNL::Matrix<double> B = A; // since A is read-only and rs_ might change its third argument...
  rs_(&n, &n, B.DataBlock(), &D[0], &want_eigenvectors, &Vvec[0], &work1[0], &work2[0], &ierr);

  if (ierr) {
    std::cerr << "symmetric_eigensystem: ierr = " << ierr << std::endl;
    return false;
  }

  // Transpose-copy into V
  double *vptr = &Vvec[0];
  for (int c = 0; c < n; ++c)
    for (int r = 0; r < n; ++r)
      V(r,c) = *vptr++;

  return true;
}

//----------------------------------------------------------------------

// - @{ Solve real symmetric eigensystem $A x = \lambda x$ @}
template <class T>
VNL::SymmetricEigensystem<T>::SymmetricEigensystem(VNL::Matrix<T> const& A)
  : n_(A.Rows()), V(n_, n_), D(n_)
{
  VNL::Vector<T> Dvec(n_);

  VNL::SymmetricEigensystemCompute(A, V, Dvec);

  // Copy Dvec into diagonal of D
  for (int i = 0; i < n_; ++i)
    D(i,i) = Dvec[i];
}

template <class T>
VNL::Vector<T> VNL::SymmetricEigensystem<T>::GetEigenvector(int i) const
{
  return VNL::Vector<T>(V.Extract(n_,1,0,i).DataBlock(), n_);
}

template <class T>
T VNL::SymmetricEigensystem<T>::GetEigenvalue(int i) const
{
  return D(i, i);
}

template <class T>
VNL::Vector<T> VNL::SymmetricEigensystem<T>::Solve(VNL::Vector<T> const& b)
{
  //VNL::Vector<T> ret(b.length());
  //FastOps::AtB(V, b, &ret);
  VNL::Vector<T> ret(b*V); // same as V.tranpose()*b

  VNL::Vector<T> tmp(b.size());
  D.Solve(ret, &tmp);

  return V * tmp;
}

template <class T>
VNL::Matrix<T> VNL::SymmetricEigensystem<T>::PsuedoInverse() const
{
  unsigned n = D.Rows();
  VNL::DiagMatrix<T> invD(n);
  for (unsigned i=0; i<n; ++i)
    if (D(i, i) == 0) {
      std::cerr << __FILE__ ": pinverse(): eigenvalue " << i << " is zero.\n";
      invD(i, i) = 0;
    }
    else
      invD(i, i) = T(1.0/D(i, i));
  return V * invD * V.Transpose();
}

template <class T>
VNL::Matrix<T> VNL::SymmetricEigensystem<T>::SquareRoot() const
{
	using namespace std;
  unsigned n = D.Rows();
  VNL::DiagMatrix<T> sqrtD(n);
  for (unsigned i=0; i<n; ++i)
    if (D(i, i) < 0) {
      std::cerr << __FILE__ ": square_root(): eigenvalue " << i << " is negative (" << D(i, i) << ").\n";
      sqrtD(i, i) = T(sqrt((typename VNL::NumericTraits<T>::real_t)(-D(i, i))));
                    // gives square root of the absolute value of T.
    }
    else
      sqrtD(i, i) = T(sqrt((typename VNL::NumericTraits<T>::real_t)(D(i, i))));
  return V * sqrtD * V.Transpose();
}

template <class T>
VNL::Matrix<T> VNL::SymmetricEigensystem<T>::InverseSquareRoot() const
{
   using namespace std;
  unsigned n = D.Rows();
  VNL::DiagMatrix<T> inv_sqrtD(n);
  for (unsigned i=0; i<n; ++i)
    if (D(i, i) <= 0) {
      std::cerr << __FILE__ ": square_root(): eigenvalue " << i << " is non-positive (" << D(i, i) << ").\n";
      inv_sqrtD(i, i) = T(sqrt(-1.0/(typename VNL::NumericTraits<T>::real_t)(D(i, i)))); // ??
    }
    else
      inv_sqrtD(i, i) = T(sqrt(1.0/(typename VNL::NumericTraits<T>::real_t)(D(i, i))));
  return V * inv_sqrtD * V.Transpose();
}

//--------------------------------------------------------------------------------

namespace VNL {
  template class SymmetricEigensystem<float>;
  template class SymmetricEigensystem<double>;
};
