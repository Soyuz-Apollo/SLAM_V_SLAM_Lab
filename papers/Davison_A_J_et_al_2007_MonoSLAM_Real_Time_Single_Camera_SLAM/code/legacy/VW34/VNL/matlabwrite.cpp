// This is vxl/VNL/matlab_write.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author fsm@robots.ox.ac.uk
*/
#include <VNL/VC6WarningsFix.h>
#include "matlabwrite.h"

#include <iostream>
#include <cstring>  // strlen()
#include <complex>
#include <VNL/matlabheader.h>

// BJT: Hard code little-endianness since this #define doesn't exist

//#if VXL_LITTLE_ENDIAN // #ifdef i386
# define native_BYTE_ORDER VNL::MatlabHeader::VNL_LITTLE_ENDIAN
//#else
//# define native_BYTE_ORDER VNL::MatlabHeader::VNL_BIG_ENDIAN
//#endif

namespace VNL {

// SGI needs char * as first argument to std::ostream::write
void WriteBytes(std::ostream &s, void const *p, unsigned bytes) {
  s.write((char const *)p, bytes);
}


// ------------------------------ traits without tears ------------------------------

//awf: these cannot be static for sunpro 5...

// template <class T> long scalar_precision(T const &);
long ScalarPrecision(float  const &) { return MatlabHeader::VNL_SINGLE_PRECISION; }
long ScalarPrecision(double const &) { return MatlabHeader::VNL_DOUBLE_PRECISION; }
long ScalarPrecision(std::complex<float>  const &) { return MatlabHeader::VNL_SINGLE_PRECISION; }
long ScalarPrecision(std::complex<double> const &) { return MatlabHeader::VNL_DOUBLE_PRECISION; }

// template <class T> long is_complex(T const &);
long IsComplex(float  const &) { return 0; }
long IsComplex(double const &) { return 0; }
long IsComplex(std::complex<float>  const &) { return 1; }
long IsComplex(std::complex<double> const &) { return 1; }

// template <class T> void WriteReal(std::ostream &, T const *, unsigned );
void WriteReal(std::ostream &s, float const *data, unsigned n)
{ VNL::WriteBytes(s, data, n*sizeof(*data)); }
void WriteReal(std::ostream &s, double const *data, unsigned n)
{ VNL::WriteBytes(s, data, n*sizeof(*data)); }
void WriteReal(std::ostream &s, std::complex<float> const *data, unsigned n) {
  float dummy;
  for (unsigned i=0; i<n; ++i) { // real block
    dummy = std::real(data[i]);
    VNL::WriteBytes(s, &dummy, sizeof(dummy));
  }
}
void WriteReal(std::ostream &s, std::complex<double> const *data, unsigned n) {
  double dummy;
  for (unsigned i=0; i<n; ++i) { // real block
    dummy = std::real(data[i]);
    VNL::WriteBytes(s, &dummy, sizeof(dummy));
  }
}

// template <class T> void WriteImag(std::ostream &, T const *, unsigned );
void WriteImag(std::ostream &, float const *, unsigned ) { }
void WriteImag(std::ostream &, double const *, unsigned ) { }
void WriteImag(std::ostream &s, std::complex<float> const *data, unsigned n) {
  float dummy;
  for (unsigned i=0; i<n; ++i) { // imag block
    dummy = std::imag(data[i]);
    VNL::WriteBytes(s, &dummy, sizeof(dummy));
  }
}
void WriteImag(std::ostream &s, std::complex<double> const *data, unsigned n) {
  double dummy;
  for (unsigned i=0; i<n; ++i) { // imag block
    dummy = std::imag(data[i]);
    VNL::WriteBytes(s, &dummy, sizeof(dummy));
  }
}

}; // End namespace VNL



//--------------------------------------------------------------------------------

/** scalars.
*/
template <class T>
bool VNL::MatlabWrite(std::ostream &s, T const & x, char const *name) {
  MatlabHeader hdr;
  hdr.type = native_BYTE_ORDER + MatlabHeader::VNL_COLUMN_WISE + ScalarPrecision(x);
  hdr.rows = 1;
  hdr.cols = 1;
  hdr.imag = IsComplex(x);
  hdr.namlen = (unsigned long)strlen(name)+1L;

  VNL::WriteBytes(s, &hdr, sizeof(hdr));
  VNL::WriteBytes(s, name, hdr.namlen);
  WriteReal(s, &x, 1);
  WriteImag(s, &x, 1);

  return s.good() != 0;
}
#define scalar_instantiate(T) \
namespace VNL { \
template bool MatlabWrite(std::ostream &, T const &, char const *); \
};

/** 1D array.
*/
template <class T>
bool VNL::MatlabWrite(std::ostream &s, T const *v, unsigned n, char const *name) {
  MatlabHeader hdr;
  hdr.type = native_BYTE_ORDER + MatlabHeader::VNL_COLUMN_WISE + ScalarPrecision(v[0]);
  hdr.rows = (long)n;
  hdr.cols = 1L;
  hdr.imag = IsComplex(v[0]);
  hdr.namlen = (unsigned long)strlen(name)+1L;

  VNL::WriteBytes(s, &hdr, sizeof(hdr));
  VNL::WriteBytes(s, name, hdr.namlen);
  WriteReal(s, v, n);
  WriteImag(s, v, n);

  return s.good() != 0;
}
#define array1D_instantiate(T) \
namespace VNL { \
  template bool MatlabWrite(std::ostream &, T const *, unsigned, char const *); \
};

/** 2D array.
*/
template <class T>
bool VNL::MatlabWrite(std::ostream &s,
                      T const * const *data,
                      unsigned rows, unsigned cols,
                      char const *name)
{
  MatlabHeader hdr;
  hdr.type = native_BYTE_ORDER + MatlabHeader::VNL_ROW_WISE + ScalarPrecision(data[0][0]);
  hdr.rows = (long)rows;
  hdr.cols = (long)cols;
  hdr.imag = IsComplex(data[0][0]);
  hdr.namlen = (unsigned long)strlen(name)+1L;

  VNL::WriteBytes(s, &hdr, sizeof(hdr));
  VNL::WriteBytes(s, name, hdr.namlen);
  for (unsigned i=0; i<rows; ++i)
    WriteReal(s, data[i], cols);
  for (unsigned j=0; j<rows; ++j)
    WriteImag(s, data[j], cols);

  return s.good() != 0;
}
#define array2D_instantiate(T) \
namespace VNL { \
  template bool MatlabWrite(std::ostream &, T const * const *, unsigned, unsigned, char const *); \
};

//--------------------------------------------------------------------------------

scalar_instantiate(float);
scalar_instantiate(double);
scalar_instantiate(std::complex<float>);
scalar_instantiate(std::complex<double>);

array1D_instantiate(float);
array1D_instantiate(double);
array1D_instantiate(std::complex<float>);
array1D_instantiate(std::complex<double>);

array2D_instantiate(float);
array2D_instantiate(double);
array2D_instantiate(std::complex<float>);
array2D_instantiate(std::complex<double>);
