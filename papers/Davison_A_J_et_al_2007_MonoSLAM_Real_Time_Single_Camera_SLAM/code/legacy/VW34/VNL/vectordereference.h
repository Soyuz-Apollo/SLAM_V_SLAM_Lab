// This is vxl/VNL/vector_dereference.h
#ifndef vnl_vector_dereference_h_
#define vnl_vector_dereference_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
/**
*  \file
*  \author Andrew W. Fitzgibbon, Oxford RRG
*  \date   28 Nov 98
*/


#include <vector>

template <class T>
class vnl_vector_dereference : public vnl_unary_function<T, int>
{
  std::vector<T> v_;
 public:

  vnl_vector_dereference(const std::vector<T>& v):
    v_(v) {
  }

  T f(const int& i) {
    return v_[i];
  }
};

#endif // vnl_vector_dereference_h_
