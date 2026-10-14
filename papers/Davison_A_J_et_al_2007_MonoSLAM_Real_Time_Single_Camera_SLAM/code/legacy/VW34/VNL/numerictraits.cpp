// This is vxl/VNL/numeric_traits.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// vnl_numeric_traits
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 12 Feb 98
//
//-----------------------------------------------------------------------------

#include <VNL/numerictraits.h>
#include <complex>

#ifdef _WIN32

#ifndef NO_STD_BOOL
const bool VNL::NumericTraits<bool>::zero = 0;
const bool VNL::NumericTraits<bool>::one = 1;
#endif

const char VNL::NumericTraits<char>::zero = 0;
const char VNL::NumericTraits<char>::one = 1;

const unsigned char VNL::NumericTraits<unsigned char>::zero = 0;
const unsigned char VNL::NumericTraits<unsigned char>::one = 1;

const signed char VNL::NumericTraits<signed char>::zero = 0;
const signed char VNL::NumericTraits<signed char>::one = 1;

const unsigned short VNL::NumericTraits<unsigned short>::zero = 0;
const unsigned short VNL::NumericTraits<unsigned short>::one = 1;

const signed short VNL::NumericTraits<signed short>::zero = 0;
const signed short VNL::NumericTraits<signed short>::one = 1;

const unsigned int VNL::NumericTraits<unsigned int>::zero = 0;
const unsigned int VNL::NumericTraits<unsigned int>::one = 1;

const signed int VNL::NumericTraits<signed int>::zero = 0;
const signed int VNL::NumericTraits<signed int>::one = 1;

const signed long VNL::NumericTraits<signed long>::zero = 0L;
const signed long VNL::NumericTraits<signed long>::one = 1L;

const unsigned long VNL::NumericTraits<unsigned long>::zero = 0L;
const unsigned long VNL::NumericTraits<unsigned long>::one = 1L;

#endif

#if _WIN32

const float VNL::NumericTraits<float>::zero = 0.0F;
const float VNL::NumericTraits<float>::one = 1.0F;

const double VNL::NumericTraits<double>::zero = 0.0;
const double VNL::NumericTraits<double>::one = 1.0;

const long double VNL::NumericTraits<long double>::zero = 0.0;
const long double VNL::NumericTraits<long double>::one = 1.0;

#endif

//--------------------------------------------------------------------------------

const std::complex<float> VNL::NumericTraits<std::complex<float> >::zero = 0.0;
const std::complex<float> VNL::NumericTraits<std::complex<float> >::one = 1.0;

const std::complex<double> VNL::NumericTraits<std::complex<double> >::zero = 0.0;
const std::complex<double> VNL::NumericTraits<std::complex<double> >::one = 1.0;

const std::complex<long double> VNL::NumericTraits<std::complex<long double> >::zero = 0.0;
const std::complex<long double> VNL::NumericTraits<std::complex<long double> >::one = 1.0;

//--------------------------------------------------------------------------------
