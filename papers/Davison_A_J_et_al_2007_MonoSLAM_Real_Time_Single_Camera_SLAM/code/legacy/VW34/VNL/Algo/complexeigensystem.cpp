/*
  fsm@robots.ox.ac.uk
*/
#include <VNL/VC6WarningsFix.h>
#include "complexeigensystem.h"

#include <assert.h>
#include <cstdlib>
#include <iostream>

#include <VNL/matlabprint.h>
#include <VNL/complexify.h>
#include <VNL/NetLib/netlib.h> // zgeev_()

void VNL::ComplexEigensystem::_Compute(VNL::Matrix<std::complex<double> > const & A,
                                      bool right,
                                      bool left)
{
  A.assert_size(N, N);

  A.assert_finite();
  assert(! A.IsZero());

  if (right)
    R.Resize(N, N);
  if (left)
    L.Resize(N, N);

  //
  // Remember that fortran matrices and C matrices are transposed
  // relative to each other. Moreover, the documentation for zgeev
  // says that left eigenvectors u satisfy u^h A = lambda u^h,
  // where ^h denotes adjoint (conjugate transpose).
  // So we pass our left eigenvector storage as their right
  // eigenvector storage and vice versa.
  // But then we also have to conjugate our R after calling the routine.
  //
  VNL::Matrix<std::complex<double> > tmp(A);

  int work_space=10*N;
  VNL::Vector<std::complex<double> > work(work_space);

  int rwork_space=2*N;
  VNL::Vector<double> rwork(rwork_space);

  int info;
  int tmpN = N;
  zgeev_(right ? "V" : "N",          // jobvl
         left  ? "V" : "N",          // jobvr
         &tmpN,                      // n
         tmp.DataBlock(),           // a
         &tmpN,                      // lda
         W.DataBlock(),             // w
         right ? R.DataBlock() : 0, // vl
         &tmpN,                      // ldvl
         left  ? L.DataBlock() : 0, // vr
         &tmpN,                      // ldvr
         work.DataBlock(),          // work
         &work_space,                // lwork
         rwork.DataBlock(),         // rwork
         &info                       // info
         );
  assert(tmpN == N);

  if (right) {
    // conjugate all elements of R :
    for (int i=0;i<N;i++)
      for (int j=0;j<N;j++)
        R(i,j) = std::conj( R(i,j) );
  }

  if (info == 0) {
    // success
  }
  else if (info < 0) {
    std::cerr << __FILE__ ": info = " << info << std::endl;
    std::cerr << __FILE__ ": " << (-info) << "th argument has illegal value" << std::endl;
    assert(false);
  }
  else /* if (info > 0) */ {
    std::cerr << __FILE__ ": info = " << info << std::endl;
    std::cerr << __FILE__ ": QR algorithm failed to compute all eigenvalues." << std::endl;
    VNL::MatlabPrint(std::cerr, A, "A", VNL::matlab_print_format_long);
    assert(false);
  }
}

//--------------------------------------------------------------------------------

//
VNL::ComplexEigensystem::ComplexEigensystem(VNL::Matrix<std::complex<double> > const &A,
                                                 bool right,
                                                 bool left)
  : N(A.Rows())
  // L and R are intentionally not initialized.
  , W(N)
{
  _Compute(A, right, left);
}

//
VNL::ComplexEigensystem::ComplexEigensystem(VNL::Matrix<double> const &A_real,
                                                 VNL::Matrix<double> const &A_imag,
                                                 bool right,
                                                 bool left)
  : N(A_real.Rows())
  // L and R are intentionally not initialized.
  , W(N)
{
  A_real.assert_size(N,N);
  A_imag.assert_size(N,N);

  VNL::Matrix<std::complex<double> > A(N,N);
  VNL::Complexify(A_real.begin(), A_imag.begin(), A.begin(), A.size());

  _Compute(A, right, left);
}
