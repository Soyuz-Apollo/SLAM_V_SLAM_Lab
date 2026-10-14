// This is vxl/VNL/linear_system.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author David Capel, capes@robots
* \date   July 2000
*/

#include "linearsystem.h"
#include <assert.h>

VNL::LinearSystem::~LinearSystem()
{
}

void VNL::LinearSystem::ApplyPreconditioner(VNL::Vector<double> const& x, VNL::Vector<double> & px) const
{
  assert(px.size() == x.size());

  px = x;
}

double VNL::LinearSystem::GetRMSError(VNL::Vector<double> const& x) const
{
  VNL::Vector<double> resid(n_);
  VNL::Vector<double> b(n_);

  Multiply(x, resid);
  GetRHS(b);

  resid -= b;

  return resid.RMS();
}

double VNL::LinearSystem::GetRelativeResidual(VNL::Vector<double> const& x) const
{
  VNL::Vector<double> resid(n_);
  VNL::Vector<double> b(n_);

  Multiply(x, resid);
  GetRHS(b);

  resid -= b;

  return resid.RMS() / b.RMS();
}
