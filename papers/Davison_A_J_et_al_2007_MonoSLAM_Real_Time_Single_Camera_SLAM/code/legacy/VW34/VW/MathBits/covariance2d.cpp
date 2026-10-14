#include "covariance2d.h"

VW::Covariance2D VW::Covariance2D::Inverse() const
{
  const double invdet = 1.0 / this->Determinant();
  return Covariance2D(_c*invdet, -_b*invdet, _a*invdet);
}

VNL::MatrixFixed<2,2,double> VW::Covariance2D::InverseMatrix() const
{
  const double invdet = 1.0 / this->Determinant();
  VNL::MatrixFixed<2,2,double> inverse;
  inverse[0][0] = _c*invdet;
  inverse[0][1] = inverse[1][0] = -_b*invdet;
  inverse[1][1] = _a*invdet;
  return inverse;
}

double VW::Covariance2D::
GetMahalanobisDistance(const double ex, const double ey) const
{
  const double numerator = _c*ex*ex - 2.0*_b*ex*ey + _a*ey*ey;
  const double denominator = _a*_c - _b*_b;
  return numerator/denominator;
}


bool VW::Covariance2D::CholeskyDecompose(VNL::MatrixFixed<2,2,double>& L) const
{
  L.Fill(0.0);
  if (_a<0.0) {
    return false;
  } else if (_a == 0.0) {
    if (_b == 0.0) {
      // Still semi-def, which means the 0,1 and 1,0 are unknown
      L[1][1] = sqrt(_c); 
      return true;
    } else {
      // Not pos-semidef
      return false;
    }
  } else {
    L[0][0] = sqrt(_a);
    L[1][0] = _b / L[0][0];
    const double l2 = L[1][0]*L[1][0];
    
    if (l2 > _c) {
      return false;
    } else {
      L[1][1] = sqrt(_c - l2);
    }
  }
  return true;
}
