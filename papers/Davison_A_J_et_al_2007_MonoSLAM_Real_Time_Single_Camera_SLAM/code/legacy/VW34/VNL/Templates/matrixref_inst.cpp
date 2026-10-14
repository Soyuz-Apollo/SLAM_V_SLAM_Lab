#include <VNL/matrix.tpp>
#include <VNL/matrixref.h>
#include <VNL/bignum.h>
#include <VNL/rational.h>
#include <VNL/complex.h>



// Now the reference class
namespace VNL {
  template class MatrixRef<double>;
  template class MatrixRef<float>;
  template class MatrixRef<int>;
  template class MatrixRef<long>;
  template class MatrixRef<long double>;
  template class MatrixRef<signed char>;
  template class MatrixRef<unsigned char>;
  template class MatrixRef<std::complex<double> >;
  template class MatrixRef<unsigned int>;
  template class MatrixRef<unsigned long>;
  
  template class MatrixRef<VNL::BigNum>;
  template class MatrixRef<VNL::Rational>;
}
