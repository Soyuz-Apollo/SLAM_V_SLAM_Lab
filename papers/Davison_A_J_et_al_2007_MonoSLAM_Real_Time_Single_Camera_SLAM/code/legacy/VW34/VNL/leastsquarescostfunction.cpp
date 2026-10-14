// This is vxl/VNL/least_squares_cost_function.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// vnl_least_squares_cost_function
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 20 Aug 99
//
//-----------------------------------------------------------------------------

#include "leastsquarescostfunction.h"

using namespace VNL;

VNL::LeastSquaresCostFunction::LeastSquaresCostFunction(LeastSquaresFunction* f):
  CostFunction(f->GetNumberOfUnknowns()),
  storage_(f->GetNumberOfResiduals()),
  jacobian_(f->GetNumberOfResiduals(), f->GetNumberOfUnknowns()),
  f_(f)
{
}

double VNL::LeastSquaresCostFunction::F(const Vector<double>& x)
{
  f_->F(x, storage_);
  return storage_.SquaredMagnitude();
}

void VNL::LeastSquaresCostFunction::GradF(const Vector<double>& x, 
					  Vector<double>& gradient)
{
  // residuals = a, b, c, ...
  // params = x, y, z, ...
  // f = a^2 + b^2 + c^2 + ...
  // df/dx = 2a*da/dx + 2b*db/dx + ...

  if (f_->HasGradient()) {
    f_->F(x,storage_);
    f_->GradF(x, jacobian_);
    for (unsigned int c=0; c<jacobian_.Columns(); ++c) {
      gradient[c] = 0.0;
      for (unsigned int r=0; r<jacobian_.Rows(); ++r)
        gradient[c] += storage_[r] * jacobian_(r,c);
      gradient[c] *= 2;
    }
  }
}
