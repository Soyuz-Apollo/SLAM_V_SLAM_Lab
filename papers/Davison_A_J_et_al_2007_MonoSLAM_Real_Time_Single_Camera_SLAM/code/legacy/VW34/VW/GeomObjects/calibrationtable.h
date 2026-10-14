/** 
 * @file   calibrationtable.h 
 * @author BJ Tordoff
 * @date   02/08/2002
 *
 *              (c) Copyright University of Oxford
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/**
 * @class    VW::CalibrationTable
 * @ingroup  GeometricObjects
 * @author   BJ Tordoff
 * @date     02/11/2002
 * @brief    A camera calibration look-up table.
 *
 * This class can hold a single calibration instance for a camera, or a set
 * of calibrations sampled at various zoom-lens motor positions. In the latter
 * case the calibration parameters are used to fit polynomials so that
 * interpolated values can be retrieved.
 *
 * CameraCalibration inherits from IOObject and is able to read calibration
 * data from suitably formatted data files. The data is input/output in the
 * following order:
 *   - Number of calibration data points
 *   - Number of distortion data points
 *   - Array of calibration data
 *      - motor position
 *      - focal-length
 *      - aspect
 *      - skew
 *      - principal-x
 *      - principal-y
 *   - Array of distortion data
 *      - motor position
 *      - kappa
 *
 */



#ifndef _calibration_h_
#define _calibration_h_

#include <vector>
#include <string>
#include <iostream>

// VW includes
#include <VW/MathBits/chebyshevpolynomial.h> // For Chebyshev polynomials
#include <VW/iobasics.h>  // For file IO

// Local includes
#include "calibrationmatrix.h"

#define K_POLYFITORDER 5


#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

namespace VW {
class CalibrationTable : public IOObject {
public:
  CalibrationTable();
  virtual ~CalibrationTable();

  void AddCalibrationData(double motor_position, 
			  const VW::CalibrationMatrix& K);
  void AddDistortionData(double motor_position,
			 const double kappa);
 
  VW::CalibrationMatrix GetCalibrationMatrix(double motor_position) const;
  double GetDistortion(double motor_position) const;

  // reverse lookup
  double GetMotorPosition(double focal_length) const;

  // Test the limits
  void GetMotorLimits(double& motor_min, double& motor_max) const;


  // If varying (default)
  void SetVaryingAspect();
  void SetVaryingSkew();
  void SetVaryingPrincipalPoint();

  // If fixed but need solving
  void SetFixedAspect();
  void SetFixedSkew();
  void SetFixedPrincipalPoint();

  // If known
  void SetKnownAspect(double aspect=1.0);
  void SetKnownSkew(double skew=0.0);
  void SetKnownPrincipalPoint(double px=0.0,
			      double py=0.0);

  // Clear all data
  void Clear();
  

  //  Stream stuff
  virtual 
  bool WriteASCII(std::ostream& stream) const;
  virtual 
  bool WriteXML(std::ostream& stream,
		const std::string& name="") const;
  virtual 
  bool WriteBinary(std::ostream& stream) const;


  virtual 
  bool ReadASCII(std::istream& stream);
  virtual 
  bool ReadXML(std::istream& stream, 
	       const std::string& name="");
  virtual 
  bool ReadBinary(std::istream& stream);

  void PrintPolyCoeffs();


private:
  // Access to raw data points
  unsigned int _GetNumberOfCalibrationData() const;
  unsigned int _GetNumberOfDistortionData() const;
  void _GetRawCalibrationData(unsigned int index,
			     double& motorposition,
			     double& focallength,
			     double& aspect,
			     double& skew,
			     double& principalpointx,
			     double& principalpointy) const;
  void _GetRawDistortionData(unsigned int index,
			    double& motorposition,
			    double& kappa) const;

  void _AddRawCalibrationData(double motorposition,
			     double focallength,
			     double aspect,
			     double skew,
			     double principalpointx,
			     double principalpointy);
  void _AddRawDistortionData(double motorposition,
			    double kappa);

  // Flags as to what is fixed
  bool is_fixed_aspect,is_fixed_skew,is_fixed_pp;
  bool is_known_aspect,is_known_skew,is_known_pp;

  double known_aspect, known_skew, known_px, known_py;

  // Chebyshev polynomials to represent variation with zoom
  VW::ChebyshevPolynomial f_poly,
                          f_reverse_poly,
                          aspect_poly,skew_poly,
                          px_poly,py_poly,
                          kappa_poly;

  // Chebyshev polynomials do nopt extrapolate well, so to prevent
  // demand outside range, the limits must be stored
  double f_min,f_max;
  double motor_min,motor_max;

};

}; // end namespace VW

















