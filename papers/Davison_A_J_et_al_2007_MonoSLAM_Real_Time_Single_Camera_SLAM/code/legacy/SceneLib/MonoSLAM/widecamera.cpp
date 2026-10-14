/*  SceneLib: software for real-time SLAM

    widecamera.cpp
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


#include "widecamera.h"
 

const unsigned int DEFAULT_IMAGE_WIDTH = 320;
const unsigned int DEFAULT_IMAGE_HEIGHT = 240;
const double DEFAULT_FKU = 195;//160;//195.0;
const double DEFAULT_FKV = 195;//160;//195.0;
const double DEFAULT_U0 = 162;//162.0;
const double DEFAULT_V0 = 125;//125.0;
const double DEFAULT_K1 = 0.000009;
const double DEFAULT_MEASUREMENT_SD = 1.0;

/**
Default constructor. If this constructor is used, read_parameters() should be
used to set the parameters.
**/
WideCamera::WideCamera()  :
  m_image_width(DEFAULT_IMAGE_WIDTH),
  m_image_height(DEFAULT_IMAGE_WIDTH),
  m_Kd1(DEFAULT_K1),
  m_Fku(DEFAULT_FKU),
  m_Fkv(DEFAULT_FKV),
  m_measurement_sd(DEFAULT_MEASUREMENT_SD)
{
  m_centre[0] = DEFAULT_U0;
  m_centre[1] = DEFAULT_V0;
  
  MakeProjectionMatrices();
}

/**
Constructor.
**/
WideCamera::WideCamera(int w, int h, double Kd1, double Fku, double Fkv,
               double U0, double V0, double measurement_sd)  :
  m_image_width(w),
  m_image_height(h),
  m_Kd1(Kd1),
  m_Fku(Fku),
  m_Fkv(Fkv),
  m_measurement_sd(measurement_sd)
{
  m_centre[0] = U0;
  m_centre[1] = V0;

  MakeProjectionMatrices();
}

/**
Form the projection matrix \f$ \mat{C} \f$ and its inverse.
\f[
\begin{aligned}
\mat{C} &= \emat{-F_{ku} & 0 & U_0 \\
                 0 & -F_{kv} & V_0 \\
                 0 & 0 & 1}
\mat{C}^{-1} &= \emat{-\frac{1}{F_{ku}} & 0 & \frac{U_0}{F_{ku}} \\
                 0 & -\frac{1}{F_{kv}} & \frac{V_0}{F_{kv}} \\
                 0 & 0 & 1}
\end{aligned}
\f]
*/
void WideCamera::MakeProjectionMatrices()
{
  // Camera calibration matrix and inverse
  double m[9] = {-m_Fku,  0.0,  m_centre[0],
                  0.0, -m_Fkv,  m_centre[1],
                  0.0,  0.0, 1.0};
  m_C.CopyIn(m);
  double n[9] = {-1.0 / m_Fku,   0.0,        m_centre[0] / m_Fku,
                  0.0,        -1.0 / -m_Fkv,  m_centre[1] / m_Fkv,
                  0.0,         0.0,        1.0};
  m_Cinv.CopyIn(n);
}

/**
Project a point from an ideal (Euclidean) camera frame into image co-ordinates
with radial distortion.

This first transforms the input position vector \f$ \vct{y} \f$ (relative to
the camera position) into an undistorted image location \f$ \vct{u}_c \f$
(relative to an origin at the optical centre)
\f[
\vct{u}_c = \evct{u_c \\ v_c} = \evct{-F_{ku} y_x / y_z \\ -F_{kv} y_y / y_z} =
  \emat{-F_{ku} & 0 \\ 0 & -F_{kv}}\evct{y_x/y_z \\ y_y/y_z}
\f]
Then radial distortion is applied to give the final image location \f$ h \f$
(with the origin back in the normal location of of the top left of the image).
\f[
\vct{h} = \frac{\vct{u}_c}{\sqrt{1 + 2 k_1 |\vct{u}_c|^2}} + \evct{u_0 \\ v_0}
\f]
**/
VNL::VectorFixed<2> WideCamera::Project(const VNL::VectorFixed<3>& camera)
{
  // Remember this position in case we are asked about the Jacobians of this
  // transformation
  m_last_camera = camera;
  
  // First do perspective projection
  // This turns the position vector into an undistorted image location (with the
  // origin at the centre)
  // Use -Fkuv to swap the image co-ordinates (0,0) at the top to
  // camera co-ordinates (0,0) at the bottom

  VNL::VectorFixed<2> imagepos_centred;
  imagepos_centred[0] = -m_Fku * camera[0] / camera[2];
  imagepos_centred[1] = -m_Fkv * camera[1] / camera[2];

  m_last_image_centred = imagepos_centred;

  // 1 distortion coefficient model
  const double radius2 = imagepos_centred.SquaredMagnitude();
  double factor = sqrt(1 + 2 * m_Kd1 * radius2);
  return imagepos_centred / factor + m_centre;
}

