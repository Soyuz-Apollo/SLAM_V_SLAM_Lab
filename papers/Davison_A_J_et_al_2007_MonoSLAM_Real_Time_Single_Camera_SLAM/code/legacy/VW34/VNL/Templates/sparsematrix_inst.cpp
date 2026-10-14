#include <VNL/VC6WarningsFix.h>
#include <VNL/sparsematrix.tpp>
#include <VNL/rational.h>
#include <VNL/bignum.h>

namespace VNL {
  template class SparseMatrix<float>;
  template class SparseMatrix<double>;
  template class SparseMatrix<VNL::Rational>;
  template class SparseMatrix<VNL::BigNum>;
}

static void vnl_sparse_matrix_float_tickler()
{
  VNL::SparseMatrix<float> md(3, 5);
  vnl_sparse_matrix_float_tickler(); // to avoid compiler warning
}

static void vnl_sparse_matrix_double_tickler()
{
  VNL::SparseMatrix<double> md(3, 5);
  vnl_sparse_matrix_double_tickler(); // to avoid compiler warning
}

static void vnl_sparse_matrix_vnl_rational_tickler()
{
  VNL::SparseMatrix<VNL::Rational> md(3, 5);
  vnl_sparse_matrix_vnl_rational_tickler(); // to avoid compiler warning
}

static void vnl_sparse_matrix_vnl_bignum_tickler()
{
  VNL::SparseMatrix<VNL::BigNum> md(3, 5);
  vnl_sparse_matrix_vnl_bignum_tickler(); // to avoid compiler warning
}
