// This is vxl/VNL/algo/generalized_schur.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author fsm@robots.ox.ac.uk
*/

#include "generalizedschur.h"

//#include <vector>
#include <iostream>
#include <assert.h>

#include <VNL/vector.h>

#include <VNL/NetLib/netlib.h> // dgges_()

namespace VNL {

template <>
bool GeneralizedSchur/*<double>*/(VNL::Matrix<double> *A,
				  VNL::Matrix<double> *B,
				  VNL::Vector<double> *alphar,
				  VNL::Vector<double> *alphai,
				  VNL::Vector<double> *beta,
				  VNL::Matrix<double> *L,
				  VNL::Matrix<double> *R)
{
  assert(A->Columns() == A->Columns());
  assert(A->Columns() == B->Rows());
  assert(A->Columns() == B->Columns());

  int n = A->Rows();
  assert(alphar!=0); alphar->Resize(n);    alphar->Fill(0);
  assert(alphai!=0); alphai->Resize(n);    alphai->Fill(0);
  assert(beta!=0);   beta  ->Resize(n);    beta  ->Fill(0);
  assert(L!=0);      L     ->Resize(n, n); L     ->Fill(0);
  assert(R!=0);      R     ->Resize(n, n); R     ->Fill(0);

  int sdim = 0;
  int lwork = 1000 + (8*n + 16);
  double *work = new double[lwork]; //std::vector<double> work(lwork);
  int info = 0;
  A->InplaceTranspose();
  B->InplaceTranspose();
  dgges_ ("V", "V",
          "N",
          0,
          &n,
          A->DataBlock(), &n,
          B->DataBlock(), &n,
          &sdim,
          alphar->DataBlock(),
          alphai->DataBlock(),
          beta->DataBlock(),
          L->DataBlock(), &n,
          R->DataBlock(), &n,
          &work[0], &lwork,
          0,
          &info);
  A->InplaceTranspose();
  B->InplaceTranspose();
  L->InplaceTranspose();
  R->InplaceTranspose();
  delete [] work;

  if (info != 0) {
    std::clog << __FILE__ ": info = " << info << std::endl;
    return false;
  }
  else
    return true;
}


}; // End namespace VNL
