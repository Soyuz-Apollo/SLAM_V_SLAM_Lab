// This is vxl/VNL/numeric_limits.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// numeric_limits
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 28 Aug 96
//
//-----------------------------------------------------------------------------

#include "numericlimits.h"

union vnl_numeric_limits_double_nan {
  double nan;
  unsigned char x[8];

  vnl_numeric_limits_double_nan() {
    x[0] = 0x7f;
    x[1] = 0xff;
    x[2] = 0xff;
    x[3] = 0xff;
    x[4] = 0xff;
    x[5] = 0xff;
    x[6] = 0xff;
    x[7] = 0xff;
  }
};
static vnl_numeric_limits_double_nan dnan;

union vnl_numeric_limits_double_inf {
  double inf;
  unsigned char x[8];

  vnl_numeric_limits_double_inf() {
    x[0] = 0x7f;
    x[1] = 0xf0;
    x[2] = 0x00;
    x[3] = 0x00;
    x[4] = 0x00;
    x[5] = 0x00;
    x[6] = 0x00;
    x[7] = 0x00;
  }
};
static vnl_numeric_limits_double_inf dinf;

//template <>
double VNL::NumericLimits<double>::infinity()
{
  return dinf.inf;
}

//template <>
double VNL::NumericLimits<double>::quiet_NaN()
{
  return dnan.nan;
}

//template <>
double VNL::NumericLimits<double>::signaling_NaN()
{
  return quiet_NaN();
}

// *****************************************************************************
union vnl_numeric_limits_float_nan {
  float nan;
  unsigned char x[4];

  vnl_numeric_limits_float_nan() {
    x[0] = 0x7f;
    x[1] = 0xff;
    x[2] = 0xff;
    x[3] = 0xff;
  }
};
static vnl_numeric_limits_float_nan fnan;

union vnl_numeric_limits_float_inf {
  float inf;
  unsigned char x[4];

  vnl_numeric_limits_float_inf() {
    x[0] = 0x7f;
    x[1] = 0x80;
    x[2] = 0x00;
    x[3] = 0x00;
  }
};
static vnl_numeric_limits_float_inf finf;

//template <>
float VNL::NumericLimits<float>::infinity()
{
  return finf.inf;
}

//template <>
float VNL::NumericLimits<float>::quiet_NaN()
{
  return fnan.nan;
}

//template <>
float VNL::NumericLimits<float>::signaling_NaN()
{
  return quiet_NaN();
}

#ifdef _WIN32
// float
const bool VNL::NumericLimits<float>::is_specialized = true;
const int  VNL::NumericLimits<float>::digits   = 24;
const int  VNL::NumericLimits<float>::digits10 =  6;
const bool VNL::NumericLimits<float>::is_signed  = true;
const bool VNL::NumericLimits<float>::is_integer = false;
const bool VNL::NumericLimits<float>::is_exact   = false;
const int  VNL::NumericLimits<float>::radix = 2;
const int  VNL::NumericLimits<float>::min_exponent   = -125;
const int  VNL::NumericLimits<float>::min_exponent10 = -37;
const int  VNL::NumericLimits<float>::max_exponent   = 128;
const int  VNL::NumericLimits<float>::max_exponent10 = 38;
const bool VNL::NumericLimits<float>::has_infinity      = true;
const bool VNL::NumericLimits<float>::has_quiet_NaN     = true;
const bool VNL::NumericLimits<float>::has_signaling_NaN = true;
const bool VNL::NumericLimits<float>::has_denorm        = false;
const bool VNL::NumericLimits<float>::is_iec559  = true;
const bool VNL::NumericLimits<float>::is_bounded = true;
const bool VNL::NumericLimits<float>::is_modulo  = false;
const bool VNL::NumericLimits<float>::traps      = true;
const bool VNL::NumericLimits<float>::tinyness_before = true;
const VNL::FloatRoundStyle VNL::NumericLimits<float>::round_style = VNL::vnl_round_to_nearest;

const bool VNL::NumericLimits<double>::is_specialized = true;
const int  VNL::NumericLimits<double>::digits   = 53;
const int  VNL::NumericLimits<double>::digits10 =  15;
const bool VNL::NumericLimits<double>::is_signed  = true;
const bool VNL::NumericLimits<double>::is_integer = false;
const bool VNL::NumericLimits<double>::is_exact   = false;
const int  VNL::NumericLimits<double>::radix = 2;
const int  VNL::NumericLimits<double>::min_exponent   = -1021;
const int  VNL::NumericLimits<double>::min_exponent10 = -307;
const int  VNL::NumericLimits<double>::max_exponent   = 1024;
const int  VNL::NumericLimits<double>::max_exponent10 = 308;
const bool VNL::NumericLimits<double>::has_infinity      = true;
const bool VNL::NumericLimits<double>::has_quiet_NaN     = true;
const bool VNL::NumericLimits<double>::has_signaling_NaN = true;
const bool VNL::NumericLimits<double>::has_denorm        = true;
const bool VNL::NumericLimits<double>::is_iec559  = true;
const bool VNL::NumericLimits<double>::is_bounded = true;
const bool VNL::NumericLimits<double>::is_modulo  = false;
const bool VNL::NumericLimits<double>::traps      = true;
const bool VNL::NumericLimits<double>::tinyness_before = true;
const VNL::FloatRoundStyle VNL::NumericLimits<double>::round_style = VNL::vnl_round_to_nearest;

#endif
