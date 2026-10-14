// This is vxl/VNL/algo/levenberg_marquardt.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// vnl_levenberg_marquardt
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 31 Aug 96
//
//-----------------------------------------------------------------------------

#include "levenbergmarquardt.h"

#include <assert.h>
#include <cstdlib>
#include <iostream>

#include <VNL/vector.h>
#include <VNL/matrix.h>
#include <VNL/vectorref.h>
#include <VNL/matrixref.h>
#include <VNL/leastsquaresfunction.h>
#include <VNL/NetLib/netlib.h>

// see header
VNL::Vector<double> VNL::LevenbergMarquardtMinimize(VNL::LeastSquaresFunction& f, 
						    VNL::Vector<double> const& initial_estimate)
{
  VNL::Vector<double> x = initial_estimate;
  VNL::LevenbergMarquardt lm(f);
  lm.Minimize(x);
  return x;
}

// ctor
void VNL::LevenbergMarquardt::_Init(VNL::LeastSquaresFunction* f)
{
  f_ = f;

  // If changing these defaults, check the help comments in vnl_levenberg_marquardt.h,
  // and MAKE SURE they're consistent.
  xtol = 1e-8;           // Termination tolerance on X (solution vector)
  maxfev = 400 * f->GetNumberOfUnknowns(); // Termination maximum number of iterations.
  ftol = xtol * 0.01;    // Termination tolerance on F (sum of squared residuals)
  gtol = 1e-5;           // Termination tolerance on Grad(F)' * F = 0
  epsfcn = xtol * 0.001; // Step length for FD Jacobian

  int m = f_->GetNumberOfResiduals();        // I     Number of residuals, must be > #unknowns
  int n = f_->GetNumberOfUnknowns();         // I     Number of unknowns

  set_covariance_ = false;
  fdjac_ = new VNL::Matrix<double>(n,m);
  ipvt_ = new VNL::Vector<int>(n);
  covariance_ = new VNL::Matrix<double>(n,n);
}

VNL::LevenbergMarquardt::~LevenbergMarquardt()
{
  delete covariance_;
  delete fdjac_;
  delete ipvt_;
}

namespace VNL {
class LevenbergMarquardtActivate {
public:
  static LevenbergMarquardt* current;

  LevenbergMarquardtActivate(LevenbergMarquardt* minimizer) {
    if (current) {
      std::cerr << "levenberg_marquardt: ERROR: Nested minimizations not supported.\n";
      abort();
      // Ask awf to fix this if you need to run one minimization inside another.
      // Or just make "current" a stack.
      // Actually....  I think it might be a lot harder than that, the statics in the fortran.
    }
    current = minimizer;
  }
  
  ~LevenbergMarquardtActivate() {
    current = 0;
  }
};
}; // End namespace VNL

VNL::LevenbergMarquardt* VNL::LevenbergMarquardtActivate::current = 0;

//--------------------------------------------------------------------------------

#ifdef VCL_SUNPRO_CC
extern "C"
#endif
void VNL::LevenbergMarquardt::lmdif_lsqfun(int* n,          // I    Number of residuals
                                           int* p,          // I    Number of unknowns
                                           double* x,       // I    Solution vector, size n
                                           double* fx,      // O    Residual vector f(x)
                                           int* iflag)      // IO   0 ==> print, -1 ==> terminate
{
  VNL::LevenbergMarquardt* active = VNL::LevenbergMarquardtActivate::current;
  VNL::LeastSquaresFunction* f = active->f_;
  assert(*p == f->GetNumberOfUnknowns());
  assert(*n == f->GetNumberOfResiduals());
  VNL::VectorRef<double> ref_x(*p, const_cast<double*>(x));
  VNL::VectorRef<double> ref_fx(*n, fx);

  if (*iflag == 0) {
    if (active->trace)
      std::cerr << "lmdif: iter " << active->num_iterations_ << " err ["
               << x[0] << ", " << x[1] << ", " << x[2] << ", " << x[3] << ", "
               << x[4] << ", ... ] = " << ref_fx.Magnitude() << '\n';

    f->Trace(active->num_iterations_, ref_x, ref_fx);
    ++(active->num_iterations_);
  } else {
    f->F(ref_x, ref_fx);
  }

  if (active->start_error_ == 0)
    active->start_error_ = ref_fx.RMS();

  if (f->failure) {
    f->ClearFailure();
    *iflag = -1; // fsm
  }
}


// This function shouldn't be inlined, because (1) modification of the
// body will not cause the timestamp on the header to change, and so
// others will not be forced to recompile, and (2) the cost of making
// one more function call is far, far less than the cost of actually
// performing the minimisation, so the inline doesn't gain you
// anything.
//
bool VNL::LevenbergMarquardt::Minimize(VNL::Vector<double>& x)
{
  if ( f_->HasGradient() )
    return MinimizeUsingGradient(x);
  else
    return MinimizeWithoutGradient(x);
}


