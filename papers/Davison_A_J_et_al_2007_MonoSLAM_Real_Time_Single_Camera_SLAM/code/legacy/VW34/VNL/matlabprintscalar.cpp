// This is vxl/VNL/matlab_print_scalar.cxx

#ifdef _WIN32
#pragma warning(disable:4275) // disable C4275 warning
#endif

#include "matlabprintscalar.h"

#include <cstdio>  // sprintf()
#include <cstdlib> // abort()
#include <cstring> // strlen()
#include <complex>

void VNL::MatlabPrintScalar(int v,
                             char *buf,
                             VNL::MatlabPrintFormat)
{
  sprintf(buf, "%4d ", v);
}

void VNL::MatlabPrintScalar(unsigned v,
                             char *buf,
                             VNL::MatlabPrintFormat)
{
  sprintf(buf, "%4u ", v);
}

void VNL::MatlabPrintScalar(float v,
                             char *buf,
                             VNL::MatlabPrintFormat format)
{
  if (format == VNL::matlab_print_format_default)
    format = VNL::MatlabPrintFormatTop();
  switch (format) {
  case VNL::matlab_print_format_long:
    if (v == 0.0)
      sprintf(buf, "%8d ", 0);
    else
      sprintf(buf, "%8.5f ", v);
    break;
  case VNL::matlab_print_format_short:
    if (v == 0.0)
      sprintf(buf, "%6d ", 0);
    else
      sprintf(buf, "%6.3f ", v);
    break;
  case VNL::matlab_print_format_long_e:
    sprintf(buf, "%11.7e ", v);
    break;
  case VNL::matlab_print_format_short_e:
    sprintf(buf, "%8.4e ", v);
    break;
  default:/*vnl_matlab_print_format_default:*/ abort(); break;
  }
}

void VNL::MatlabPrintScalar(double v,
                             char *buf,
                             VNL::MatlabPrintFormat format)
{
  if (format == VNL::matlab_print_format_default)
    format = VNL::MatlabPrintFormatTop();
  switch (format) {
  case VNL::matlab_print_format_long:
    if (v == 0.0)
      sprintf(buf, "%16d ", 0);
    else
      sprintf(buf, "%16.13f ", v);
    break;
  case VNL::matlab_print_format_short:
    if (v == 0.0)
      sprintf(buf, "%8d ", 0);
    else
      sprintf(buf, "%8.4f ", v);
    break;
  case VNL::matlab_print_format_long_e:
    sprintf(buf, "%20.14e ", v);
    break;
  case VNL::matlab_print_format_short_e:
    sprintf(buf, "%10.4e ", v);
    break;
  default:/*vnl_matlab_print_format_default:*/ abort(); break;
  }
}

void VNL::MatlabPrintScalar(long double v,
                             char *buf,
                             VNL::MatlabPrintFormat format)
{
  VNL::MatlabPrintScalar(double(v), buf, format); // FIXME
}

void VNL::MatlabPrintScalar(std::complex<double> v,
                             char *buf,
                             VNL::MatlabPrintFormat format)
{
  if (format == VNL::matlab_print_format_default)
    format = VNL::MatlabPrintFormatTop();
  int width = 16;
  int precision = 12;
  char conv = 'f';

  switch (format) {
  case VNL::matlab_print_format_long:
  case VNL::matlab_print_format_long_e:
    width = 16;
    precision = 12;
    break;
  case VNL::matlab_print_format_short:
  case VNL::matlab_print_format_short_e:
    width = 8;
    precision = 4;
    break;
  default:/*vnl_matlab_print_format_default:*/ abort(); break;
  }

  switch (format) {
  case VNL::matlab_print_format_long:
  case VNL::matlab_print_format_short:
    conv = 'f';
    break;
  case VNL::matlab_print_format_long_e:
  case VNL::matlab_print_format_short_e:
    conv = 'e';
    break;
  default:/*vnl_matlab_print_format_default:*/ abort(); break;
  }

  double r = std::real(v);
  double i = std::imag(v);

  char fmt[1024];
  // Real part
  if (r == 0) {
    sprintf(fmt, "%%" "%d" "d ", width);
    sprintf(buf, fmt, 0);

  } else {
    sprintf(fmt, "%%" "%d" "." "%d" "%c ", width, precision, conv);
    sprintf(buf, fmt, r);
  }

  buf += strlen(buf);

  // Imaginary part.  Width is reduced as sign is taken care of separately
  if (i == 0) {
    sprintf(fmt, " %%" "%d" "s  ", width-1);
    sprintf(buf, fmt, "");
  } else {
    char sign = '+';
    if (i < 0) {
      sign = '-';
      i = -i;
    }
    sprintf(fmt, "%c%%" "%d.%d%ci ", sign, width-1, precision, conv);
    sprintf(buf, fmt, i);
  }
}

void VNL::MatlabPrintScalar(std::complex<float> v,
                             char *buf,
                             VNL::MatlabPrintFormat format)
{
  if (format == VNL::matlab_print_format_default)
    format = VNL::MatlabPrintFormatTop();
  int width = 10;
  int precision = 6;
  char conv = 'f';

  switch (format) {
  case VNL::matlab_print_format_long:
  case VNL::matlab_print_format_long_e:
    width = 10;
    precision = 6;
    break;
  case VNL::matlab_print_format_short:
  case VNL::matlab_print_format_short_e:
    width = 8;
    precision = 4;
    break;
  default:/*vnl_matlab_print_format_default:*/ abort(); break;
  }

  switch (format) {
  case VNL::matlab_print_format_long:
  case VNL::matlab_print_format_short:
    conv = 'f';
    break;
  case VNL::matlab_print_format_long_e:
  case VNL::matlab_print_format_short_e:
    conv = 'e';
    break;
  default:/*vnl_matlab_print_format_default:*/ abort(); break;
  }

  float r = std::real(v);
  float i = std::imag(v);

  char fmt[1024];
  // Real part
  if (r == 0) {
    sprintf(fmt, "%%" "%d" "d ", width);
    sprintf(buf, fmt, 0);

  } else {
    sprintf(fmt, "%%" "%d" "." "%d" "%c ", width, precision, conv);
    sprintf(buf, fmt, r);
  }

  buf += strlen(buf);

  // Imaginary part.  Width is reduced as sign is taken care of separately
  if (i == 0) {
    sprintf(fmt, " %%" "%d" "s  ", width-1);
    sprintf(buf, fmt, "");
  } else {
    char sign = '+';
    if (i < 0) {
      sign = '-';
      i = -i;
    }
    sprintf(fmt, "%c%%" "%d.%d%ci ", sign, width-1, precision, conv);
    sprintf(buf, fmt, i);
  }
}

void VNL::MatlabPrintScalar(std::complex<long double> v,
                             char *buf,
                             VNL::MatlabPrintFormat format)
{
  VNL::MatlabPrintScalar(std::complex<double>(std::real(v), std::imag(v)), buf, format); // FIXME
}


#include <iostream>
template <class T>
std::ostream &VNL::MatlabPrintScalar(std::ostream &s,
                                     T value,
                                     VNL::MatlabPrintFormat format)
{
  char buf[1024];
  VNL::MatlabPrintScalar(value, buf, format);
  return s << buf;
}

#define INST(T) \
namespace VNL { \
  template std::ostream &MatlabPrintScalar(std::ostream &, T, VNL::MatlabPrintFormat); \
}

INST(int);
INST(float);
INST(double);
INST(long double);
INST(std::complex<float>);
INST(std::complex<double>);
INST(std::complex<long double>);

