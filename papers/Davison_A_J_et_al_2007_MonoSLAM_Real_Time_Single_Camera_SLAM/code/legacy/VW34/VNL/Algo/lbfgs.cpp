// This is vxl/VNL/algo/lbfgs.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
*
* \author Andrew W. Fitzgibbon, Oxford RRG
* \date   22 Aug 99
*
*/
#include <VNL/VC6StdFix.h>
#include "lbfgs.h"
#include <cmath>
#include <iostream>
#include <iomanip> // for setw (replaces cout.form())

#include <VNL/NetLib/netlib.h> // lbfgs_()

/** Default constructor.
* memory is set to 5, line_search_accuracy to 0.9.
* Calls init_parameters
*/
VNL::LBFGS::LBFGS():
  f_(0)
{
  _InitParameters();
}

/** Constructor.\ f is the cost function to be minimized.
* Calls init_parameters
*/
VNL::LBFGS::LBFGS(VNL::CostFunction& f):
  f_(&f)
{
  _InitParameters();
}

/** Called by constructors.
* Memory is set to 5, line_search_accuracy to 0.9, default_step_length to 1.
*/
void VNL::LBFGS::_InitParameters()
{
  memory = 5;
  line_search_accuracy = 0.9;
  default_step_length = 1.0;
}

#if 0
# include <netlib/f2c.h>
#else
typedef int integer;
typedef double doublereal;
#endif

/* Common Block Declarations */

// Declare this as "static" (=local) to avoid multiple definition, because
// this struct is also defined in v3p/netlib/lbfgs.c
static struct {
// C    GTOL is a DOUBLE PRECISION variable with default value 0.9, which
// C        controls the accuracy of the line search routine MCSRCH. If the
// C        function and gradient evaluations are inexpensive with respect
// C        to the cost of the iteration (which is sometimes the case when
// C        solving very large problems) it may be advantageous to set GTOL
// C        to a small value. A typical small value is 0.1.  Restriction:
// C        GTOL should be greater than 1.D-04.


// C    STPMIN and STPMAX are non-negative DOUBLE PRECISION variables which
// C        specify lower and uper bounds for the step in the line search.
// C        Their default values are 1.D-20 and 1.D+20, respectively. These
// C        values need not be modified unless the exponents are too large
// C        for the machine being used, or unless the problem is extremely
// C        badly scaled (in which case the exponents should be increased).
  integer mp, lp; // Fortran i/o stuff.  Unused here.
  doublereal gtol, stpmin, stpmax;
  doublereal stpawf; // line search default step length, added by awf
} lb3_; // SGI CC warns here about unused variable. Just ignore it.

#define lb3_1 lb3_

bool VNL::LBFGS::Minimize(VNL::Vector<double>& x)
{
  /* Local variables */
  /*     The driver for vnl_lbfgs must always declare LB2 as EXTERNAL */

  int n = f_->GetNumberOfUnknowns();
  int m = memory; // The number of basis vectors to remember.

  int iprint[2] = {1, 0};
  VNL::Vector<double> g(n);

  // Workspace
  VNL::Vector<double> diag(n);

  VNL::Vector<double> w(n * (2*m+1)+2*m);

  if (verbose_)
    std::cerr << "lbfgs: n = "<< n <<", memory = "<< m <<", Workspace = "
             << w.size() << "[ "<< ( w.size() / 128.0 / 1024.0) <<" MB], ErrorScale = "
             << f_->ReportedError(1) <<", xnorm = "<< x.Magnitude() << std::endl;

  bool we_trace = (verbose_ && !trace);

  if (we_trace)
    std::cerr << "lbfgs: ";

  double best_f = 0;
  VNL::Vector<double> best_x;

  bool ok = true;
  this->num_evaluations_ = 0;
  this->num_iterations_ = 0;
  int iflag = 0;
  while (true) {
    // We do not wish to provide the diagonal matrices Hk0, and therefore set DIAGCO to FALSE.
    logical diagco = false;

    // Set these every iter in case user changes them to bail out
    double eps = gtol; // Gradient tolerance
    double local_xtol = 1e-16;
    lb3_.gtol = line_search_accuracy; // set to 0.1 for huge problems or cheap functions
    lb3_.stpawf = default_step_length;

    // Call function
    double f;
    f_->Compute(x, &f, &g);
    if (this->num_evaluations_ == 0) {
      this->start_error_ = f;
      best_f = f;
    } else if (f < best_f) {
      best_x = x;
      best_f = f;
    }

#define print_(i,a,b,c,d) std::cerr<<std::setw(6)<<i<<' '<<std::setw(20)<<a<<' '\
           <<std::setw(20)<<b<<' '<<std::setw(20)<<c<<' '<<std::setw(20)<<d<<'\n'

    if (check_derivatives_)
    {
      std::cerr << "lbfgs: f = " << f_->ReportedError(f) << ", computing FD gradient\n";
      VNL::Vector<double> fdg = f_->FDGradF(x);
      if (verbose_)
      {
        int l = n;
        int limit = 100;
        int limit_tail = 10;
        if (l > limit + limit_tail) {
          std::cerr << " [ Showing only first " <<limit<< " components ]\n";
          l = limit;
        }
        print_("i","x","g","fdg","dg");
        print_("-","-","-","---","--");
        for (int i = 0; i < l; ++i)
          print_(i, x[i], g[i], fdg[i], g[i]-fdg[i]);
        if (n > limit) {
          std::cerr << "   ...\n";
          for (int i = n - limit_tail; i < n; ++i)
            print_(i, x[i], g[i], fdg[i], g[i]-fdg[i]);
        }
      }
      std::cerr << "   ERROR = " << (fdg - g).SquaredMagnitude() / std::sqrt(double(n)) << "\n";
    }

    iprint[0] = trace ? 1 : -1; // -1 no o/p, 0 start and end, 1 every iter.
    iprint[1] = 0; // 1 prints X and G
    lbfgs_(&n, &m, x.DataBlock(), &f, g.DataBlock(), &diagco, diag.DataBlock(),
           iprint, &eps, &local_xtol, w.DataBlock(), &iflag);
    ++this->num_iterations_;

    if (we_trace)
      std::cerr << iflag << ":" << f_->ReportedError(f) << " ";

    if (iflag == 0) {
      // Successful return
      this->end_error_ = f;
      ok = true;
      x = best_x;
      break;
    }

    if (iflag < 0) {
      // Eeek.
      std::cerr << "lbfgs: ** EEEK **\n";
      ok = false;
      x = best_x;
      break;
    }

    if (++this->num_evaluations_ > GetMaxFunctionEvals()) {
      failure_code_ = FAILED_TOO_MANY_ITERATIONS;
      ok = false;
      x = best_x;
      break;
    }
  }
  if (we_trace) std::cerr << "done\n";

  return ok;
}
