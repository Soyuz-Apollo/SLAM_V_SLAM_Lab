#include "covariance2d.h"
#include "covariance3d.h"

double VW::Covariance3D::
GetMahalanobisDistance(const double ex, const double ey, const double ez) const
{
  const double new_a = (_d*_f - _e*_e); // +3 flops per entry
  const double new_b = (_c*_e - _b*_f);
  const double new_c = (_b*_e - _c*_d);
  const double new_d = (_a*_f - _c*_c);
  const double new_e = (_b*_c - _a*_e);
  const double new_f = (_a*_d - _b*_b);
  // 18 flops to get distance
  const double numerator = new_a*ex*ex + new_d*ey*ey + new_f*ez*ez
                    + 2.0*(new_b*ex*ey + new_c*ex*ez + new_e*ey*ez)
	                                    ;;
  const double denominator = this->Determinant(); // 16 flops
  return numerator/denominator;
}

VW::Covariance3D VW::Covariance3D::Inverse() const
{
  const double invdet = 1.0 / this->Determinant(); // 16 flops
  const double new_a = invdet*(_d*_f - _e*_e); // +4 flops per entry
  const double new_b = invdet*(_c*_e - _b*_f);
  const double new_c = invdet*(_b*_e - _c*_d);
  const double new_d = invdet*(_a*_f - _c*_c);
  const double new_e = invdet*(_b*_c - _a*_e);
  const double new_f = invdet*(_a*_d - _b*_b);

  return Covariance3D(new_a, new_b, new_c, new_d, new_e, new_f);
}

VNL::MatrixFixed<3,3,double> VW::Covariance3D::InverseMatrix() const
{
  const double invdet = 1.0 / this->Determinant();
  const double new_a = invdet*(_d*_f - _e*_e);
  const double new_b = invdet*(_c*_e - _b*_f);
  const double new_c = invdet*(_b*_e - _c*_d);
  const double new_d = invdet*(_a*_f - _c*_c);
  const double new_e = invdet*(_b*_c - _a*_e);
  const double new_f = invdet*(_a*_d - _b*_b);

  VNL::MatrixFixed<3,3,double> inverse;
  inverse[0][0] = invdet*(_d*_f - _e*_e);
  inverse[0][1] = inverse[1][0] = invdet*(_c*_e - _b*_f);
  inverse[0][2] = inverse[2][0] = invdet*(_b*_e - _c*_d);
  inverse[1][1] = invdet*(_a*_f - _c*_c);
  inverse[1][2] = inverse[2][1] = invdet*(_b*_c - _a*_e);
  inverse[2][2] = invdet*(_a*_d - _b*_b);
  return inverse;
}

VW::Covariance2D VW::Covariance3D::
LineariseAroundHomgPoint(const double x,
			 const double y,
			 const double w) const
{
  const double z = 1.0/w;
  const double z2 = z*z;
  const double xz = x*z, yz = y*z;
  VW::Covariance2D dest_covar(_a - 2.0*_c*xz + _f*xz*xz, // 6
			      _b - _c*yz - _e*xz + _f*xz*yz, // 7
			      _d - 2.0*_e*yz + _f*yz*yz); // 6
  dest_covar *= z2; // 3
  return dest_covar;
}

