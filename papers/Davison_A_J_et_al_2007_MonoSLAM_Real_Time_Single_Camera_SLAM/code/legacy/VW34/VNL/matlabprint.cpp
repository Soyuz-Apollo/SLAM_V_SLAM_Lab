// This is vxl/VNL/matlab_print.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author fsm@robots.ox.ac.uk
*
* Adapted from awf's MatOps class.
*/

#include "matlabprint.h"
#include <iostream>
#include <VNL/matrix.h>

/** Can be used within debugger to print matrix.
*/
extern "C"
void vnl_dbprintmx(VNL::Matrix<double> const& p)
{
  // why the cast? is it a const_cast?
  VNL::MatlabPrint(std::cerr, 
		   p, 
		   (char const*)"M", 
		   VNL::matlab_print_format_default);
}
