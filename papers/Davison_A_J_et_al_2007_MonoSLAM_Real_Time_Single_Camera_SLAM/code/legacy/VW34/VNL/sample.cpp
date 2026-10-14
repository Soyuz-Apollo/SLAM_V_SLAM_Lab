// This is vxl/VNL/sample.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author fsm@robots.ox.ac.uk
*/

#include "sample.h"
#include <VNL/vnlmath.h>

#include <cmath>
#include <cstdlib>

#if VXL_STDLIB_HAS_DRAND48
// On SunPro 5.0, <cstdlib> does not
// declare drand48() but <stdlib.h> does.
# include <stdlib.h> // dont_vxl_filter
#endif


#if !VXL_STDLIB_HAS_DRAND48
// rand() is not always a good random number generator,
// so use the following congruential random number generator - PVr
static unsigned long vnl_sample_seed = 12345;
#endif

# include <ctime>

void VNL::SampleReseed()
{
# if VXL_STDLIB_HAS_DRAND48
  srand48( time(0) );
# else
  vnl_sample_seed = time(0);
# endif
}

void VNL::SampleReseed(int seed)
{
# if VXL_STDLIB_HAS_DRAND48
  srand48( seed );
# else
  vnl_sample_seed = seed;
# endif
}

/** return a random number uniformly drawn on [a, b).
*/
double VNL::SampleUniform(double a, double b)
{
#if VXL_STDLIB_HAS_DRAND48
  // it's your lucky day.
  double u = drand48(); // uniform on [0, 1)
#else
  // unlucky! perhaps your luck will change if you try
  // a different random number generator.
  vnl_sample_seed = (vnl_sample_seed*16807)%2147483647L;
  double u = double(vnl_sample_seed)/2147483711L;
#endif
  return (1.0 - u)*a + u*b;
}

void VNL::SampleNormal2(double *x, double *y)
{
  double u     = VNL::SampleUniform(0, 1);
  double theta = VNL::SampleUniform(0, 2 * VNL::Math::pi);

  double r = sqrt(-2*log(u));

  if (x) *x = r * cos(theta);
  if (y) *y = r * sin(theta);
}

double VNL::SampleNormal(double mean, double sigma)
{
  double x;
  VNL::SampleNormal2(&x, 0);
  return mean + sigma * x;
}
