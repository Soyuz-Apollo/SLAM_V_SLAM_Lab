#ifndef _covariance3d_h_
#define _covariance3d_h_

#include <VNL/matrixfixed.h>

namespace VW {
  class Covariance2D;

class Covariance3D {
public:
  /** Construct and set to identity */
  Covariance3D();

  /** Construct from the three parameters of a symmetric 3x3 matrix
	*    C = [ a  b  c ]
	*        [ b  d  e ]
	*        [ c  e  f ]
	*/
  Covariance3D(double a, double b, double c, double d, double e, double f);
  Covariance3D(const Covariance3D& C);

  /** Construct from an arbitrary 3x3 matrix. If the matrix C is not positive
	* semi-definite an assertion failure results.
	*/
  explicit Covariance3D(const VNL::MatrixFixed<3,3,double>& C);

  void SetIdentity();

  /** Set to diagonal with equal variances along each axis. */
  void SetIsotropic(const double variance);

  /** The matrix must be either positive definite or semi-definite. */
  bool IsPositiveDefinite() const;

  /** the determinant >= 0.
	* Cost = 15 flops
	*/
  double Determinant() const;

  /** If the matrix is positive definite then the inverse is also
	* symmetrical and positive definite (ie a covariance). 
	* Cost = 40 flops
	*/
  Covariance3D Inverse() const;

  /** Get the matrix as a 3x3 VNL::MatrixFixed. */
  VNL::MatrixFixed<3,3,double> AsMatrix() const;

  /** Get the inverse of the matrix as a 3x3 VNL::MatrixFixed.
	* Cost = 40 flops 
	*/
  VNL::MatrixFixed<3,3,double> InverseMatrix() const;

  /** Get the distance of this matrix d = (ex ey ez) C (ex ey ez)'.
   * If [ex ey] forms a unit vector, this is the Rayleigh quotient.
	* Cost = 18 flops
   */
  double GetDistance(const double ex, 
							const double ey, 
							const double ez) const;

  /** Get the Mahalanobis distance given errors in x and y
   *   d = (ex ey ez) inv(C) (ex ey ez)'
   * Note that the inverse makes this a bit expensive, so if you want to use
   * it repeatedly, calculate the inverse matrix explicitly and use the
   * GetDistance() function of the result.
	* Cost = 52 flops
   */
  double GetMahalanobisDistance(const double ex, 
										  const double ey, 
										  const double ez) const;


  /** Scale the covariance (must be a positive scalar). */
  Covariance3D operator*(const double scalar) const;

  /** Scale the covariance (must be a positive scalar). */
  Covariance3D& operator*=(const double scalar);

  /** Add a covariances. */
  Covariance3D operator+(const Covariance3D& C) const;

  /** Add a covariances. */
  Covariance3D& operator+=(const Covariance3D& C);

  /** Scale the covariance (must be a positive scalar). */
  Covariance3D operator/(const double scalar) const;

  /** Scale the covariance (must be a positive scalar). */
  Covariance3D& operator/=(const double scalar);

  /** Multiplying two covariances can result in a non-symmetric matrix. 
	* Cost is 45 flops */
  VNL::MatrixFixed<3,3,double> operator*(const Covariance3D& C) const;

  /** Treat this 3D covariance as a 2D homogeneous one and linearise it
   * around the point (x,y,w) to get the 2D non-homogeneous one. 
   * If the point is linearised as [u,v] = [x/w, y/w] then the covariance
   * linearises as C_2 = J C J' where J = [ w  0 -x ] / w^2
   *                                      [ 0  w -y ]
   * This is optimised to take 26 flops (standard multiply would take 45).
   */
  VW::Covariance2D LineariseAroundHomgPoint(const double x,
					    const double y,
					    const double w) const;

  /** directly access the data (six doubles) - use with extreme caution */
  const double& operator[](int i) const;
  /** directly access the data (six doubles) - use with extreme caution */
  double& operator[](int i);


private:
  double _a, _b, _c, _d, _e, _f;
};


// Some useful operators
/** Pre-multiply by an arbitrary quare matrix 
 * Cost is 45 flops.
 */
VNL::MatrixFixed<3,3,double> operator*(const VNL::MatrixFixed<3,3,double>& M, 
													const Covariance3D& C); 
/** Post-multiply by an arbitrary quare matrix 
 * Cost is 45 flops.
 */
VNL::MatrixFixed<3,3,double> operator*(const Covariance3D& C, 
													const VNL::MatrixFixed<3,3,double>& M); 

/** Map the Covariance using another matrix C2 = M*C*M' 
 * Cost is 75 flops.
 */
VW::Covariance3D MapCovariance(const VNL::MatrixFixed<3,3,double>& M, 
			       const Covariance3D& C);



}; // End namespace VW

















