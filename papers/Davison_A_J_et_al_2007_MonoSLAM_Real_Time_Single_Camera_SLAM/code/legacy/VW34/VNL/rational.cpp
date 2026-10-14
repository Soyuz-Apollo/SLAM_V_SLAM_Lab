// This is ./vxl/VNL/rational.cxx
#include <VNL/VC6WarningsFix.h>
#include <VNL/rational.h>

/**
* \file
*/

VNL::Rational::Rational(double d)
{
  bool sign = d<0;
  if (sign) d = -d;

  // Continued fraction approximation of abs(d): recursively determined
  long den=0L, num=1L, prev_den=1L, prev_num=0L;

  while (d*num < 1e9 && d*den < 1e9) {
    long a = (long)d; // integral part of d
    d -= a; // certainly >= 0
    long temp = num; num = a*num + prev_num; prev_num = temp;
         temp = den; den = a*den + prev_den; prev_den = temp;
    if (d < 1e-6) break;
    d = 1/d;
  }
  num_ = num; den_ = den;
  if (sign) num_ = -num_;
  // no need to normalize() since prev_num and prev_den have guaranteed a gcd=1
}

const VNL::Rational VNL::NumericTraits<VNL::Rational>::zero = VNL::Rational(0L,1L);
const VNL::Rational VNL::NumericTraits<VNL::Rational>::one = VNL::Rational(1L,1L);

const std::complex<VNL::Rational> VNL::NumericTraits<std::complex<VNL::Rational> >::zero
  = std::complex<VNL::Rational>(VNL::Rational(0L,1L),VNL::Rational(0L,1L));
const std::complex<VNL::Rational> VNL::NumericTraits<std::complex<VNL::Rational> >::one
  = std::complex<VNL::Rational>(VNL::Rational(1L,1L),VNL::Rational(0L,1L));

#ifdef VCL_GCC_EGCS // this includes egcs, gcc 2.8 and gcc 2.95
#include <std/complext.cc>
template complex<VNL::Rational>& __doadv<VNL::Rational>(complex<VNL::Rational>*, complex<VNL::Rational> const&);
template complex<VNL::Rational> operator/(complex<VNL::Rational> const&, complex<VNL::Rational> const&);
#endif
