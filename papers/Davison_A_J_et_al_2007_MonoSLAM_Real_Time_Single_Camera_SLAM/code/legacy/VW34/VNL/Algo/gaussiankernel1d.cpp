// This is vxl/VNL/algo/gaussian_kernel_1d.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author Andrew W. Fitzgibbon, Oxford RRG
* \date   07 Aug 97
*
*/

#include "gaussiankernel1d.h"
#include <cmath>
#include <VNL/vnlmath.h>

// G(x) = 1/(sigma * sqrt(2*pi)) * exp(-0.5 * (x/sigma)^2)
// x(g) = sigma * sqrt(-2 * log(g * sigma * sqrt(2*pi) ) )

// Compute the x value at which a Gaussian becomes lower than cutoff.
static inline
double compute_width(double sigma, double cutoff)
{
using namespace std;
  return sigma * sqrt(-2 * log(cutoff * sigma * sqrt(2*VNL::Math::pi)));
}

/** Construct a sampled 1D gaussian of standard deviation sigma.
* The vector is normalized so that its sum is 0.5.
*/
vnl_gaussian_kernel_1d::vnl_gaussian_kernel_1d(double sigma, double cutoff):
  vec_((int)ceil(compute_width(sigma, cutoff)))
{
  int width = vec_.size();
  inscale_ = 0.5/(sigma * sigma);
  double area = 0;
  for (int i = 0; i < width; ++i) {
    double v = G(i);
    area += v;
    vec_[i] = v;
  }
  vec_ *= (0.5/area);
}

double vnl_gaussian_kernel_1d::G(double x) const
{
  return exp(-x*x * inscale_);
}