//
bool VNL::LevenbergMarquardt::MinimizeWithoutGradient(VNL::Vector<double>& x)
{
  //fsm
  if (f_->HasGradient()) {
    std::cerr << __FILE__ " : WARNING. calling minimize_without_gradient(), but f_ has gradient.\n";
  }

  // e04fcf
  int m = f_->GetNumberOfResiduals();        // I     Number of residuals, must be > #unknowns
  int n = f_->GetNumberOfUnknowns();         // I     Number of unknowns

  if (m < n) {
    std::cerr << "levenberg_marquardt: Number of unknowns("<<n<<") greater than number of data ("<<m<<")\n";
    failure_code_ = ERROR_DODGY_INPUT;
    return false;
  }

  if (int(x.size()) != n) {
    std::cerr << "levenberg_marquardt: Input vector length ("<<x.size()<<") not equal to num unknowns ("<<n<<")\n";
    failure_code_ = ERROR_DODGY_INPUT;
    return false;
  }

  VNL::Vector<double> fx(m);    // W m   Storage for residual vector
  VNL::Vector<double> diag(n);  // I     Multiplicative scale factors for variables
  int user_provided_scale_factors = 1;  // 1 is no, 2 is yes
  double factor = 100;
  int nprint = 1;

  VNL::Vector<double> qtf(n);
  VNL::Vector<double> wa1(n);
  VNL::Vector<double> wa2(n);
  VNL::Vector<double> wa3(n);
  VNL::Vector<double> wa4(m);

  //std::cerr << "STATUS: " << failure_code_ << std::endl;
  VNL::LevenbergMarquardtActivate activator(this);

  double errors[2] = {0,0};
  num_iterations_ = 0;
  set_covariance_ = false;
  int info;
  start_error_ = 0; // Set to 0 so first call to lmdif_lsqfun will know to set it.
  lmdif_(lmdif_lsqfun, &m, &n,
         x.DataBlock(),
         fx.DataBlock(),
         &ftol, &xtol, &gtol, &maxfev, &epsfcn,
         &diag[0],
         &user_provided_scale_factors, &factor, &nprint,
         &info, &num_evaluations_,
         fdjac_->DataBlock(), &m, ipvt_->DataBlock(),
         &qtf[0],
         &wa1[0], &wa2[0], &wa3[0], &wa4[0],
         errors);
  failure_code_ = (ReturnCodes) info;

  // One more call to compute final error.
  lmdif_lsqfun(&m,              // I    Number of residuals
               &n,              // I    Number of unknowns
               x.DataBlock(),  // I    Solution vector, size n
               fx.DataBlock(), // O    Residual vector f(x)
               &info);
  end_error_ = fx.RMS();

#ifdef _SGI_CC_6_
  // Something fundamentally odd about the switch below on SGI native... FIXME
  std::cerr << "levenberg_marquardt: termination code = " << failure_code_ << std::endl;
  // diagnose_outcome(std::cerr);
  return 1;
#endif

  // Translate status code
  switch ((int)failure_code_) {
  case 1: // ftol
  case 2: // xtol
  case 3: // both
  case 4: // gtol
    return true;
  default:
    DiagnoseOutcome();
    return false;
  }
}

//--------------------------------------------------------------------------------

#ifdef VCL_SUNPRO_CC
extern "C"
#endif
void VNL::LevenbergMarquardt::lmder_lsqfun(int* n,          // I    Number of residuals
                                           int* p,          // I    Number of unknowns
                                           double* x,       // I    Solution vector, size n
                                           double* fx,      // O    Residual vector f(x)
                                           double* fJ,      // O    m * n Jacobian f(x)
                                           int*,
                                           int* iflag)      // I    1 -> calc fx, 2 -> calc fjac
{
  VNL::LevenbergMarquardt* active = VNL::LevenbergMarquardtActivate::current;
  VNL::LeastSquaresFunction* f = active->f_;
  assert(*p == f->GetNumberOfUnknowns());
  assert(*n == f->GetNumberOfResiduals());
  VNL::VectorRef<double> ref_x(*p, (double*)x); // const violation!
  VNL::VectorRef<double> ref_fx(*n, fx);
  VNL::MatrixRef<double> ref_fJ(*n, *p, fJ);

  if (*iflag == 0) {
    if (active->trace)
      std::cerr << "lmder: iter " << active->num_iterations_ << " err ["
               << x[0] << ", " << x[1] << ", " << x[2] << ", " << x[3] << ", "
               << x[4] << ", ... ] = " << ref_fx.Magnitude() << '\n';
    f->Trace(active->num_iterations_, ref_x, ref_fx);
  }
  else if (*iflag == 1) {
    f->F(ref_x, ref_fx);
    if (active->start_error_ == 0)
      active->start_error_ = ref_fx.RMS();
    ++(active->num_iterations_);
  }
  else if (*iflag == 2) {
    f->GradF(ref_x, ref_fJ);
    ref_fJ.InplaceTranspose();
  }

  if (f->failure) {
    f->ClearFailure();
    *iflag = -1; // fsm
  }
}


