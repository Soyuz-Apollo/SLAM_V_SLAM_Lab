// This is vxl/VNL/algo/generalized_eigensystem.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// vnl_generalized_eigensystem
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 29 Aug 96
//

#include "generalizedeigensystem.h"

#include <iostream>

#include <VNL/fortrancopy.h>
#include <VNL/matlabprint.h>
#include <VNL/NetLib/netlib.h> // rsg_()

// local includes
#include "symmetriceigensystem.h"
#include "svd.h"

VNL::GeneralizedEigensystem::GeneralizedEigensystem(const VNL::Matrix<double>& A,
						    const VNL::Matrix<double>& B)
  :
  n(A.Rows()), V(n,n), D(n)
{
  // Copy source matrices into fortran storage
  vnl_fortran_copy<double> a(A);
  vnl_fortran_copy<double> b(B);

  // Make workspace and storage for V transpose
  VNL::Vector<double> work1(n);
  VNL::Vector<double> work2(n);
  VNL::Vector<double> V1(n*n);

  int want_eigenvectors = 1;
  int ierr = -1;

  // Call EISPACK rsg.
  rsg_ (&n, &n, a, b, D.DataBlock(),
        &want_eigenvectors,
        V1.begin(),
        work1.begin(),
        work2.begin(), &ierr);

  // If b was not pos-def, retry with projection onto nullspace
  if (ierr == 7*n+1) {
    const double THRESH = 1e-8;
    VNL::SymmetricEigensystem<double> eig(B);
    if (eig.D(0,0) < -THRESH) {
      std::cerr << "**** vnl_generalized_eigensystem: ERROR\n"
               << "Matrix B is not nonneg-definite\n";
      VNL::MatlabPrint(std::cerr, B, "B");
      std::cerr << "**** eigenvalues(B) = " << eig.D << std::endl;
      return;
    }
    // hmmmmm - all this crap below is worse than whatever the default is...
    return;
#if 0 // so don't compile it then...
    int rank_deficiency = 0;
    while (eig.D(rank_deficiency,rank_deficiency) < THRESH)
      ++rank_deficiency;
    int rank = B.columns() - rank_deficiency;

    std::cerr << "generalized_eigensystem: B rank def by " << rank_deficiency << std::endl;
    // M is basis for non-nullspace of B
    VNL::Matrix<double> M = eig.V.GetNColumns(rank_deficiency, rank);
    VNL::Matrix<double> N = eig.V.GetNColumns(0, rank_deficiency);

    VNL::SVD<double> svd(M.Transpose()*A*N);

    VNL::GeneralizedEigensystem reduced(M.Transpose() * A * M,
                                        M.Transpose() * B * M);

    std::cerr << "AN: " << reduced.D << std::endl;

    VNL::Matrix<double> V05 = M * reduced.V.transpose();
    VNL::SVD<double> sv6(V05.Transpose());
    V.Update(V05, 0, 0);
    V.Update(sv6.Nullspace(), 0, rank - 1);
    for (int i = 0; i < rank; ++i)
      D(i,i) = reduced.D(i,i);
    for (unsigned i = rank; i < B.columns(); ++i)
      D(i,i) = 0;
    std::cerr << "AN: " << D << std::endl;

    return;
#endif
  }

  // vnl_transpose-copy V1 to V
  {
    double *vptr = &V1[0];
    for (int c = 0; c < n; ++c)
      for (int r = 0; r < n; ++r)
        V(r,c) = *vptr++;
  }

  // Diagnose errors
  if (ierr) {
    if (ierr == 10*n)
      std::cerr << "generalized_eigensystem: N is greater than NM.  Bug in interface to rsg.f\n";
    else {
      std::cerr << "generalized_eigensystem: The "
               << ierr << "-th eigenvalue has not been determined after 30 iterations.\n"
               << "The eigenvalues should be correct for indices 1.." << ierr-1
               << ", but no eigenvectors are computed.\n"
               << "A = " << A
               << "\nsingular values(A) = " << VNL::SVD<double>(A).W() << "\n"
               << "B = " << B
               << "\nsingular values(B) = " << VNL::SVD<double>(B).W() << "\n";
    }
  }
}
