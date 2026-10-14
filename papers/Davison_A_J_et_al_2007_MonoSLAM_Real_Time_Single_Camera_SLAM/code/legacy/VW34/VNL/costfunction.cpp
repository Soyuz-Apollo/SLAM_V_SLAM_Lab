// This is vxl/VNL/cost_function.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author Andrew W. Fitzgibbon, Oxford RRG
* \date   23 Oct 97
*
*/

#include <assert.h>
#include "costfunction.h"

static bool f_calling_compute;

void VNL::CostFunction::Compute(VNL::Vector<double> const& x, 
				double *f, 
				VNL::Vector<double>* g)
{
  if (f) *f = this->F(x);
  if (g) this->GradF(x, *g);
}

/** Default implementation of f is compute...
*/
double VNL::CostFunction::F(VNL::Vector<double> const& x)
{
  // if we get back here from compute, neither vf was implemented.
  if (f_calling_compute)
    assert(!"cost_function: RECURSION");
  double f;
  f_calling_compute = true;
  this->Compute(x, &f, 0);
  f_calling_compute = false;
  return f;
}

/** Default implementation of gradf is to call compute.
*/
void VNL::CostFunction::GradF(VNL::Vector<double> const& x, VNL::Vector<double>& g)
{
  if (f_calling_compute)
    assert(!"cost_function: RECURSION");
  f_calling_compute = true;
  this->Compute(x, 0, &g);
  f_calling_compute = false;
}

/** Compute fd gradient.
*/
void VNL::CostFunction::FDGradF(VNL::Vector<double> const& x,
                                VNL::Vector<double> &  gradient,
                                double stepsize )
{
  VNL::Vector<double> tx = x;
  double h = stepsize;
  for (int i = 0; i < dim; ++i) {

    double tplus = x[i] + h;
    tx[i] = tplus;
    double fplus = this->F(tx);

    double tminus = x[i] - h;
    tx[i] = tminus;
    double fminus = this->F(tx);

    gradient[i] = (fplus - fminus) / (tplus - tminus);
    tx[i] = x[i];
  }
}

VNL::Vector<double> VNL::CostFunction::GradF(VNL::Vector<double> const& x)
{
  VNL::Vector<double> g(dim);
  this->GradF(x, g);
  return g;
}

VNL::Vector<double> VNL::CostFunction::FDGradF(VNL::Vector<double> const& x)
{
  VNL::Vector<double> g(dim);
  this->FDGradF(x, g);
  return g;
}