//
bool VNL::LevenbergMarquardt::MinimizeUsingGradient(VNL::Vector<double>& x)
{
  //fsm
  if (! f_->HasGradient()) {
    std::cerr << __FILE__ ": called method minimize_using_gradient(), but f_ has no gradient.\n";
    return false;
  }

  int m = f_->GetNumberOfResiduals();        // I     Number of residuals, must be > #unknowns
  int n = f_->GetNumberOfUnknowns();         // I     Number of unknowns

  if (m < n) {
    std::cerr << __FILE__ ": Number of unknowns("<<n<<") greater than number of data ("<<m<<")\n";
    failure_code_ = ERROR_DODGY_INPUT;
    return false;
  }

  VNL::Vector<double> fx(m);    // W m   Storage for residual vector
  VNL::Vector<double> wa1(5*n + m);

  LevenbergMarquardtActivate activator(this);

  num_iterations_ = 0;
  set_covariance_ = false;
  int info;
  int size = wa1.size();
  start_error_ = 0; // Set to 0 so first call to lmder_lsqfun will know to set it.
  lmder1_(lmder_lsqfun, &m, &n,
          x.DataBlock(),
          fx.DataBlock(),
          fdjac_->DataBlock(), &m,
          &ftol,
          &info,
          ipvt_->DataBlock(),
          wa1.DataBlock(),
          &size);
  num_evaluations_ = num_iterations_; // for lmder, these are the same.
  if (info<0)
    info = ERROR_FAILURE;
  failure_code_ = (ReturnCodes) info;
  end_error_ = fx.RMS();

  // Translate status code
  switch (failure_code_) {
  case 1: // ftol
  case 2: // xtol
  case 3: // both
  case 4: // gtol
    return true;
  default:
    DiagnoseOutcome();
    return false;
  }
}

//--------------------------------------------------------------------------------

void VNL::LevenbergMarquardt::DiagnoseOutcome() const
{
  DiagnoseOutcome(std::cerr);
}

// fsm: should this function be a method on vnl_nonlinear_minimizer?
// if not, the return codes should be moved into LM.
void VNL::LevenbergMarquardt::DiagnoseOutcome(std::ostream& s) const
{
#define whoami "levenberg_marquardt"
  //if (!verbose_) return;
  switch (failure_code_) {
    //  case -1:
    // have already warned.
    //    return;
  case ERROR_FAILURE:
    s << (whoami ": OIOIOI -- failure in leastsquares function\n");
    break;
  case ERROR_DODGY_INPUT:
    s << (whoami ": OIOIOI -- lmdif dodgy input\n");
    break;
  case CONVERGED_FTOL: // ftol
    s << (whoami ": converged to ftol\n");
    break;
  case CONVERGED_XTOL: // xtol
    s << (whoami ": converged to xtol\n");
    break;
  case CONVERGED_XFTOL: // both
    s << (whoami ": converged nicely\n");
    break;
  case CONVERGED_GTOL:
    s << (whoami ": converged via gtol\n");
    break;
  case FAILED_TOO_MANY_ITERATIONS:
    s << (whoami ": too many iterations\n");
    break;
  case FAILED_FTOL_TOO_SMALL:
    s << (whoami ": ftol is too small. no further reduction in the sum of squares is possible.\n");
    break;
  case FAILED_XTOL_TOO_SMALL:
    s << (whoami ": xtol is too small. no further improvement in the approximate solution x is possible.\n");
    break;
  case FAILED_GTOL_TOO_SMALL:
    s << (whoami ": gtol is too small. Fx is orthogonal to the columns of the jacobian to machine precision.\n");
    break;
  default:
    s << (whoami ": OIOIOI: unkown info code from lmder.\n");
    break;
  }
  int m = f_->GetNumberOfResiduals();
  s << whoami ": " << num_iterations_ << " iterations, "
    << num_evaluations_ << " evaluations, "<< m <<" residuals.  RMS error start/end "
    << GetStartError() << "/" << GetEndError() << std::endl;
#undef whoami
}

/* fjac is an output m by n array. the upper n by n submatrix */
/*         of fjac contains an upper triangular matrix r with */
/*         diagonal elements of nonincreasing magnitude such that */

/*                t     t           t */
/*               p *(jac *jac)*p = r *r, */

/*         where p is a permutation matrix and jac is the final */
/*         calculated jacobian. column j of p is column ipvt(j) */
/*         (see below) of the identity matrix. the lower trapezoidal */
/*         part of fjac contains information generated during */
/*         the computation of r. */

// fdjac is target m*n

VNL::Matrix<double> const& VNL::LevenbergMarquardt::GetJtJ()
{
  if (!set_covariance_) {
    std::cerr << __FILE__ ": get_covariance() not implemented yet\n";
    set_covariance_ = true;
  }
  return *covariance_;
}
