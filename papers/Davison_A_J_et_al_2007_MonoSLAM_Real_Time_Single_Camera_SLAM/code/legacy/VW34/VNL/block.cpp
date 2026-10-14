// This is vxl/VNL/block.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author fsm@robots.ox.ac.uk
*/

#include "block.h"

#include <assert.h>
#include <iostream>

void vnl_block_raise_exception(char const *FILE, int LINE, char const *why)
{
  std::cerr << FILE << ":" << LINE << ": " << why << std::endl;
  assert(!"raise_exeption() called");
  // throw;
}