// INLINES

inline VW::Covariance3D::Covariance3D()
  : _a(1.0),
	 _b(0.0),
	 _c(0.0),
	 _d(1.0),
	 _e(0.0),
	 _f(1.0)
{
}

inline VW::Covariance3D::Covariance3D(double a, double b, double c, 
												  double d, double e, double f)
  : _a(a),
	 _b(b),
	 _c(c),
	 _d(d),
	 _e(e),
	 _f(f)
{
}

inline VW::Covariance3D::Covariance3D(const Covariance3D& C)
  : _a(C._a),
	 _b(C._b),
	 _c(C._c),
	 _d(C._d),
	 _e(C._e),
	 _f(C._f)
{
}

inline VW::Covariance3D::Covariance3D(const VNL::MatrixFixed<3,3,double>& C)
  : _a(C[0][0]),
	 _b(0.5*(C[0][1]+C[1][0])),
	 _c(0.5*(C[0][2]+C[2][0])),
	 _d(C[1][1]),
	 _e(0.5*(C[1][2]+C[2][1])),
	 _f(C[2][2])
{
}

inline void VW::Covariance3D::SetIdentity()
{
  _a = _d = _f = 1.0;
  _b = _c = _e = 0.0;
}

inline void VW::Covariance3D::SetIsotropic(const double variance)
{
  _a = _d = _f = variance;
  _b = _c = _e = 0.0;
}

inline VNL::MatrixFixed<3,3,double> VW::Covariance3D::AsMatrix() const
{
  VNL::MatrixFixed<3,3,double> result;
  result[0][0] = _a;
  result[0][1] = result[1][0] = _b;
  result[0][2] = result[2][0] = _c;
  result[1][1] = _d;
  result[1][2] = result[2][1] = _e;
  result[2][2] = _f;
  return result;
}

inline double VW::Covariance3D::Determinant() const
{
  return 2*_b*_c*_e + _a*_d*_f
	    - _a*_e*_e - _d*_c*_c - _f*_b*_b;
}

inline double VW::Covariance3D::
GetDistance(const double ex, const double ey, const double ez) const
{
  return _a*ex*ex + _d*ey*ey + _f*ez*ez +
 	 2.0*(_b*ex*ey + _c*ex*ez + _e*ey*ez);
}


inline VNL::MatrixFixed<3,3,double>
VW::Covariance3D::operator*(const VW::Covariance3D& C) const
{
  VNL::MatrixFixed<3,3,double> result;
  result[0][0] = this->_a*C._a + this->_b*C._b + this->_c*C._c;
  result[0][1] = this->_a*C._b + this->_b*C._d + this->_c*C._e;
  result[0][2] = this->_a*C._c + this->_b*C._e + this->_c*C._f;

  result[1][0] = this->_b*C._a + this->_d*C._b + this->_e*C._c;
  result[1][1] = this->_b*C._b + this->_d*C._d + this->_e*C._e;
  result[1][2] = this->_b*C._c + this->_d*C._e + this->_e*C._f;

  result[2][0] = this->_c*C._a + this->_e*C._b + this->_f*C._c;
  result[2][1] = this->_c*C._b + this->_e*C._d + this->_f*C._e;
  result[2][2] = this->_c*C._c + this->_e*C._e + this->_f*C._f;

  return result;	  
}

inline VW::Covariance3D 
VW::Covariance3D::operator*(const double scalar) const
{
  assert(scalar > 0);
  return Covariance3D(_a*scalar, _b*scalar, _c*scalar,
							 _d*scalar, _e*scalar, _f*scalar);
}

inline VW::Covariance3D& 
VW::Covariance3D::operator*=(const double scalar)
{
  assert(scalar > 0);
  _a *= scalar;
  _b *= scalar;
  _c *= scalar;
  _d *= scalar;
  _e *= scalar;
  _f *= scalar;
  return *this;
}
inline VW::Covariance3D 
VW::Covariance3D::operator+(const Covariance3D& C) const
{
  return Covariance3D(_a+C._a, _b+C._b, _c+C._c, _d+C._d, _e+C._e, _f+C._f);
}

inline VW::Covariance3D& 
VW::Covariance3D::operator+=(const Covariance3D& C)
{
  _a += C._a;
  _b += C._b;
  _c += C._c;
  _d += C._d;
  _e += C._e;
  _f += C._f;
  return *this;  
}

