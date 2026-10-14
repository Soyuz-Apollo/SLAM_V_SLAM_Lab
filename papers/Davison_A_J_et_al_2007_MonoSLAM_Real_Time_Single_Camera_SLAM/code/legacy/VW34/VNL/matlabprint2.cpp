// This is vxl/VNL/matlab_print2.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author fsm@robots.ox.ac.uk
*/
#include <VNL/VC6WarningsFix.h>
#include "matlabprint2.h"
#include <complex>

VNL_MATLAB_PRINT2_INSTANTIATE(VNL::Vector<float>);
VNL_MATLAB_PRINT2_INSTANTIATE(VNL::Vector<double>);
VNL_MATLAB_PRINT2_INSTANTIATE(VNL::Vector<std::complex<double> >);

VNL_MATLAB_PRINT2_INSTANTIATE(VNL::Matrix<float>);
VNL_MATLAB_PRINT2_INSTANTIATE(VNL::Matrix<double>);
VNL_MATLAB_PRINT2_INSTANTIATE(VNL::Matrix<std::complex<double> >);
