// This is ./vxl/VNL/matlab_print_format.cxx

/**
* \file
*/

#include "matlabprint.h"

#include <iostream>
#include <vector>

/** Choose precision in printouts.
*
* vnl_matlab_format(vnl_matops::fmt_long) selects 16-digit precision
*
* vnl_matlab_format(vnl_matops::fmt_short) selects 4-digit precision
*/

/** this variable is the current top of the stack.
* moved here because gcc 2.7 choked
*/
static VNL::MatlabPrintFormat the_format = VNL::matlab_print_format_short;
/** the rest of the stack is stored in this vector.
*/
static std::vector<int> *format_stack = 0;
/** call this to initialize the format stack.
*/

namespace VNL { // This isn't in the header file, so must be added to namespace
static void MatlabPrintFormatInit()
{ if (!format_stack) format_stack = new std::vector<int>; }
};

void VNL::MatlabPrintFormatPush(VNL::MatlabPrintFormat f)
{
  VNL::MatlabPrintFormatInit();
  format_stack->push_back(the_format);
  the_format = f;
}

void VNL::MatlabPrintFormatPop()
{
  VNL::MatlabPrintFormatInit();
  if (format_stack->empty())
    std::cerr << __FILE__ ": format stack empty" << std::endl;
  else {
    the_format = VNL::MatlabPrintFormat(format_stack->back());
    format_stack->pop_back();
  }
}

VNL::MatlabPrintFormat VNL::MatlabPrintFormatSet(VNL::MatlabPrintFormat f)
{
  VNL::MatlabPrintFormatInit();
  VNL::MatlabPrintFormat old = the_format;
  the_format = f;
  return old;
}

VNL::MatlabPrintFormat VNL::MatlabPrintFormatTop()
{
  return the_format;
}