/**
Project a point from image co-ordinates into the ideal (Euclidean) camera frame.

The origin for the image point \f$ \vct{h} \f$ is moved to the optical centre
and the inverse distortion applied to give the undistorted location
\f$ \vct{u}_c \f$:
\f[
\begin{aligned}
\vct{u}_0 &= \evct{u_0 \\ v_0} \\
\vct{u}_c = \evct{u_c \\ v_c} &=
  \frac{\vct{h} - \vct{u}_0}{\sqrt{1 - 2 k_1 |\vct{h} - \vct{u}_0|^2}}
\end{aligned}
\f]
This is then unprojected to give the world location
\f$ \vct{y} = \evct{y_x & y_y & 1}^T \f$ in homogeneous co-ordinates.
\f[
\vct{y} = \evct{\frac{1}{-F_{ku}} u_c \\ \frac{1}{-F_{kv}} v_c \\ 1}
\f]
**/
VNL::VectorFixed<3> WideCamera::Unproject(const VNL::VectorFixed<2>& image)
{
  VNL::VectorFixed<2> centred;
  centred = image - m_centre;

  m_last_image_centred = centred;

  const double radius2 = centred.SquaredMagnitude();
  double factor = sqrt(1 - 2 * m_Kd1 * radius2);

  VNL::VectorFixed<2> undistorted = centred / factor;

  VNL::VectorFixed<3> camera;
  
  camera[0] = undistorted[0] / -m_Fku;
  camera[1] = undistorted[1] / -m_Fkv;
  camera[2] = 1.0;
  
  return camera;
}

/** Calculate the Jacobian \f$ \partfrac{\vct{h}}{\vct{y}} \f$ for the
Project() operation, for the most recent point that was projected.

This is calculated in two stages:
\f[
\begin{aligned}
\partfrac{ \vct{u} }{ \vct{y} } &= \emat{
  \frac{ -f_{ku} }{ y_z } & 0 & \frac{ f_{ku} y_x }{ y_z^2 } \\
  0 & \frac{ -f_{kv} }{ y_z } & \frac{ f_{kv} y_y }{ y_z^2 } } \\
\partfrac{\vct{h}}{\vct{u}} &= \emat{
    -\frac{ 2 u_c^2 k_1 }{ f^{\frac{3}{2}} } +
      \frac{ 1 }{ f^{\frac{1}{2}} } &
    -\frac{ 2 u_c v_c k_1 }{ f^{\frac{3}{2}} } \\
    -\frac{ 2 v_c u_c k_1 }{ f^{\frac{3}{2}} } &
    -\frac{ 2 v_c^2 k_1 }{ f^{\frac{3}{2}} } +
      \frac{ 1 }{ f^{\frac{1}{2}} } } =
  -\frac{2 k_1}{f^\frac{3}{2}} \vct{u}_c \vct{u}_c^T +
    \emat{\frac{1}{\sqrt{f}} & 0 \\
    0 & \frac{1}{\sqrt{f}}}
\end{aligned}
\f]
where \f$ f = 1 + 2 k_1 |\vct{u}_c|^2 \f$. The final Jacobian is then given by
combining these two
\f[
\partfrac{\vct{h}}{\vct{y}} =
  \partfrac{\vct{h}}{\vct{u}} \partfrac{\vct{u}}{\vct{y}}
\f]
*/
VNL::MatrixFixed<2,3> WideCamera::ProjectionJacobian()
{
  // Jacobians
  // Normal image measurement
  const double fku_yz = m_Fku/m_last_camera[2];
  const double fkv_yz = m_Fkv/m_last_camera[2];
  const double a[6] =
    {-fku_yz,
      0.0,
      fku_yz * m_last_camera[0] / m_last_camera[2],
      0.0,
     -fkv_yz,
      fkv_yz * m_last_camera[1] / m_last_camera[2]};
  const VNL::MatrixFixed<2,3> du_by_dy(a);
  
  // Distortion model Jacobians
  // Generate the outer product matrix first
  VNL::MatrixFixed<2,2> dh_by_du =
    m_last_image_centred.AsColumn() * m_last_image_centred.AsRow();

  // this matrix is not yet dh_by_du, it is just
  // [ uc*uc  uc*vc ]
  // [ vc*uc  vc*vc ]
  // The trace of this matrix gives the magnitude of the vector
  const double radius2 = dh_by_du(0,0) + dh_by_du(1,1);
  // Calculate various constants to save typing
  const double distor = 1 + 2 * m_Kd1 * radius2;
  const double distor1_2 = sqrt(distor);
  const double distor3_2 = distor1_2 * distor;

  // Now form the proper dh_by_du by manipulating the outer product matrix
  dh_by_du *= -2 * m_Kd1 / distor3_2;
  dh_by_du(0,0) += (1/distor1_2);
  dh_by_du(1,1) += (1/distor1_2);

  return dh_by_du * du_by_dy;
}


