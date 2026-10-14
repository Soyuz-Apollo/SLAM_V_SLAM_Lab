// This is vxl/VNL/fastops.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author Andrew W. Fitzgibbon, Oxford RRG
* \date   08 Dec 96
*
*/

#include "fastops.h"

#include <cstdlib> // abort()
#include <cstring> // memset()
#include <iostream>

/** Compute $A^\top A$.
*/
void vnl_fastops::AtA(const VNL::Matrix<double>& A, VNL::Matrix<double>* AtA)
{
  unsigned n = A.Columns();
  // Verify output is the right size
  if (AtA->Rows() != n || AtA->Columns() != n) {
    std::cerr << "fastops::AtA: " << AtA->Rows() << "x" << AtA->Columns() << " is not " << n << "x" << n << std::endl;
    abort();
  }

  int m = A.Rows();

  double const* const* a = A.DataArray();
  double** ata = AtA->DataArray();

  if (0) {
    for (unsigned i = 0; i < n; ++i)
      for (unsigned j = i; j < n; ++j) {
        double accum = 0;
        for (int k = 0; k < m; ++k)
          accum += a[k][i] * a[k][j];
        ata[i][j] = ata[j][i] = accum;
      }
  } else {
    // 5 times faster on 600 Mhz Pentium III for m = 10000, n = 50
    memset(ata[0], 0, n * n * sizeof ata[0][0]);
    for (int k = 0; k < m; ++k)
      for (unsigned i = 0; i < n; ++i) {
        double aki = a[k][i];
        double const* arow = a[k] + i;
        double* atarow = ata[i] + i;
        double const* arowend = a[k] + n;
        while (arow != arowend)
          *atarow++ += aki * *arow++;
      }
      for (unsigned i = 0; i < n; ++i)
        for (unsigned j = i+1; j < n; ++j)
          ata[j][i] = ata[i][j];
  }
}

/** Compute AxB.
*/
void vnl_fastops::AB(const VNL::Matrix<double>& A, const VNL::Matrix<double>& B, VNL::Matrix<double>* out_ptr)
{
  unsigned ma = A.Rows();
  unsigned na = A.Columns();
  unsigned mb = B.Rows();
  unsigned nb = B.Columns();

  // Verify matrices compatible
  if (na != mb) {
    std::cerr << "fastops::AB: " << na << " != " << mb << std::endl;
    abort();
  }

  // Verify output is the right size
  if (out_ptr->Rows() != ma || out_ptr->Columns() != nb) {
    std::cerr << "fastops::AB: " << out_ptr->Rows() << "x" << out_ptr->Columns() << " is not " << ma << "x" << nb << std::endl;
    abort();
  }

  double const* const* a = A.DataArray();
  double const* const* b = B.DataArray();
  double** out = out_ptr->DataArray();

  for (unsigned i = 0; i < ma; ++i)
    for (unsigned j = 0; j < nb; ++j) {
      double accum = 0;
      for (unsigned k = 0; k < na; ++k)
        accum += a[i][k] * b[k][j];
      out[i][j] = accum;
    }
}

/**  Compute $A^\top B$.
*/
void vnl_fastops::AtB(const VNL::Matrix<double>& A, const VNL::Matrix<double>& B, VNL::Matrix<double>* out_ptr)
{
  unsigned ma = A.Rows();
  unsigned na = A.Columns();
  unsigned mb = B.Rows();
  unsigned nb = B.Columns();

  // Verify matrices compatible
  if (ma != mb) {
    std::cerr << "fastops::AtA: " << ma << " != " << mb << std::endl;
    abort();
  }

  // Verify output is the right size
  if (out_ptr->Rows() != na || out_ptr->Columns() != nb) {
    std::cerr << "fastops::AtA: " << out_ptr->Rows() << "x" << out_ptr->Columns() << " is not " << na << "x" << nb << std::endl;
    abort();
  }

  double const* const* a = A.DataArray();
  double const* const* b = B.DataArray();
  double** out = out_ptr->DataArray();

  for (unsigned i = 0; i < na; ++i)
    for (unsigned j = 0; j < nb; ++j) {
      double accum = 0;
      for (unsigned k = 0; k < ma; ++k)
        accum += a[k][i] * b[k][j];
      out[i][j] = accum;
    }
}

