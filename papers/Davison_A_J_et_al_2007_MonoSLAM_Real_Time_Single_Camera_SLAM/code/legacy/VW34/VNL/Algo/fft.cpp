// This is vxl/VNL/algo/fft.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author fsm@robots.ox.ac.uk
*/
#include <VNL/VC6WarningsFix.h>
#include "fft.h"

#include <VNL/NetLib/netlib.h>

void VNL::FFTSetgpfa(float *triggs, int size, int pqr[3], int *info)
{
  setgpfa_(triggs, &size, pqr, info);
}

void VNL::FFTSetgpfa(double *triggs, int size, int pqr[3], int *info)
{
  dsetgpfa_(triggs, &size, pqr, info);
}

//----------------------------------------------------------------------

void VNL::FFTgpfa(float  *a, float  *b, float const  *triggs,
                  int inc, int jump, int n,
                  int lot, int isign, int const pqr[3], int *info)
{
  gpfa_(a, b, triggs, &inc, &jump, &n, &lot, &isign, pqr, info);
}

void VNL::FFTgpfa(double *a, double *b, double const *triggs,
                  int inc, int jump, int n,
                  int lot, int isign, int const pqr[3], int *info)
{
  dgpfa_(a, b, triggs, &inc, &jump, &n, &lot, &isign, pqr, info);
}
