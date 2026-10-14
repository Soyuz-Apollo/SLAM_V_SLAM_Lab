// This is vxl/VNL/matlab_read.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author fsm@robots.ox.ac.uk
*/
#include <VNL/VC6WarningsFix.h>
#include "matlabread.h"

// Although technically correct, <ios> doesn't always exist, and where it does, gets 
// included by <iostream> anyway.
//#include <ios> // for std::ios::cur

#include <iostream>
#include <cstdlib> // abort()
#include <cstring> // memset()
#include <complex>
#include <VNL/cvector.h>

// FIXME: Currently ignores the byte ordering of the MAT file header, effectively
// assuming the MAT file was written with the native byte ordering.

//--------------------------------------------------------------------------------

// SGI needs??
namespace VNL {

void ReadBytes(std::istream &s, void *p, unsigned bytes) {
  s.read((char *)p, bytes);
}

template <>
void MatlabReadData(std::istream &s, float *p, unsigned n)
{ VNL::ReadBytes(s, p, n*sizeof(*p)); }

template <>
void MatlabReadData(std::istream &s, double *p, unsigned n)
{ VNL::ReadBytes(s, p, n*sizeof(*p)); }

#define implement_read_complex_data(T) \
template <> \
void MatlabReadData(std::istream &s, std::complex<T > *ptr, unsigned n) { \
  T *re = VNL::CVector<T >::AllocateT(n); \
  T *im = VNL::CVector<T >::AllocateT(n); \
  VNL::ReadBytes(s, re, n*sizeof(T)); \
  VNL::ReadBytes(s, im, n*sizeof(T)); \
  for (unsigned i=0; i<n; ++i) \
    ptr[i] = std::complex<T >(re[i], im[i]); \
  VNL::CVector<T >::Deallocate(re, n); \
  VNL::CVector<T >::Deallocate(im, n); \
}

implement_read_complex_data(float )
implement_read_complex_data(double)

#undef implement_read_complex_data

  }; // End namespace VNL

//--------------------------------------------------------------------------------

VNL::MatlabReadHdr::MatlabReadHdr(std::istream &s_)
  : s(s_), varname(0), data_read(false) 
{
  _ReadHdr();
}

VNL::MatlabReadHdr::~MatlabReadHdr() 
{
  if (varname)
    delete [] varname;
  varname = 0;
}

VNL::MatlabReadHdr::operator bool () const 
{
  return s.good() && !s.eof(); // FIXME
}

bool VNL::MatlabReadHdr::IsSingle() const 
{
  return (hdr.type % (10*MatlabHeader::VNL_SINGLE_PRECISION)) >= MatlabHeader::VNL_SINGLE_PRECISION;
}

bool VNL::MatlabReadHdr::IsRowwise() const 
{
  return (hdr.type % (10*MatlabHeader::VNL_ROW_WISE)) >= MatlabHeader::VNL_ROW_WISE;
}

bool VNL::MatlabReadHdr::IsBigendian() const 
{
  return (hdr.type % (10*MatlabHeader::VNL_BIG_ENDIAN)) >= MatlabHeader::VNL_BIG_ENDIAN;
}

/** internal.
* increment 'current', record the file position and read the header.
*/
void VNL::MatlabReadHdr::_ReadHdr() 
{
  memset(&hdr, 0, sizeof hdr);
  VNL::ReadBytes(s, &hdr, sizeof(hdr));
  if (varname)
    delete [] varname;
  varname = new char[hdr.namlen+1];
#ifdef DEBUG
  std::cerr << "type:" << hdr.type << std::endl;
  std::cerr << "rows:" << hdr.rows << std::endl;
  std::cerr << "cols:" << hdr.cols << std::endl;
  std::cerr << "imag:" << hdr.imag << std::endl;
  std::cerr << "namlen:" << hdr.namlen << std::endl;
#endif
  VNL::ReadBytes(s, varname, hdr.namlen);
  varname[hdr.namlen] = '\0';

  data_read = false;
}

