// This is vxl/VNL/least_squares_function.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author Andrew W. Fitzgibbon, Oxford RRG
* \date   31 Aug 96
*/

#include <iostream>
#include "leastsquaresfunction.h"
#include <assert.h>

VNL::LeastSquaresFunction::LeastSquaresFunction(int number_of_unknowns,
						int number_of_residuals,
						UseGradient g)
  : failure(false)
{
  n_ = number_of_residuals;
  p_ = number_of_unknowns;
  assert(n_>=0);
  assert(p_>=0);
  use_gradient_ = (g == use_gradient);

  if (number_of_unknowns > number_of_residuals) {
    std::cerr << "least_squares_function: WARNING: "
      "unknowns(" << number_of_unknowns << ")"
      " > "
      "residuals("<< number_of_residuals << ")"
      "\n";
  }
}

void VNL::LeastSquaresFunction::_init(int number_of_unknowns,
                                      int number_of_residuals)
{
  n_ = number_of_residuals;
  p_ = number_of_unknowns;
  assert(n_>=0);
  assert(p_>=0);

  if (number_of_unknowns > number_of_residuals) {
    std::cerr << "least_squares_function: WARNING: "
      "unknowns(" << number_of_unknowns << ")"
      " > "
      "residuals("<< number_of_residuals << ")"
      "\n";
  }
}

VNL::LeastSquaresFunction::~LeastSquaresFunction()
{
}

void VNL::LeastSquaresFunction::ThrowFailure()
{
  //std::cerr << "throw_failure()\n";
  failure = true;
}

void VNL::LeastSquaresFunction::ClearFailure()
{
  //std::cerr << "clear_failure()\n";
  failure = false;
}

void VNL::LeastSquaresFunction::GradF(VNL::Vector<double> const& /*x*/,
                                       VNL::Matrix<double>& /*jacobian*/)
{
}

/** Compute fd gradient.
*/
void VNL::LeastSquaresFunction::FDGradF(VNL::Vector<double> const& x,
                                         VNL::Matrix<double>& jacobian,
                                         double stepsize)
{
  int dim = x.size();
  int n = jacobian.Rows();
  assert(dim == GetNumberOfUnknowns());
  assert(n == GetNumberOfResiduals());
  assert(dim == int(jacobian.Columns()));

  VNL::Vector<double> tx = x;
  VNL::Vector<double> fplus(n);
  VNL::Vector<double> fminus(n);
  double h = stepsize;
  for (int i = 0; i < dim; ++i)
  {
    double tplus = x[i] + h;
    tx[i] = tplus;
    this->F(tx, fplus);

    double tminus = x[i] - h;
    tx[i] = tminus;
    this->F(tx, fminus);

    double h = (tplus - tminus);
    for (int j = 0; j < n; ++j)
      jacobian(j,i) = (fplus[j] - fminus[j]) / h;

    tx[i] = x[i];
  }
}

void VNL::LeastSquaresFunction::Trace(int /* iteration */,
                                       VNL::Vector<double> const& /*x*/,
                                       VNL::Vector<double> const& /*fx*/)
{
}

double VNL::LeastSquaresFunction::RMS(VNL::Vector<double> const& x)
{
  VNL::Vector<double> fx(n_);
  F(x, fx);
  return fx.RMS();
}
