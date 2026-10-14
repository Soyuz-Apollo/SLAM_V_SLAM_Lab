#ifndef _covariance2d_h_
#define _covariance2d_h_

#include <VNL/matrixfixed.h>

namespace VW {

class Covariance2D {
public:
  /** Construct and set to identity */
  Covariance2D();

  /** Construct from the three parameters of a symmetric 2x2 matrix
	*    C = [ a  b ]
	*        [ b  c ]
	*/
  Covariance2D(double a, double b, double c);
  Covariance2D(const Covariance2D& C);

  /** Construct from an arbitrary 2x2 matrix. If the matrix C is not positive
	* semi-definite an assertion failure results.
	*/
  explicit Covariance2D(const VNL::MatrixFixed<2,2,double>& C);

  void SetIdentity();

  /** Set to diagonal with equal variances along each axis. */
  void SetIsotropic(const double variance);

  /** The matrix must be either positive definite or semi-definite. */
  bool IsPositiveDefinite() const;

  /** the determinant >= 0.
	* Cost = 3 flops
	*/
  double Determinant() const;

  /** If the matrix is positive definite then the inverse is also
	* symmetrical and positive definite (ie a covariance). 
	* Cost = 6 flops
	*/
  Covariance2D Inverse() const;

  /** Get the matrix as a 2x2 VNL::MatrixFixed. */
  VNL::MatrixFixed<2,2,double> AsMatrix() const;

  /** Get the inverse of the matrix as a 2x2 VNL::MatrixFixed.
	* Cost = 6 flops 
	*/
  VNL::MatrixFixed<2,2,double> InverseMatrix() const;

  /** Perform a cholesky decomposition, returning the lower triangle L such
   * that C = L*L'. This will fail if the matrix is not positive semi-definite,
   * returning false.
   */
  bool CholeskyDecompose(VNL::MatrixFixed<2,2,double>& L) const;

  /** Get the distance of this matrix d = (ex ey) C (ex ey)'.
   * If [ex ey] forms a unit vector, this is the Rayleigh quotient.
   */
  double GetDistance(const double ex, const double ey) const;

  /** Get the Mahalanobis distance given errors in x and y
   *   d = (ex ey) inv(C) (ex ey)'
   * Note that the inverse makes this a bit expensive, so if you want to use
   * it repeatedly, calculate the inverse matrix explicitly and use the
   * GetDistance() function of the result.
   */
  double GetMahalanobisDistance(const double ex, const double ey) const;


  /** Scale the covariance (must be a positive scalar). */
  Covariance2D operator*(const double scalar) const;

  /** Scale the covariance (must be a positive scalar). */
  Covariance2D& operator*=(const double scalar);

  /** Add a covariances. */
  Covariance2D operator+(const Covariance2D& C) const;

  /** Add a covariances. */
  Covariance2D& operator+=(const Covariance2D& C);

  /** Scale the covariance (must be a positive scalar). */
  Covariance2D operator/(const double scalar) const;

  /** Scale the covariance (must be a positive scalar). */
  Covariance2D& operator/=(const double scalar);

  /** Multiplying two covariances can result in a non-symmetric matrix. */
  VNL::MatrixFixed<2,2,double> operator*(const Covariance2D& C) const;

  /** directly access the data (three doubles) - use with extreme caution */
  double& operator[](int i);
  /** directly access the data (three doubles) - use with extreme caution */
  const double& operator[](int i) const;

private:
  double _a, _b, _c;
};

// Some useful operators
/** Pre-multiply by an arbitrary quare matrix 
 * Cost is 12 flops.
 */
VNL::MatrixFixed<2,2,double> operator*(const VNL::MatrixFixed<2,2,double>& M, 
													const Covariance2D& C); 
/** Post-multiply by an arbitrary quare matrix 
 * Cost is 12 flops.
 */
VNL::MatrixFixed<2,2,double> operator*(const Covariance2D& C, 
													const VNL::MatrixFixed<2,2,double>& M); 

/** Map the Covariance using another matrix C2 = M*C*M' 
 * Cost is 21 flops.
 */
VW::Covariance2D MapCovariance(const VNL::MatrixFixed<2,2,double>& M, 
										 const Covariance2D& C);




}; // End namespace VW














// INLINES

inline VW::Covariance2D::Covariance2D()
  : _a(1.0),
	 _b(0.0),
	 _c(1.0)
{
}

inline VW::Covariance2D::Covariance2D(double a, double b, double c)
  : _a(a),
	 _b(b),
	 _c(c)
{
}

inline VW::Covariance2D::Covariance2D(const Covariance2D& C)
  : _a(C._a),
	 _b(C._b),
	 _c(C._c)
{
}