// INLINES


inline void VW::CalibrationTable::AddCalibrationData(double motor_position, 
						      const VW::CalibrationMatrix& K)
{
  f_poly.AddData(motor_position,
		 K.GetFocalLength());
  f_reverse_poly.AddData(K.GetFocalLength(),
			 motor_position);
  aspect_poly.AddData(motor_position,
		      K.GetAspectRatio());
  skew_poly.AddData(motor_position,
		    K.GetSkew());
  px_poly.AddData(motor_position,
		  K.GetPrincipalPointX());
  py_poly.AddData(motor_position,
		  K.GetPrincipalPointY());
}

inline void VW::CalibrationTable::AddDistortionData(double motor_position,
						     const double kappa)
{
  kappa_poly.AddData(motor_position,kappa);
}
inline void 
VW::CalibrationTable::GetMotorLimits(double& motor_min, double& motor_max) const
{
  motor_min = this->motor_min;
  motor_max = this->motor_max;
}

inline VW::CalibrationMatrix
VW::CalibrationTable::GetCalibrationMatrix(double motor_position) const
{
  if(VERBOSE) std::cerr << "VW::CalibrationTable::GetCalibrationMatrix: requested motor of "<<motor_position<<" and limits are "<<motor_min<<", "<<motor_max<<std::endl;
  if (motor_position<motor_min) motor_position = motor_min;
  if (motor_position>motor_max) motor_position = motor_max;
  VW::CalibrationMatrix K;

  // F
  K.SetFocalLength(f_poly.GetYValue(motor_position));

  // Aspect
  if (is_known_aspect)
    K.SetAspectRatio(known_aspect);
  else
    K.SetAspectRatio(aspect_poly.GetYValue(motor_position));

  // skew
  if (is_known_skew)
    K.SetSkew(known_skew);
  else
    K.SetSkew(skew_poly.GetYValue(motor_position));

  // PP
  if (is_known_pp) {
    K.SetPrincipalPointX(known_px);
    K.SetPrincipalPointY(known_py);
  } else {
    K.SetPrincipalPointX(px_poly.GetYValue(motor_position));
    K.SetPrincipalPointY(py_poly.GetYValue(motor_position));
  }

  if(VERBOSE) std::cerr << "VW::CalibrationTable::GetCalibrationMatrix: done"<<std::endl;
  return(K);
}
inline double VW::CalibrationTable::GetDistortion(double motor_position) const
{
  return(kappa_poly.GetYValue(motor_position));
}

inline double VW::CalibrationTable::GetMotorPosition(double focal_length) const
{
  // Never ever look outside the calibrated range (Chebyshev polys do
  // not extrapolate well)
  if (focal_length<f_min) focal_length = f_min;
  if (focal_length>f_max) focal_length = f_max;
  return(f_reverse_poly.GetYValue(focal_length));
}

inline void VW::CalibrationTable::Clear()
{
  f_poly.Clear();
  aspect_poly.Clear();
  skew_poly.Clear();
  px_poly.Clear();
  py_poly.Clear();
  kappa_poly.Clear();
}

inline unsigned int VW::CalibrationTable::_GetNumberOfCalibrationData() const
{
  return(f_poly.GetNumberOfDataPoints());
}

inline unsigned int VW::CalibrationTable::_GetNumberOfDistortionData() const
{
  return(kappa_poly.GetNumberOfDataPoints());
}

inline void VW::CalibrationTable::_GetRawCalibrationData(unsigned int index,
							 double& motorposition,
							 double& focallength,
							 double& aspect,
							 double& skew,
							 double& principalpointx,
							 double& principalpointy) const
{
  motorposition   =      f_poly.GetDataX(index);
  focallength     =      f_poly.GetDataY(index);
  aspect          = aspect_poly.GetDataY(index);
  skew            =   skew_poly.GetDataY(index);
  principalpointx =     px_poly.GetDataY(index);
  principalpointy =     py_poly.GetDataY(index);
}

inline void VW::CalibrationTable::_GetRawDistortionData(unsigned int index,
							 double& motorposition,
							 double& kappa) const
{
  motorposition = kappa_poly.GetDataX(index);
  kappa         = kappa_poly.GetDataY(index);
}

  /**
   * @param motorposition    the zoom motor demand value
   * @param focallength      the focal length for this motor position
   * @param aspect           the aspect ratio (fy / fx)
   * @param skew             the skew (atan(theta) - where theta is the skew angle)
   * @param principalpointx  the principal point's X coord (pixels)
   * @param principalpointy  the principal point's Y coord (pixels)
   */
