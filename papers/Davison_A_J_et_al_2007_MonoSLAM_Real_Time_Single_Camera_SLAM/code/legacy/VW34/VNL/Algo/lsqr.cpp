// This is vxl/VNL/algo/lsqr.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// vnl_lsqr
// Author: David Capel
// Created: July 2000
//
//-----------------------------------------------------------------------------

#include "lsqr.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include <VNL/vectorref.h>

#include <VNL/NetLib/netlib.h> // lsqr_()

using namespace VNL;

namespace VNL {
class LSqrActivate {
public:
  static LSqr* current;

  LSqrActivate(LSqr* minimizer) {
    if (current) {
      std::cerr << "lsqr: ERROR: Nested minimizations not supported.\n";
      abort();
      // This is a copy of what goes on in LevenbergMarquardt, so if awf decides to
      // fix that one, then maybe he could do the same here...
    }
    current = minimizer;
  }
  ~LSqrActivate() {
    current = 0;
  }
};
}; // End namespace VNL


VNL::LSqr *VNL::LSqrActivate::current= 0;

VNL::LSqr::~LSqr()
{
}

// Requires number_of_residuals() of workspace in rw.
void VNL::LSqr::aprod_(int* mode, int* m, int* n, double* x, double* y, int* /*leniw*/, int* /*lenrw*/, int* /*iw*/, double* rw )
{
  VNL::LSqr* active = VNL::LSqrActivate::current;

  //  If MODE = 1, compute  y = y + A*x.
  //  If MODE = 2, compute  x = x + A(transpose)*y.

  VNL::VectorRef<double> x_ref(*n,x);
  VNL::VectorRef<double> y_ref(*m,y);

  if (*mode == 1) {
    VNL::VectorRef<double> tmp(*m,rw);
    active->ls_->Multiply(x_ref, tmp);
    y_ref += tmp;
  }
  else {
    VNL::VectorRef<double> tmp(*n,rw);
    active->ls_->TransposeMultiply(y_ref, tmp);
    x_ref += tmp;
  }
}

int VNL::LSqr::Minimize(VNL::Vector<double>& result)
{
  int m = ls_->GetNumberOfResiduals();
  int n = ls_->GetNumberOfUnknowns();
  double damp = 0;
  int leniw = 1;
  int* iw = 0;
  int lenrw = m;
#ifdef __GNUC__
  double rw[m];
  double v[n];
  double w[n];
  double se[n];
#else
  std::vector<double> rw(m);
  std::vector<double> v(n);
  std::vector<double> w(n);
  std::vector<double> se(n);
#endif
  double atol = 0;
  double btol = 0;
  double conlim = 0;
  int nout = -1;
  double anorm, acond, rnorm, arnorm, xnorm;

  VNL::Vector<double> rhs(m);
  ls_->GetRHS(rhs);

  VNL::LSqrActivate activator(this); // This variable is not used, but the constructor must be called.

  lsqr_(&m, &n, aprod_, &damp, &leniw, &lenrw, iw, &rw[0],
        rhs.DataBlock(), &v[0], &w[0], result.DataBlock(), &se[0],
        &atol, &btol, &conlim, &max_iter_, &nout, &return_code_,
        &num_iter_, &anorm, &acond, &rnorm, &arnorm, &xnorm);

  resid_norm_estimate_ = rnorm;
  result_norm_estimate_ = xnorm;
  A_condition_estimate_ = acond;

#if 0
  std::cerr << "A Fro norm estimate      = " << anorm << std::endl
           << "A condition estimate     = " << acond << std::endl
           << "Residual norm estimate   = " << rnorm << std::endl
           << "A'(Ax - b) norm estimate = " << arnorm << std::endl
           << "x norm estimate          = " << xnorm << std::endl;
#endif

  return 0; // return value not used
}

void VNL::LSqr::DiagnoseOutcome(std::ostream& os) const
{
  TranslateReturnCode(os, return_code_);
  os << __FILE__ " : residual norm estimate = " << resid_norm_estimate_ << std::endl
     << __FILE__ " : result norm estimate   = " << result_norm_estimate_ << std::endl
     << __FILE__ " : condition no. estimate = " << A_condition_estimate_ << std::endl
     << __FILE__ " : iterations             = " << num_iter_ << std::endl;
}

void VNL::LSqr::TranslateReturnCode(std::ostream& os, int rc)
{
  char* lsqr_reasons[] = {
   "x = 0  is the exact solution. No iterations were performed.",
   "The equations A*x = b are probably compatible.  "
       "Norm(A*x - b) is sufficiently small, given the "
       "values of ATOL and BTOL.",
   "The system A*x = b is probably not compatible.  "
       "A least-squares solution has been obtained that is "
       "sufficiently accurate, given the value of ATOL.",
   "An estimate of cond(Abar) has exceeded CONLIM.  "
       "The system A*x = b appears to be ill-conditioned.  "
       "Otherwise, there could be an error in subroutine APROD.",
   "The equations A*x = b are probably compatible.  "
       "Norm(A*x - b) is as small as seems reasonable on this machine.",
   "The system A*x = b is probably not compatible.  A least-squares "
       "solution has been obtained that is as accurate as seems "
       "reasonable on this machine.",
   "Cond(Abar) seems to be so large that there is no point in doing further "
       "iterations, given the precision of this machine. "
       "There could be an error in subroutine APROD.",
   "The iteration limit ITNLIM was reached."
  };

  if
    (rc < 0 || rc > 7) os << __FILE__ " : Illegal return code : " << rc << std::endl;
  else
    os << __FILE__ " : " << lsqr_reasons[rc] << std::endl;
}
