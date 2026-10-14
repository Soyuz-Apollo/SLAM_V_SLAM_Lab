#include <VNL/VC6WarningsFix.h>
#include <VNL/matrixfixed.tpp>
#include <VNL/rational.h>
#include <VNL/bignum.h>

namespace VNL {
  // Double
  template class MatrixFixed<2,2,double>;
  template class MatrixFixedRef<2,2,double>;

  template class MatrixFixed<2,3,double>;
  template class MatrixFixedRef<2,3,double>;

  template class MatrixFixed<2,6,double>;
  template class MatrixFixedRef<2,6,double>;

  template class MatrixFixed<3,12,double>;
  template class MatrixFixedRef<3,12,double>;

  template class MatrixFixed<3,3,double>;
  template class MatrixFixedRef<3,3,double>;

  template class MatrixFixed<3,4,double>;
  template class MatrixFixedRef<3,4,double>;

  template class MatrixFixed<4,3,double>;
  template class MatrixFixedRef<4,3,double>;

  template class MatrixFixed<4,4,double>;
  template class MatrixFixedRef<4,4,double>;

  // Float
  template class MatrixFixed<3,3,float>;
  template class MatrixFixedRef<3,3,float>;

  // Bignum
  template class MatrixFixed<3,3,VNL::BigNum>;
  template class MatrixFixedRef<3,3,VNL::BigNum>;
  
  // Rational
  template class MatrixFixed<3,3,VNL::Rational>;
  template class MatrixFixedRef<3,3,VNL::Rational>;

};



// instantiate a few others (square versions are hard-coded in .h)
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(2,3,2,double);
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(2,3,3,double);
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(3,3,2,double);
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(3,3,4,double);
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(3,4,4,double);


