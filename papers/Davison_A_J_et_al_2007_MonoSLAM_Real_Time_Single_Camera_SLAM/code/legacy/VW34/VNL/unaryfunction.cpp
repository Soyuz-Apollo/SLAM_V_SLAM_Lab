// This is vxl/VNL/unary_function.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// UnaryFunction
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 28 Nov 98
//
//-----------------------------------------------------------------------------
#include <VNL/VC6WarningsFix.h>
#include "unaryfunction.h"
#include "identity.h"
#include <VNL/vector.h>


namespace VNL {

  template class UnaryFunction<int,int>;
  template class Identity<int>;
  template class UnaryFunction<double, Vector<double> >;


}; // End namespace VNL