inline VW::Covariance3D 
VW::Covariance3D::operator/(const double scalar) const
{
  assert(scalar > 0);
  const double inv_scalar = 1.0/scalar;
  return Covariance3D(_a*inv_scalar, _b*inv_scalar, _c*inv_scalar,
							 _d*inv_scalar, _e*inv_scalar, _f*inv_scalar);
}

inline VW::Covariance3D& 
VW::Covariance3D::operator/=(const double scalar)
{
  assert(scalar > 0);
  const double inv_scalar = 1.0/scalar;
  _a *= inv_scalar;
  _b *= inv_scalar;
  _c *= inv_scalar;
  _d *= inv_scalar;
  _e *= inv_scalar;
  _f *= inv_scalar;
  return *this;
}

inline const double& VW::Covariance3D::operator[](int i) const
{
  return *(&_a + i);
}

inline double& VW::Covariance3D::operator[](int i)
{
  return *(&_a + i);
}

inline VNL::MatrixFixed<3,3,double> 
VW::operator*(const VNL::MatrixFixed<3,3,double>& M, 
			 const Covariance3D& C)
{
  VNL::MatrixFixed<3,3,double> result;
  result[0][0] = M[0][0]*C[0] + M[0][1]*C[1] + M[0][2]*C[2];
  result[0][1] = M[0][0]*C[1] + M[0][1]*C[3] + M[0][2]*C[4];
  result[0][2] = M[0][0]*C[2] + M[0][1]*C[4] + M[0][2]*C[5];

  result[1][0] = M[1][0]*C[0] + M[1][1]*C[1] + M[1][2]*C[2];
  result[1][1] = M[1][0]*C[1] + M[1][1]*C[3] + M[1][2]*C[4];
  result[1][2] = M[1][0]*C[2] + M[1][1]*C[4] + M[1][2]*C[5];

  result[2][0] = M[2][0]*C[0] + M[2][1]*C[1] + M[2][2]*C[2];
  result[2][1] = M[2][0]*C[1] + M[2][1]*C[3] + M[2][2]*C[4];
  result[2][2] = M[2][0]*C[2] + M[2][1]*C[4] + M[2][2]*C[5];

  return result;	  
}
inline VNL::MatrixFixed<3,3,double>
VW::operator*(const Covariance3D& C, const VNL::MatrixFixed<3,3,double>& M)
{
  VNL::MatrixFixed<3,3,double> result;
  result[0][0] = C[0]*M[0][0] + C[1]*M[1][0] + C[2]*M[2][0];
  result[0][1] = C[0]*M[0][1] + C[1]*M[1][1] + C[2]*M[2][1];
  result[0][2] = C[0]*M[0][2] + C[1]*M[1][2] + C[2]*M[2][2];

  result[1][0] = C[1]*M[0][0] + C[3]*M[1][0] + C[4]*M[2][0];
  result[1][1] = C[1]*M[0][1] + C[3]*M[1][1] + C[4]*M[2][1];
  result[1][2] = C[1]*M[0][2] + C[3]*M[1][2] + C[4]*M[2][2];

  result[2][0] = C[2]*M[0][0] + C[4]*M[1][0] + C[5]*M[2][0];
  result[2][1] = C[2]*M[0][1] + C[4]*M[1][1] + C[5]*M[2][1];
  result[2][2] = C[2]*M[0][2] + C[4]*M[1][2] + C[5]*M[2][2];

  return result;	  
}

inline VW::Covariance3D 
VW::MapCovariance(const VNL::MatrixFixed<3,3,double>& M, 
				  const VW::Covariance3D& C)
{
  // First product must be full (this is actually the transpose)
  VNL::MatrixFixed<3,3,double> CM(M*C);
  const double a = M[0][0]*CM[0][0] + M[0][1]*CM[0][1] + M[0][2]*CM[0][2];
  const double b = M[0][0]*CM[1][0] + M[0][1]*CM[1][1] + M[0][2]*CM[1][2];
  const double c = M[0][0]*CM[2][0] + M[0][1]*CM[2][1] + M[0][2]*CM[2][2];
  const double d = M[1][0]*CM[1][0] + M[1][1]*CM[1][1] + M[1][2]*CM[1][2];
  const double e = M[1][0]*CM[2][0] + M[1][1]*CM[2][1] + M[1][2]*CM[2][2];
  const double f = M[2][0]*CM[2][0] + M[2][1]*CM[2][1] + M[2][2]*CM[2][2];
  return Covariance3D(a,b,c,d,e,f);
}

#endif