inline void VW::CalibrationTable::_AddRawCalibrationData(double motorposition,
							 double focallength,
							 double aspect,
							 double skew,
							 double principalpointx,
							 double principalpointy)
{
  f_poly.AddData(motorposition,focallength);
  f_reverse_poly.AddData(focallength,motorposition);
  aspect_poly.AddData(motorposition,aspect);
  skew_poly.AddData(motorposition,skew);
  px_poly.AddData(motorposition,principalpointx);
  py_poly.AddData(motorposition,principalpointy);
  f_poly.GetLimits(motor_min,motor_max,
  		   f_min,f_max);

}

inline void VW::CalibrationTable::_AddRawDistortionData(double motorposition,
							double kappa) 
{
  kappa_poly.AddData(motorposition,kappa); 
}

inline void VW::CalibrationTable::SetVaryingAspect()
{
  is_fixed_aspect = false;
  is_known_aspect = false;
  aspect_poly.SetPolynomialOrder(K_POLYFITORDER);
}

inline void VW::CalibrationTable::SetVaryingSkew()
{
  is_fixed_skew = false;
  is_known_skew = false;
  skew_poly.SetPolynomialOrder(K_POLYFITORDER);
}

inline void VW::CalibrationTable::SetVaryingPrincipalPoint()
{
  is_fixed_pp = false;
  is_known_pp = false;
  px_poly.SetPolynomialOrder(K_POLYFITORDER);
  py_poly.SetPolynomialOrder(K_POLYFITORDER);
}

inline void VW::CalibrationTable::SetFixedAspect()
{
  is_fixed_aspect = true;
  is_known_aspect = false;
  aspect_poly.SetPolynomialOrder(0);
}

inline void VW::CalibrationTable::SetFixedSkew()
{
  is_fixed_skew = true;
  is_known_skew = false;
   skew_poly.SetPolynomialOrder(0);
}

inline void VW::CalibrationTable::SetFixedPrincipalPoint()
{
  is_fixed_pp = true;
  is_known_pp = false;
  px_poly.SetPolynomialOrder(0);
  py_poly.SetPolynomialOrder(0);
}
inline void VW::CalibrationTable::SetKnownAspect(double aspect)
{
  is_fixed_aspect = true;
  is_known_aspect = true;
  known_aspect = aspect;
}
inline void VW::CalibrationTable::SetKnownSkew(double skew)
{
  is_fixed_skew = true;
  is_known_skew = true;
  known_skew = skew;
}
inline void VW::CalibrationTable::SetKnownPrincipalPoint(double px,
							  double py)
{
  is_fixed_pp = true;
  is_known_pp = true;
  known_px = px;
  known_py = py;
}

inline void VW::CalibrationTable::PrintPolyCoeffs(){
  
  std::vector<double> poly_coeffs;

  std::cout << "\nPolynomial coefficients:\n";


  f_poly.GetCoeffs(poly_coeffs);
  std::cout << "f_poly: ";
  for(unsigned int i=0; i<poly_coeffs.size(); i++)
    std::cout << poly_coeffs[i] << "\t";
  std::cout << std::endl;

  f_reverse_poly.GetCoeffs(poly_coeffs);
  std::cout << "f_reverse_poly: ";
  for(unsigned int i=0; i<poly_coeffs.size(); i++)
    std::cout << poly_coeffs[i] << "\t";
  std::cout << std::endl;

  aspect_poly.GetCoeffs(poly_coeffs);
  std::cout << "aspect_poly: ";
  for(unsigned int i=0; i<poly_coeffs.size(); i++)
    std::cout << poly_coeffs[i] << "\t";
  std::cout << std::endl;


  skew_poly.GetCoeffs(poly_coeffs);
  std::cout << "skew_poly: ";
  for(unsigned int i=0; i<poly_coeffs.size(); i++)
    std::cout << poly_coeffs[i] << "\t";
  std::cout << std::endl;


  px_poly.GetCoeffs(poly_coeffs); 
  std::cout << "px_poly: "; 
  for(unsigned int i=0; i<poly_coeffs.size(); i++) 
    std::cout << poly_coeffs[i] << "\t"; 
  std::cout << std::endl; 

  py_poly.GetCoeffs(poly_coeffs);
  std::cout << "py_poly: ";
  for(unsigned int i=0; i<poly_coeffs.size(); i++)
    std::cout << poly_coeffs[i] << "\t";
  std::cout << std::endl;

  kappa_poly.GetCoeffs(poly_coeffs);
  std::cout << "kappa_poly: ";
  for(unsigned int i=0; i<poly_coeffs.size(); i++)
    std::cout << poly_coeffs[i] << "\t";
  std::cout << std::endl;


}



#endif // _calibration_h_