void VNL::MatlabReadHdr::ReadNext() {
  if (!data_read) {
    // number of bytes to skip :
    unsigned long n = Rows()*Cols()*sizeof(float);

    if (!IsSingle())
      n *= 2;

    if (IsComplex())
      n *= 2;
    s.seekg(n, std::ios::cur);
  }

  _ReadHdr();
}

//--------------------------------------------------------------------------------

bool VNL::MatlabReadHdr::TypeChck(float &) { return IsSingle() && !IsComplex(); }
bool VNL::MatlabReadHdr::TypeChck(double &) { return !IsSingle() && !IsComplex(); }
bool VNL::MatlabReadHdr::TypeChck(std::complex<float> &) { return IsSingle() && IsComplex(); }
bool VNL::MatlabReadHdr::TypeChck(std::complex<double> &) { return !IsSingle() && IsComplex(); }

#define fsm_define_methods(T) \
bool VNL::MatlabReadHdr::ReadData(T &v) { \
  if (!TypeChck(v)) { std::cerr << "type_check\n"; return false; }\
  if (Rows()!=1 || Cols()!=1) { std::cerr << "size0\n"; return false; } \
  MatlabReadData(s, &v, 1); \
  data_read = true; return *this; \
} \
bool VNL::MatlabReadHdr::ReadData(T *p) { \
  if (!TypeChck(p[0])) { std::cerr << "type_check\n"; return false; } \
  if (Rows()!=1 && Cols()!=1) { std::cerr << "size1\n"; return false; } \
  MatlabReadData(s, p, Rows()*Cols()); \
  data_read = true; return *this; \
} \
bool VNL::MatlabReadHdr::ReadData(T * const *m) { \
  if (!TypeChck(m[0][0])) { std::cerr << "type_check\n"; return false; } \
  T *tmp = VNL::CVector<T >::AllocateT(Rows()*Cols()); \
  /*VNL::CVector<T >::fill(tmp, rows()*cols(), 3.14159);*/ \
  MatlabReadData(s, tmp, Rows()*Cols()); \
  int a, b; \
  if (IsRowwise()) { \
    a = Cols(); \
    b = 1; \
  } \
  else { \
    a = 1; \
    b = Rows(); \
  } \
  for (int i=0; i<Rows(); ++i) \
    for (int j=0; j<Cols(); ++j) \
      m[i][j] = tmp[a*i + b*j]; \
  VNL::CVector<T >::Deallocate(tmp, Rows()*Cols()); \
  data_read = true; return *this; \
}
fsm_define_methods(float);
fsm_define_methods(double);
//fsm_define_methods(std::complex<float>);
fsm_define_methods(std::complex<double>);
#undef fsm_define_methods

//--------------------------------------------------------------------------------

#include <assert.h>
#include <new>
#include <algorithm>
#include <VNL/vector.h>
#include <VNL/matrix.h>

template <class T>
bool VNL::MatlabReadOrDie(std::istream &s,
			  VNL::Vector<T> &v,
			  char const *name)
{
  MatlabReadHdr h(s);
  if (!s) // eof?
    return false;
  if (name && *name)
    assert(strcmp(name, h.Name())==0/*wrong name?*/);
  if (v.size() != unsigned(h.Rows()*h.Cols()))
  {
	  v.Resize( h.Rows()*h.Cols() );
  }
  assert(h.ReadData(v.begin())/*wrong type?*/);
  return true;
}

template <class T>
bool VNL::MatlabReadOrDie(std::istream &s,
                            VNL::Matrix<T> &M,
                            char const *name)
{
  MatlabReadHdr h(s);
  if (!s) // eof?
    return false;
  if (name && *name)
    assert(strcmp(name, h.Name())==0/*wrong name?*/);
  if (M.Rows() != unsigned(h.Rows()) || M.Cols() != unsigned(h.Cols()))
  {
    delete(&M);
    new (&M) VNL::Matrix<T>(h.Rows(), h.Cols());
  }
  assert(h.ReadData(M.DataArray())/*wrong type?*/);
  return true;
}

#define inst(T) \
namespace VNL { \
template bool MatlabReadOrDie(std::istream &, Vector<T> &, char const *); \
template bool MatlabReadOrDie(std::istream &, Matrix<T> &, char const *); \
};

inst(double);
inst(float);
