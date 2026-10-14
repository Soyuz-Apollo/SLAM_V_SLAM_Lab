// This is vxl/VNL/conjugate_gradient.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author Geoffrey Cross, Oxford RRG
* \date   15 Feb 99
*
*/
#include "conjugategradient.h"

#include <cstdlib>
#include <iostream>

#include <VNL/costfunction.h>
#include <VNL/vectorref.h>

using namespace VNL;




// external netlib function
extern "C"
int cg_( double *x,                     // IO start guess
         double *e,                     // O max-norm of gradient
         int    *it,                    // O number of iterations performed
         double *step,                  // I step=0 make guess at first direction
                                        // O step size along search direction for final iteration
         double *t,                     // I tolerance (iterations stop when max-norm of gradient < t)
         int *limit,                    // I maximum number of iterations
         int *n,                        // I number of unknowns
         int *m,                        // I number of iterations before renormalizing (normally m=n)
         double value( double *x),      // I value(x) is cost at x
         int grad( double *g,
                   double *x),          // I grad(g,x) puts gradient into g at x
         int both( double *v,
                   double *g,
                   double *x),          // I both(v,g,x) puts value in v and gradient in g at x
         int pre( double *y,
                  double *z),           // I preconditions (not necessarily needed) pre(y,z)
         double *h );                   // I space to work size h = 3*n

/////////////////////////////////////

namespace VNL {
class ConjugateGradientActivate {
public:
  static ConjugateGradient* current;

  ConjugateGradientActivate(ConjugateGradient* minimizer) {
    if (current) {
      std::cerr << "conjugate_gradient: ERROR: Nested minimizations not supported.\n";
      abort();
      // This is a copy of what goes on in LevenbergMarquardt, so if awf decides to
      // fix that one, then maybe he could do the same here...
    }
    current = minimizer;
  }
  ~ConjugateGradientActivate() {
    current = 0;
  }
};
}; // End namespace VNL


ConjugateGradient *ConjugateGradientActivate::current= 0;


/////////////////////////////////////

ConjugateGradient::~ConjugateGradient()
{
}

void ConjugateGradient::Init(CostFunction &f)
{
  f_= &f;
  num_iterations_ = 0;
  num_evaluations_ = 0;
  start_error_ = 0;
  end_error_ = 0;
}

///////////////////////////////////////

double ConjugateGradient::ValueComputer_(double *x)
{
  ConjugateGradient* active = ConjugateGradientActivate::current;
  CostFunction* f = active->f_;
  VectorRef<double> ref_x(f->GetNumberOfUnknowns(), x);

  active->num_evaluations_++;

  return f->F(ref_x);
}

int ConjugateGradient::GradientComputer_(double *g, double *x)
{
  ConjugateGradient* active = ConjugateGradientActivate::current;
  CostFunction* f = active->f_;
  VectorRef<double> ref_x(f->GetNumberOfUnknowns(), x);
  VectorRef<double> ref_g(f->GetNumberOfUnknowns(), g);

  f->GradF(ref_x, ref_g);

  return 0;
}

int ConjugateGradient::ValueAndGradientComputer_(double *v, double *g, double *x)
{
  ConjugateGradient* active = ConjugateGradientActivate::current;
  CostFunction* f = active->f_;
  VectorRef<double> ref_x(f->GetNumberOfUnknowns(), x);
  VectorRef<double> ref_g(f->GetNumberOfUnknowns(), g);

  f->Compute(ref_x, v, &ref_g);

  return 0;
}

int ConjugateGradient::Preconditioner_( double *out, double *in)
{
  // FIXME - there should be some way to set a preconditioner if you have one
  // e.g. P = inv(diag(A'A)) for linear least squares systems.

  ConjugateGradient* active = ConjugateGradientActivate::current;
  CostFunction* f = active->f_;

  int n = f->GetNumberOfUnknowns();
  for (int i=0; i < n; ++i)
    out[i] = in[i];

  return 0;
}

///////////////////////////////////////

// avoid anachronism warning from fussy compilers
#ifdef VCL_SUNPRO_CC
extern "C" double ConjugateGradient__valuecomputer_( double *x)
{
  return ConjugateGradient::ValueComputer_(x);
}
extern "C" int ConjugateGradient__gradientcomputer_( double *g, double *x)
{
  return ConjugateGradient::GradientComputer_(g,x);
}
extern "C" int ConjugateGradient__valueandgradientcomputer_( double *v, double *g, double *x)
{
  return ConjugateGradient::ValueAndGradientComputer_(v,g,x);
}
extern "C" int ConjugateGradient__preconditioner_( double *out, double *in)
{
  return ConjugateGradient::Preconditioner_(out,in);
}
#endif

bool ConjugateGradient::Minimize( Vector<double> &x)
{
  double *xp = x.DataBlock();
  double max_norm_of_gradient;
  int number_of_iterations;
  final_step_size_ = 0;
  double gradient_tolerance = gtol;
  Vector<double> workspace(f_->GetNumberOfUnknowns()*3);
  int number_of_unknowns = f_->GetNumberOfUnknowns();

  ConjugateGradientActivate activator(this);

  start_error_ = ValueComputer_(xp);
  num_evaluations_ = 0;

  cg_( xp,
       &max_norm_of_gradient,
       &number_of_iterations,
       &final_step_size_,
       &gradient_tolerance,
       &maxfev,
       &number_of_unknowns,
       &number_of_unknowns,
#ifdef VCL_SUNPRO_CC
       ConjugateGradient__valuecomputer_,
       ConjugateGradient__gradientcomputer_,
       ConjugateGradient__valueandgradientcomputer_,
       ConjugateGradient__preconditioner_,
#else
       ValueComputer_,
       GradientComputer_,
       ValueAndGradientComputer_,
       Preconditioner_,
#endif
       workspace.DataBlock());

  end_error_= ValueComputer_(xp);
  num_iterations_ = number_of_iterations;

  return true;
}


void ConjugateGradient::DiagnoseOutcome(std::ostream& os) const
{
  os << "conjugate_gradient: "
     << num_iterations_
     << " iterations, "
     << num_evaluations_
     << " evaluations. Cost function reported error"
     << f_->ReportedError(start_error_)
     << "/"
     << f_->ReportedError(end_error_)
     << " . Final step size = " << final_step_size_
     << std::endl;
}

void ConjugateGradient::DiagnoseOutcome() const
{
  DiagnoseOutcome(std::cout);
}
