// This is vxl/VNL/matlab_filewrite.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
*/
#include <VNL/VC6WarningsFix.h>
#include "matlabfilewrite.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>

#include <VNL/vector.h>
#include <VNL/matrix.h>
#include <VNL/matlabwrite.h>

VNL::MatlabFilewrite::MatlabFilewrite(char const *file_name,
				      char const *basename)
  : basename_(basename ? basename : "targetvar"), variable_int_(0)
{
  out_.open(file_name, std::ios::out | std::ios::binary);
  if (out_.bad())
    std::cerr << __FILE__ ":" << __LINE__ << ", WARNING : output stream is bad\n";
}

std::string VNL::MatlabFilewrite::_MakeVarName(char const* variable_name)
{
  if (variable_name)
    return std::string(variable_name);
  else {
    std::stringstream ss;
    ss << variable_int_++;
    return basename_ + ss.str();
  }
}

//--------------------------------------------------------------------------------

/** scalar.
*/
void VNL::MatlabFilewrite::Write(double v, char const* variable_name) {
  VNL::MatlabWrite(out_, v, _MakeVarName(variable_name).c_str());
}

/** vector.
*/
void VNL::MatlabFilewrite::Write(VNL::Vector<double> const& v, char const* variable_name) {
  VNL::MatlabWrite(out_, v.DataBlock(), v.size(), _MakeVarName(variable_name).c_str());
}

void VNL::MatlabFilewrite::Write(VNL::Vector<std::complex<double> > const& v, char const* variable_name) {
  VNL::MatlabWrite(out_, v.DataBlock(), v.size(), _MakeVarName(variable_name).c_str());
}

/** matrix.
*/
void VNL::MatlabFilewrite::Write(VNL::Matrix<float> const& M, char const* variable_name) {
  VNL::MatlabWrite(out_, M.DataArray(), M.Rows(), M.Cols(), _MakeVarName(variable_name).c_str());
}

void VNL::MatlabFilewrite::Write(VNL::Matrix<double> const& M, char const* variable_name) {
  VNL::MatlabWrite(out_, M.DataArray(), M.Rows(), M.Cols(), _MakeVarName(variable_name).c_str());
}

void VNL::MatlabFilewrite::Write(VNL::Matrix<std::complex<float> > const& M, char const* variable_name) {
  VNL::MatlabWrite(out_, M.DataArray(), M.Rows(), M.Cols(), _MakeVarName(variable_name).c_str());
}

void VNL::MatlabFilewrite::Write(VNL::Matrix<std::complex<double> > const& M, char const* variable_name) {
  VNL::MatlabWrite(out_, M.DataArray(), M.Rows(), M.Cols(), _MakeVarName(variable_name).c_str());
}

void VNL::MatlabFilewrite::Write(double const * const *M, int rows, int cols, char const* variable_name) {
  VNL::MatlabWrite(out_, M, rows, cols, _MakeVarName(variable_name).c_str());
}