inline VW::Covariance2D::Covariance2D(const VNL::MatrixFixed<2,2,double>& C)
  : _a(C[0][0]),
	 _b(0.5*(C[0][1]+C[1][0])),
	 _c(C[1][1])
{
}

inline void VW::Covariance2D::SetIdentity()
{
  _a = _c = 1.0;
  _b = 0.0;
}

inline void VW::Covariance2D::SetIsotropic(const double variance)
{
  _a = _c = variance;
  _b = 0.0;
}

inline VNL::MatrixFixed<2,2,double> VW::Covariance2D::AsMatrix() const
{
  VNL::MatrixFixed<2,2,double> result;
  result[0][0] = _a;
  result[0][1] = result[1][0] = _b;
  result[1][1] = _c;
  return result;
}

inline double VW::Covariance2D::Determinant() const
{
  return _a*_c - _b*_b;
}

inline double VW::Covariance2D::
GetDistance(const double ex, const double ey) const
{
  return _a*ex*ex + 2.0*_b*ex*ey + _c*ey*ey;
}

inline VNL::MatrixFixed<2,2,double> VW::Covariance2D::operator*(const Covariance2D& C) const
{
  VNL::MatrixFixed<2,2,double> result;
  result[0][0] = this->_a * C._a  +  this->_b * C._b;
  result[0][1] = this->_a * C._b  +  this->_b * C._c;
  result[1][0] = this->_b * C._a  +  this->_c * C._b;
  result[1][1] = this->_b * C._b  +  this->_c * C._c;
  return result;	  
}

inline VW::Covariance2D 
VW::Covariance2D::operator*(const double scalar) const
{
  assert(scalar > 0);
  return Covariance2D(_a*scalar, _b*scalar, _c*scalar);
}

inline VW::Covariance2D& 
VW::Covariance2D::operator*=(const double scalar)
{
  assert(scalar > 0);
  _a *= scalar;
  _b *= scalar;
  _c *= scalar;
  return *this;
}

inline VW::Covariance2D VW::Covariance2D::operator+(const Covariance2D& C) const
{
  return Covariance2D(_a+C._a, _b+C._b, _c+C._c);
}

inline VW::Covariance2D& VW::Covariance2D::operator+=(const Covariance2D& C)
{
  _a += C._a;
  _b += C._b;
  _c += C._c;
  return *this;  
}

inline VW::Covariance2D 
VW::Covariance2D::operator/(const double scalar) const
{
  assert(scalar > 0);
  const double inv_scalar = 1.0/scalar;
  return Covariance2D(_a*inv_scalar, _b*inv_scalar, _c*inv_scalar);
}

inline VW::Covariance2D& 
VW::Covariance2D::operator/=(const double scalar)
{
  assert(scalar > 0);
  const double inv_scalar = 1.0/scalar;
  _a *= inv_scalar;
  _b *= inv_scalar;
  _c *= inv_scalar;
  return *this;
}

inline const double& VW::Covariance2D::operator[](int i) const
{
  return *(&_a + i);
}

inline double& VW::Covariance2D::operator[](int i)
{
  return *(&_a + i);
}

inline VNL::MatrixFixed<2,2,double> 
VW::operator*(const VNL::MatrixFixed<2,2,double>& M, 
				  const Covariance2D& C)
{
  VNL::MatrixFixed<2,2,double> result;
  result[0][0] = M[0][0]*C[0] + M[0][1]*C[1];
  result[0][1] = M[0][0]*C[1] + M[0][1]*C[2];

  result[1][0] = M[1][0]*C[0] + M[1][1]*C[1];
  result[1][1] = M[1][0]*C[1] + M[1][1]*C[2];

  return result;	  
}

inline VNL::MatrixFixed<2,2,double>
VW::operator*(const VW::Covariance2D& C, 
				  const VNL::MatrixFixed<2,2,double>& M)
{
  VNL::MatrixFixed<2,2,double> result;
  result[0][0] = C[0]*M[0][0] + C[1]*M[1][0];
  result[0][1] = C[0]*M[0][1] + C[1]*M[1][1];

  result[1][0] = C[1]*M[0][0] + C[2]*M[1][0];
  result[1][1] = C[1]*M[0][1] + C[2]*M[1][1];

  return result;	  
}


inline VW::Covariance2D 
VW::MapCovariance(const VNL::MatrixFixed<2,2,double>& M, 
						const VW::Covariance2D& C)
{
  // First product must be full (this is actually the transpose)
  VNL::MatrixFixed<2,2,double> CM(M*C); // 12 flops
  const double a = M[0][0]*CM[0][0] + M[0][1]*CM[0][1];
  const double b = M[0][0]*CM[1][0] + M[0][1]*CM[1][1];
  const double c = M[1][0]*CM[1][0] + M[1][1]*CM[1][1];
  return Covariance2D(a,b,c);
}

#endif
