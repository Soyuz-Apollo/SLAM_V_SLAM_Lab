// This is vxl/VNL/binary.cxx

/**
* \file
* \author fsm@robots.ox.ac.uk
*/


#include <iostream>
#include <new>

#include <VNL/vector.h>
#include <VNL/matrix.h>
#include <VNL/diagmatrix.h>

// KAI C++ wants char*, so it must be correct....
#define stream_cast (char*)

// -------------------- vnl_vector

namespace VNL {

template <class T>
bool BinarySave(std::ostream &f, VNL::Vector<T> const &v)
{
  int tmp = v.size();
  f.write(stream_cast &tmp, sizeof(tmp));
  if (v.DataBlock())
    f.write(stream_cast v.DataBlock(), v.size() * sizeof(T));
  else
    ; // FIXME: vector  has been default-constructed.
  return true;
}

template <class T>
bool BinaryLoad(std::istream &f, VNL::Vector<T> &v)
{
  int n = -1;
  f.read(stream_cast &n, sizeof(n));
  if (n > 0) {
    v.Resize(n);
    f.read(stream_cast v.DataBlock(), v.size() * sizeof(T));
  }
  else
    v.Clear();
  return true;
}

/**vnl_matrix.
*/
template <class T>
bool BinarySave(std::ostream &f, VNL::Matrix<T> const &A)
{
  int tmp;
  tmp = A.Rows(); f.write(stream_cast &tmp, sizeof(tmp));
  tmp = A.Cols(); f.write(stream_cast &tmp, sizeof(tmp));
  if (A.DataArray())
    f.write(stream_cast A.DataBlock(), A.size() * sizeof(T));
  else
    ; // FIXME: matrix has been default-constructed.
  return true;
}

template <class T>
bool BinaryLoad(std::istream &f, VNL::Matrix<T> &A)
{
  int r = -1, c = -1;
  f.read(stream_cast &r, sizeof(r));
  f.read(stream_cast &c, sizeof(c));
  //std::cerr << "r c = " << r << ' ' << c << std::endl;
  if (r > 0 || c > 0) {
    A.Resize(r, c);
    f.read(stream_cast A.DataBlock(), A.size() * sizeof(T));
  }
  else
    A.Clear();
  return true;
}

/** vnl_diag_matrix.
*/

template <class T>
bool BinarySave(std::ostream &f, VNL::DiagMatrix<T> const &D)
{
  int tmp = D.size();
  f.write(stream_cast &tmp, sizeof(tmp));
  if (D.DataBlock())
    f.write(stream_cast D.DataBlock(), D.size() * sizeof(T));
  else
    ; // FIXME: matrix has been default-constructed.
  return true;
}

template <class T>
bool BinaryLoad(std::istream &f, VNL::DiagMatrix<T> &D)
{
  int n = -1;
  f.read(stream_cast &n, sizeof(n));
  if (n > 0) {
    D.Resize(n);
    f.read(stream_cast D.DataBlock(), D.size() * sizeof(T));
  }
  else
    D.Clear();

  return true;
}

//------------------------------------------------------------

#define inst(T) \
	template bool BinarySave(std::ostream &, VNL::Vector<T > const &); \
	template bool BinarySave(std::ostream &, VNL::Matrix<T > const &); \
	template bool BinarySave(std::ostream &, VNL::DiagMatrix<T > const &); \
	template bool BinaryLoad(std::istream &, VNL::Vector<T > &); \
	template bool BinaryLoad(std::istream &, VNL::Matrix<T > &); \
	template bool BinaryLoad(std::istream &, VNL::DiagMatrix<T > &)

inst(int);
inst(float);
inst(double);

}; // end namespace VNL