/** Calculate the Jacobian \f$ \partfrac{\vct{y}}{\vct{h}} \f$ for the
Unproject() operation, for the most recent point that was projected.

This is calculated in two stages: the Jacobian between undistorted and
distorted image co-ordinates
\f[
\partfrac{\vct{u}}{\vct{h}} = \emat{
    \frac{ 2 u_c^2 k_1 }{ f^{\frac{3}{2}} } +
      \frac{ 1 }{ f^{\frac{1}{2}} } &
    \frac{ 2 u_c v_c k_1 }{ f^{\frac{3}{2}} } \\
    \frac{ 2 v_c u_c k_1 }{ f^{\frac{3}{2}} } &
    \frac{ 2 v_c^2 k_1 }{ f^{\frac{3}{2}} } +
      \frac{ 1 }{ f^{\frac{1}{2}} } } =
  \frac{2 k_1}{f^\frac{3}{2}} \vct{u}_c \vct{u}_c^T +
    \emat{\frac{1}{\sqrt{f}} & 0 \\
    0 & \frac{1}{\sqrt{f}}}
\f]
where \f$ f = 1 - 2 k_1 |\vct{u}_c|^2 \f$ and the Jacobian for the unprojection
operation between image rays and (undistorted, centred) image co-ordinates
\f[
\partfrac{ \vct{y} }{ \vct{u} } = \emat{
  -\frac{ 1 }{ f_{ku} } & 0 \\
  0 & -\frac{ 1}{ f_{kv}} \\
  0 & 0 } 
\f]
 The final Jacobian is then given by combining these two
\f[
\partfrac{\vct{y}}{\vct{h}} =
  \partfrac{\vct{y}}{\vct{u}} \partfrac{\vct{u}}{\vct{h}}
\f]
*/
VNL::MatrixFixed<3,2> WideCamera::UnprojectionJacobian()
{
  double a[6] = {-1/m_Fku, 0.0,
                 0.0, -1/m_Fkv,
                 0.0, 0.0};
  VNL::MatrixFixed<3,2,double> dy_by_du(a);
  
  // Generate the outer product matrix first
  VNL::MatrixFixed<2,2> du_by_dh =
    m_last_image_centred.AsColumn() * m_last_image_centred.AsRow();
  // this matrix is not yet du_by_dh, it is just
  // [ uc*uc  uc*vc ]
  // [ vc*uc  vc*vc ]
  // The trace of this matrix gives the magnitude of the vector
  const double radius2 = du_by_dh(0,0) + du_by_dh(1,1);
  // Calculate various constants to save typing
  double distor = 1 - 2 * m_Kd1 * radius2;
  double distor1_2 = sqrt(distor);
  double distor3_2 = distor1_2 * distor;

  // Now form the proper du_by_dh by manipulating the outer product matrix
  du_by_dh *= 2 * m_Kd1 / distor3_2;
  du_by_dh(0,0) += (1/distor1_2);
  du_by_dh(1,1) += (1/distor1_2);

  return dy_by_du * du_by_dh;
}

/** Calculate the image position measurement noise at this location.
@param h The image location

This is not constant across the image. It has the value of m_measurement_sd at
the centre, increasing with radial distance to 2*m_measurement_sd at the corners
*/
VNL::MatrixFixed<2,2> WideCamera::MeasurementNoise(
  const VNL::VectorFixed<2>& h)
{
  // Distance of point we are considering from image centre
  const double distance = (h - m_centre).Magnitude();
  const double max_distance = m_centre.Magnitude();
  const double ratio = distance / max_distance; // goes from 0 to 1

  const double SD_image_filter_to_use = m_measurement_sd * (1.0 + ratio);

  const double measurement_noise_variance =
    SD_image_filter_to_use * SD_image_filter_to_use;
  
  // RiRES is diagonal
  VNL::MatrixFixed<2,2> noise;
  noise.SetIdentity(measurement_noise_variance);

  return noise;
}


/** Write the camera parameters to a stream in the format
<code>Width Height U0 V0 Fku Fkv Kd1 SD</code>.
@param os The stream
@param x The camera
@ingroup MonoSLAM
*/
std::ostream& operator <<(std::ostream& os, const WideCamera& c)
{
   return os << c.ImageWidth() << " " << c.ImageHeight() << " "
             << c.U0() << " " << c.V0() << " "
             << c.Fku() << " " << c.Fkv() << " "
             << c.Kd1() << " " << c.MeasurementNoise(); 
}

/** Read camera paremeters from a stream, interpreting eight numbers as
the parameters in the format <code>Width Height U0 V0 Fku Fkv Kd1 SD</code>.
@param os The stream
@param c The camera
@ingroup MonoSLAM
*/
std::istream& operator >>(std::istream& is, WideCamera& c)
{
   return is >> c.m_image_width >> c.m_image_height
             >> c.m_centre[0] >> c.m_centre[1]
             >> c.m_Fku >> c.m_Fkv
             >> c.m_Kd1 >> c.m_measurement_sd;
}

