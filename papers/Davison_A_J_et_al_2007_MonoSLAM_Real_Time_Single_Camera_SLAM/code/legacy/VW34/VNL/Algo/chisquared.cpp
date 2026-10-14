// This is vxl/VNL/algo/chi_squared.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
*/
#include <VNL/VC6WarningsFix.h>
#include "chisquared.h"

#include <cmath>

// FORTRAN routine
#include <VNL/NetLib/netlib.h> // dchscdf_()

/** Compute cumulative distribution function for chi-squared distribution.
* This subroutine computes the cumulative distribution function
* value for the chi-squared distribution with integer degrees of
* freedom parameter = dof.  This distribution is defined for all
* non-negative chisq.  Thus if a random variable x is drawn from a
* chi-squared distribution with d degrees of freedom, then P(x < X) =
* VNL::ChiSquaredCumulative(X,d).
*/
double VNL::ChiSquaredCumulative(double chisq, int dof) {
  double cdf;
  dchscdf_(&chisq,&dof,&cdf);
  return cdf;
}

//------------------------------------------------------------

template <class T>
double VNL::ChiSquaredStatistic1 (T const *A, T const *B, int n, bool normalize)
{
  double sum = 0;

  if (normalize) {
    T sumA = 0;
    T sumB = 0;
    for (int i=0; i<n; ++i) {
      sumA += A[i];
      sumB += B[i];
    }

    {for (int i=0; i<n; ++i)
      if (A[i]) {
        double a = double(A[i])/sumA;
        double b = double(B[i])/sumB;
        double tmp = a - b;
        sum += tmp*tmp/a;
	}}
  }
  else {
    for (int i=0; i<n; ++i)
      if (A[i]) {
        double tmp = A[i] - B[i];
        sum += tmp*tmp/A[i];
      }
  }

  return sum;
}

template <class T>
double VNL::ChiSquaredStatistic2 (T const *A, T const *B, int n, bool normalize)
{
  return VNL::ChiSquaredStatistic1(B, A, n, normalize);
}

template <class T>
double VNL::ChiSquaredStatistic12(T const *A, T const *B, int n, bool normalize)
{
  double sum = 0;

  if (normalize) {
    T sumA = 0;
    T sumB = 0;
    for (int i=0; i<n; ++i) {
      sumA += A[i];
      sumB += B[i];
    }

    {for (int i=0; i<n; ++i)
      if (A[i] || B[i]) {
        double a = double(A[i])/sumA;
        double b = double(B[i])/sumB;
        double tmp = a - b;
        sum += tmp*tmp/(a + b);
	}}
  }
  else {
    for (int i=0; i<n; ++i)
      if (A[i] || B[i]) {
        double tmp = A[i] - B[i];
        sum += tmp*tmp/(A[i] + B[i]);
      }
  }

  return sum;
}

#define inst(T) \
namespace VNL { \
  template double ChiSquaredStatistic1 (T const *, T const *, int, bool); \
  template double ChiSquaredStatistic2 (T const *, T const *, int, bool); \
  template double ChiSquaredStatistic12(T const *, T const *, int, bool); \
};

inst(int);
inst(double);
