// This is vxl/VNL/algo/real_eigensystem.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
*
* \author Andrew W. Fitzgibbon, Oxford RRG
* \date   23 Jan 97
*/

//-----------------------------------------------------------------------------
#include <VNL/VC6WarningsFix.h>
#include "realeigensystem.h"
#include <assert.h>
#include <iostream>
#include <VNL/fortrancopy.h>
#include <VNL/NetLib/netlib.h> // rg_()

/** Extract eigensystem of unsymmetric matrix M, using the EISPACK routine rg.
*  Should probably switch to using LAPACK's dgeev to avoid transposing.
*/
VNL::RealEigensystem::RealEigensystem(VNL::Matrix<double> const & M):
  Vreal(M.Rows(), M.Columns()),
  V(M.Rows(), M.Columns()),
  D(M.Rows())
{
  int n = M.Rows();
  assert(n == (int)(M.Columns()));

  vnl_fortran_copy<double> mf(M);

  VNL::Vector<double> wr(n);
  VNL::Vector<double> wi(n);
  VNL::Vector<int> iv1(n);
  VNL::Vector<double> fv1(n);
  VNL::Matrix<double> devout(n, n);

  int ierr = 0;
  int matz = 1;
  rg_(&n, &n, mf, wr.DataBlock(), wi.DataBlock(), &matz, devout.DataBlock(), iv1.DataBlock(), fv1.DataBlock(), &ierr);

  if (ierr != 0) {
    std::cerr << " *** vnl_real_eigensystem: Failed on " << ierr << "th eigenvalue\n";
    std::cerr << M << std::endl;
  }

  // Copy out eigenvalues and eigenvectors
  for (int c = 0; c < n; ++c) {
    D(c,c) = std::complex<double>(wr[c], wi[c]);
    if (wi[c] != 0) {
      // Complex - copy conjugates and inc c.
      D(c+1, c+1) = std::complex<double>(wr[c], -wi[c]);
      for (int r = 0; r < n; ++r) {
        V(r, c) = std::complex<double>(devout(c,r), devout(c+1,r));
        V(r, c+1) = std::complex<double>(devout(c,r), -devout(c+1,r));
      }

      ++c;
    } else
      for (int r = 0; r < n; ++r) {
        V(r, c) = std::complex<double>(devout(c,r), 0);
        Vreal(r,c) = devout(c,r);
      }
  }
}