/** Compute $A^\top b$ for vector b.\ out_ptr may not be b.
*/
void vnl_fastops::AtB(const VNL::Matrix<double>& A, const VNL::Vector<double>& B, VNL::Vector<double>* out_ptr)
{
  unsigned m = A.Rows();
  unsigned n = A.Columns();
  unsigned l = B.Size();

  // Verify matrices compatible
  if (m != l) {
    std::cerr << "fastops::AtB: " << m << " != " << l << std::endl;
    abort();
  }

  // Verify output is the right size
  if (out_ptr->Size() != n) {
    std::cerr << "fastops::AtA: Output vector wrong size.  Is " << out_ptr->size() << ", should be " << n << std::endl;
    abort();
  }

  double const* const* a = A.DataArray();
  double const* b = B.DataBlock();
  double* out = out_ptr->DataBlock();

  for (unsigned i = 0; i < n; ++i) {
    double accum = 0;
    for (unsigned k = 0; k < l; ++k)
      accum += a[k][i] * b[k];
   out[i] = accum;
  }
}

/** Compute $A B^\top$.
*/
void vnl_fastops::ABt(const VNL::Matrix<double>& A, const VNL::Matrix<double>& B, VNL::Matrix<double>* out_ptr)
{
  unsigned ma = A.Rows();
  unsigned na = A.Columns();
  unsigned mb = B.Rows();
  unsigned nb = B.Columns();

  // Verify matrices compatible
  if (na != nb) {
    std::cerr << "fastops::ABt: " << na << " != " << nb << std::endl;
    abort();
  }

  // Verify output is the right size
  if (out_ptr->Rows() != ma || out_ptr->Columns() != mb) {
    std::cerr << "fastops::ABt: " << out_ptr->Rows() << "x" << out_ptr->Columns() << " is not " << ma << "x" << mb << std::endl;
    abort();
  }

  double const* const* a = A.DataArray();
  double const* const* b = B.DataArray();
  double** out = out_ptr->DataArray();

  for (unsigned i = 0; i < ma; ++i)
    for (unsigned j = 0; j < mb; ++j) {
      double accum = 0;
      for (unsigned k = 0; k < na; ++k)
        accum += a[i][k] * b[j][k];
      out[i][j] = accum;
    }
}

/** Compute $ X += A^\top A$.
*/
void vnl_fastops::inc_X_by_AtA(VNL::Matrix<double>& X, const VNL::Matrix<double>& A)
{
  unsigned m = X.Rows();
  unsigned n = X.Columns();
  unsigned l = A.Rows();

  if (m != n || m != A.Columns()) {
    std::cerr << "fastops::inc_X_by_AtA: size error\n";
    abort();
  }

  double const* const* a = A.DataArray();
  double** x = X.DataArray();

  if (l == 2) {
    for (unsigned i = 0; i < n; ++i) {
      x[i][i] += (a[0][i] * a[0][i] +
        a[1][i] * a[1][i]);
      for (unsigned j = i+1; j < n; ++j) {
        double accum = (a[0][i] * a[0][j] +
          a[1][i] * a[1][j]);
        x[i][j] += accum;
        x[j][i] += accum;
      }
    }
  } else {
    for (unsigned i = 0; i < n; ++i)
      for (unsigned j = i; j < n; ++j) {
        double accum = 0;
        for (unsigned k = 0; k < l; ++k)
          accum += a[k][i] * a[k][j];
        x[i][j] += accum;
        if (i != j)
          x[j][i] += accum;
      }
  }
}

// Compute $X += A^\top B$
void vnl_fastops::inc_X_by_AtB(VNL::Matrix<double>& X, const VNL::Matrix<double>& A, const VNL::Matrix<double>& B)
{
  unsigned mx = X.Rows();
  unsigned nx = X.Columns();
  unsigned ma = A.Rows();
  unsigned na = A.Columns();
  unsigned mb = B.Rows();
  unsigned nb = B.Columns();

  // Verify matrices compatible
  if (ma != mb) {
    std::cerr << "fastops::inc_X_by_AtB: A " << ma << " != B " << mb << std::endl;
    abort();
  }

  // Verify output is the right size
  if (mx != na || nx != nb) {
    std::cerr << "fastops::inc_X_by_AtB: X " << mx << "x" << nx << " is not A " << na << "x" << nb << std::endl;
    abort();
  }

  double const* const* a = A.DataArray();
  double const* const* b = B.DataArray();
  double** x = X.DataArray();

  for (unsigned i = 0; i < na; ++i)
    for (unsigned j = 0; j < nb; ++j) {
      double accum = 0;
      for (unsigned k = 0; k < ma; ++k)
        accum += a[k][i] * b[k][j];
      x[i][j] += accum;
    }
}

