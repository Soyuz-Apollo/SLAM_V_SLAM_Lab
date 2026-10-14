// This is vxl/VNL/matops.cxx
#ifdef _WIN32
#pragma warning(disable:4275) // disable C4275 warning
#endif

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author Andrew W. Fitzgibbon, Oxford RRG
* \date   05 Aug 96
*
*/

#include "matops.h"
#include <assert.h>

VNL::Matrix<double> vnl_matops::cat(VNL::Matrix<double> const &A, VNL::Matrix<double> const &B) {
  int rowsA = A.Rows();
  int colsA = A.Columns();
  int rowsB = B.Rows();
  int colsB = B.Columns();

  assert(rowsA == rowsB);

  VNL::Matrix<double> M(rowsA,colsA+colsB);
  M.Update(A,0,0);
  M.Update(B,0,colsA);

  return M;
}

VNL::Matrix<double> vnl_matops::cat(VNL::Matrix<double> const &A, VNL::Vector<double> const &B) {
  int rowsA = A.Rows();
  int colsA = A.Columns();
  int rowsB = B.Size();

  assert(rowsA == rowsB);

  VNL::Matrix<double> M(rowsA,colsA+1);
  M.Update(A,0,0);
  M.SetColumn(colsA,B);

  return M;
}

VNL::Matrix<double> vnl_matops::cat(VNL::Vector<double> const &A, VNL::Matrix<double> const &B) {
  int rowsA = A.Size();
  int rowsB = B.Rows();
  int colsB = B.Columns();

  assert(rowsA == rowsB);

  VNL::Matrix<double> M(rowsA,colsB+1);
  M.SetColumn(0,A);
  M.Update(B,0,1);

  return M;
}

VNL::Matrix<double> vnl_matops::vcat(VNL::Matrix<double> const &A, VNL::Matrix<double> const &B) {
  int rowsA = A.Rows();
  int colsA = A.Columns();
  int rowsB = B.Rows();
  int colsB = B.Columns();

  assert(colsA == colsB);

  VNL::Matrix<double> M(rowsA+rowsB,colsA);
  M.Update(A,0,0);
  M.Update(B,rowsA,0);

  return M;
}

extern "C" int dtrans_(double *a, const int& m, const int& n, const int& mn, int* move, const int& iwrk, int* iok);

/** Return fro_norm( (A ./ B) - mean(A ./ B) ).
*/
double vnl_matops::homg_diff(VNL::Matrix<double> const& A, VNL::Matrix<double> const& B)
{
	VNL::Matrix<double> ratio = VNL::ElementQuotient(A, B);

  return (ratio - ratio.Mean()).FroNorm();
}

#define implement_converters(U,V)                           \
VNL::Matrix<U> make_matrix_ ## U(VNL::Matrix<V> const& M)     \
{                                                           \
  unsigned m = M.Rows();                                    \
  unsigned n = M.Columns();                                 \
  VNL::Matrix<U> ret(m, n);                                  \
  for (unsigned i = 0; i < m; ++i)                           \
    for (unsigned j = 0; j < n; ++j)                         \
      ret(i,j) = U(M(i,j));                                 \
  return ret;                                               \
}                                                           \
                                                            \
VNL::Vector<U> make_vector_ ## U(VNL::Vector<V> const& v)     \
{                                                           \
  unsigned n = v.size();                                    \
  VNL::Vector<U> ret(n);                                     \
  for (unsigned i = 0; i < n; ++i)                           \
    ret[i] = U(v[i]);                                       \
  return ret;                                               \
}                                                           \

implement_converters(double,float)

implement_converters(float,double)

VNL::Matrix<double>  vnl_matops::f2d(VNL::Matrix<float> const& M)
{
  return make_matrix_double(M);
}


VNL::Matrix<float>  vnl_matops::d2f(VNL::Matrix<double> const& M)
{
  return make_matrix_float(M);
}

VNL::Vector<double>  vnl_matops::f2d(VNL::Vector<float> const& M)
{
  return make_vector_double(M);
}


VNL::Vector<float>  vnl_matops::d2f(VNL::Vector<double> const& M)
{
  return make_vector_float(M);
}
