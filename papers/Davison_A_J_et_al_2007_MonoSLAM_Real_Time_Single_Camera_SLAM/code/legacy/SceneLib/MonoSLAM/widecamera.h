/*  SceneLib: software for real-time SLAM

    widecamera.h
    Copyright (C) 2005 University of Oxford

    Author
    Paul Smith
    pas@robots.ox.ac.uk

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef WIDECAMERA_H
#define WIDECAMERA_H

#include <VNL/vectorfixed.h>
#include <VNL/matrixfixed.h>

/**
A model of a pinhole camera with cubic distortion, able to project image rays
into the camera image, and vice versa. This is the basic camera type, but
different, more or less specific, camera types could also be derived from this.

The distortion model used in this camera is the Swaminathan and Nayar
(2000) model, which is invertible to a good approximation:
\f[
\begin{aligned}
\vct{u}_{\text{image}} - \vct{u}_0 &=
  \frac{\vct{u}_{\text{ideal}} - \vct{u}_0}{\sqrt{1+2 k_1 r^2}} \\
\vct{u}_{\text{ideal}} - \vct{u}_0 &=
  \frac{\vct{u}_{\text{image}} - \vct{u}_0}{\sqrt{1-2 k_1 r^2}}
\end{aligned}
\f]
where \f$ \vct{u}_0 \f$ is the centre of distortion (assumed to be the camera
centre \f$ (u_0, v_0) \f$, and \f$ r = |\vct{u}_{\text{ideal}} -
\vct{u}_0| \f$ or \f$ r = |\vct{u}_{\text{image}} - \vct{u}_0| \f$ as
appropriate.
@ingroup gMonoSLAM
**/
class WideCamera
{
public:
  WideCamera();
  WideCamera(int w, int h, double Kd1, double Fku,
         double Fkv, double U0, double V0, double measurement_sd);
  /// Destructor does nothing.
  virtual ~WideCamera() {}
  
  // Virtual to allow the class to be overloaded to provide different behaviour
  virtual VNL::VectorFixed<2> Project(const VNL::VectorFixed<3>& camframe);
  virtual VNL::VectorFixed<3> Unproject(const VNL::VectorFixed<2>& imframe);
  virtual VNL::MatrixFixed<2,3> ProjectionJacobian();
  virtual VNL::MatrixFixed<3,2> UnprojectionJacobian();
  virtual VNL::MatrixFixed<2,2> MeasurementNoise(const VNL::VectorFixed<2>& h);
  
  /// What is the image width?
  unsigned int ImageWidth() const {return m_image_width;}
  /// What is the image height?
  unsigned int ImageHeight() const {return m_image_height;}
  
  /// First radial distrotion coefficient
  double Kd1() const {return m_Kd1;} 
  /// Focal length \f$ \times \f$ pixel density in \f$ u \f$ image direction.
  double Fku() const {return m_Fku;} 
  /// Focal length \f$ \times \f$ pixel density in \f$ v \f$ image direction.
  double Fkv() const {return m_Fkv; }
  /// Optical centre
  const VNL::VectorFixed<2>& Centre() {return m_centre;}
  /// Optical centre in \f$ u \f$ image direction.
  double U0() const {return m_centre[0];}
  /// Optical centre in \f$ v \f$ image direction.
  double V0() const {return m_centre[1];}   
  /** Measurement standard deviation. This is the noise at the centre of
  the image - see  MeasurementNoise() for a description of how this is used. */
  double MeasurementNoise() const {return m_measurement_sd;}
  
  /// What is the camera calibration matrix?
  const VNL::MatrixFixed<3,3,double>& C() const {return m_C;}
  /// What is the inverse camera calibration matrix?
  const VNL::MatrixFixed<3,3,double>& Cinv() const {return m_Cinv;}

  friend std::istream& operator >>(std::istream& os, WideCamera& c);

private:
  void MakeProjectionMatrices();

private:
  // Image size
  unsigned int m_image_width;  ///< Image width
  unsigned int m_image_height; ///< Image height
  // Camera calibration parameters
  double m_Kd1; ///< First radial distortion coefficient.
  double m_Fku; ///< Focal length \f$ \times \f$ pixel density in \f$ u \f$ image direction.
  double m_Fkv; ///< Focal length \f$ \times \f$ pixel density in \f$ v \f$ image direction.
  // Principal point
  VNL::VectorFixed<2> m_centre;  ///< Optical centre

  /// The standard deviation of image measurement noise
  double m_measurement_sd;
  
  VNL::MatrixFixed<3,3,double> m_C;    ///< The camera calibration matrix
  VNL::MatrixFixed<3,3,double> m_Cinv; ///< The inverse camera calibration matrix

  /** The most recent camera point that was projected. (Stored so that we can
  return the Jacobian for this transformation if it is asked for immediately
  afterwards). */
  VNL::VectorFixed<3> m_last_camera;
  VNL::VectorFixed<2> m_last_image_centred;
};

std::ostream& operator <<(std::ostream& os, const WideCamera& c);

#endif
