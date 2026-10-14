// This is vxl/VNL/algo/rpoly_roots.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
*
* \author Andrew W. Fitzgibbon, Oxford RRG
* \date   06 Aug 96
*
*/
#include <VNL/VC6WarningsFix.h>

#include <cmath> // for fabs()
#include <iostream>
#include <complex>
#include <VNL/realpolynomial.h>

#include <VNL/NetLib/netlib.h> // rpoly_()
#include "rpolyroots.h"


// - The constructor calculates the roots.  This is the most efficient interface
// as all the result variables are initialized to the correct size.
// The polynomial is $ a[0] x^d + a[1] x^{d-1} + \cdots + a[d] = 0 $.
// Note that if the routine fails, not all roots will be found.  In this case,
// the "realroots" and "roots" functions will return fewer than n roots.
VNL::RPolyRoots::RPolyRoots(const VNL::Vector<double>& a)
  : coeffs_(a), r_(coeffs_.size()-1), i_(coeffs_.size()-1)
{
  // fsm : if the coefficients are NaNs then rpoly_ gets stuck in an
  // infinite loop. of course, the caller shouldn't pass in NaNs, but
  // it would be nice to get an error message instead of hanging.
  a.assert_finite();

  Compute();
}

VNL::RPolyRoots::RPolyRoots(const VNL::RealPolynomial& poly)
  : coeffs_(poly.Coefficients()), r_(poly.Degree()), i_(poly.Degree())
{
  poly.Coefficients().assert_finite();

  Compute();
}

// - Complex vector of all roots.
VNL::Vector<std::complex<double> > VNL::RPolyRoots::Roots() const
{
  VNL::Vector<std::complex<double> > ret(num_roots_found_);
  for (int i = 0; i < num_roots_found_; ++i)
    ret[i] = std::complex<double>(r_[i], i_[i]);
  return ret;
}

// - Return real roots only.  Roots are real if the absolute value
// of their imaginary part is less than the optional argument TOL.
// TOL defaults to 1e-12 [untested]
VNL::Vector<double> VNL::RPolyRoots::RealRoots(double tol) const
{
  int c = 0;
  for (int i = 0; i < num_roots_found_; ++i)
    if (fabs(i_[i]) < tol)
      ++c;

  VNL::Vector<double> ret(c);
  c = 0;
  {for (int i = 0; i < num_roots_found_; ++i)
    if (fabs(i_[i]) < tol)
      ret[c++] = r_[i];}

  return ret;
}

/** Compute roots using Jenkins-Traub algorithm.
* Calls rpoly and interprets failure codes.
*/
bool VNL::RPolyRoots::Compute()
{
  int fail = 0;
  int n = coeffs_.size() - 1;

  rpoly_(coeffs_.DataBlock(), &n, r_.DataBlock(), i_.DataBlock(), &fail);

  if (!fail) {
    num_roots_found_ = n;
    return true;
  }

  num_roots_found_ = n;

  if (coeffs_[0] == 0.0)
    std::cerr << "rpoly_roots: Leading coefficient is zero.  Not allowed.\n";
  else
    std::cerr << "rpoly_roots: Calculation failed, only " << n << " roots found\n";

  return false;
}