/** Compute $X += A^\top b$.
*/
void vnl_fastops::inc_X_by_AtB(VNL::Vector<double>& X, const VNL::Matrix<double>& A, const VNL::Vector<double>& B)
{
  unsigned mx = X.Size();
  unsigned ma = A.Rows();
  unsigned na = A.Columns();
  unsigned mb = B.Size();

  // Verify matrices compatible
  if (ma != mb) {
    std::cerr << "fastops::inc_X_by_AtB: " << ma << " != " << mb << std::endl;
    abort();
  }

  // Verify output is the right size
  if (mx != na) {
    std::cerr << "fastops::inc_X_by_AtB: " << mx << "x" << 1 << " is not " << na << "x" << 1 << std::endl;
    abort();
  }

  double const* const* a = A.DataArray();
  double const* b = B.DataBlock();
  double* x = X.DataBlock();

  for (unsigned i = 0; i < na; ++i) {
    double accum = 0;
    for (unsigned k = 0; k < ma; ++k)
      accum += a[k][i] * b[k];
    x[i] += accum;
  }
}

/** Compute $X -= A^\top B$.
*/
void vnl_fastops::dec_X_by_AtB(VNL::Matrix<double>& X, const VNL::Matrix<double>& A, const VNL::Matrix<double>& B)
{
  unsigned mx = X.Rows();
  unsigned nx = X.Columns();
  unsigned ma = A.Rows();
  unsigned na = A.Columns();
  unsigned mb = B.Rows();
  unsigned nb = B.Columns();

  // Verify matrices compatible
  if (ma != mb) {
    std::cerr << "fastops::inc_X_by_AtB: " << ma << " != " << mb << std::endl;
    abort();
  }

  // Verify output is the right size
  if (mx != na || nx != nb) {
    std::cerr << "fastops::inc_X_by_AtB: " << mx << "x" << nx << " is not " << na << "x" << nb << std::endl;
    abort();
  }

  double const* const* a = A.DataArray();
  double const* const* b = B.DataArray();
  double** x = X.DataArray();

  for (unsigned i = 0; i < na; ++i)
    for (unsigned j = 0; j < nb; ++j) {
      double accum = 0;
      for (unsigned k = 0; k < ma; ++k)
        accum += a[k][i] * b[k][j];
      x[i][j] -= accum;
    }
}

/** Compute dot product of a and b.
*/
double vnl_fastops::dot(const double* a, const double* b, int n)
{
  double accum = 0;
  // Method 2 is fastest on the u170 -- weird.
#define method 3
#if method == 1
  const double* aend = a + n;
  while (a != aend)
    accum += *a++ * *b++;
#endif
#if method == 2
  for (int k = 0; k < n; ++k)
    accum += a[k] * b[k];
#endif
#if method == 3
  while (n--)
    accum += a[n] * b[n];
#endif
  return accum;
}

/** Compute $X -= A B^\top$.
*/
void vnl_fastops::dec_X_by_ABt(VNL::Matrix<double>& X, const VNL::Matrix<double>& A, const VNL::Matrix<double>& B)
{
  unsigned mx = X.Rows();
  unsigned nx = X.Columns();
  unsigned ma = A.Rows();
  unsigned na = A.Columns();
  unsigned mb = B.Rows();
  unsigned nb = B.Columns();

  // Verify matrices compatible
  if (na != nb) {
    std::cerr << "fastops::dec_X_by_ABt: A.columns " << na << " != B.columns " << nb << std::endl;
    abort();
  }

  // Verify output is the right size
  if (mx != ma || nx != mb) {
    std::cerr << "fastops::dec_X_by_ABt: X size " << mx << "x" << nx << " is not AB^T " << ma << "x" << mb << std::endl;
    abort();
  }

  double const* const* a = A.DataArray();
  double const* const* b = B.DataArray();
  double** x = X.DataArray();

  if (na == 3) {
    for (unsigned i = 0; i < mb; ++i)
      for (unsigned j = 0; j < ma; ++j)
        x[j][i] -= (a[j][0] * b[i][0] +
                    a[j][1] * b[i][1] +
                    a[j][2] * b[i][2]);
  } else if (na == 2) {
    for (unsigned i = 0; i < mb; ++i)
      for (unsigned j = 0; j < ma; ++j)
        x[j][i] -= (a[j][0] * b[i][0] +
                    a[j][1] * b[i][1]);
  } else {
    for (unsigned i = 0; i < mb; ++i)
      for (unsigned j = 0; j < ma; ++j)
        x[j][i] -= dot(a[j], b[i], na);
  }
}
