/*  SceneLib: software for real-time SLAM

    models_wideangle.h
    Copyright (C) 2005 University of Oxford

    Author
    Andrew Davison
    ajd@doc.ic.ac.uk

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

/// Model for wide angle camera making point measurements

// Frames of reference:
// W: world frame
// R: robot (= camera) frame

// Spatial vectors: satisfy yi = r + hLi
// r:   robot spatial position vector
// hLi: spatial measurement vector
// yi:  feature position vector

// Spatial vectors with frame label is component vector in that frame

// Rotation matrices: RAB transforms the component representation of a 
// spatial vector from one frame to the other such that
// vA = RAB * vB

// Special vectors: are not real vectors but lists of parameters
// xp: robot position vector = (rW )
//                             (qWR)
// hi:  measurement vector in image coordinates = (u)
//                                                (v)
// ypi: partially initialised feature position vector = (rWi    )
//                                                      (hLhatWi)
//      This represents the position of a line, which is all we know
//      about the position of a feature after only one measurement.
//      Points on the line are described by rWi + constant * hLhatWi
//      rWi is the camera position when the the feature was observed
//      hLhatWi is the normalised direction vector lying along the line

#ifndef _monoslam_models_wideangle_h_
#define _monoslam_models_wideangle_h_

#include "Scene/models_base.h"
#include "widecamera.h"

/**
@addtogroup gMonoSLAM
@{
*/

const double WIDE_MAXIMUM_LENGTH_RATIO = 2.0;
const double WIDE_MAXIMUM_ANGLE_DIFFERENCE = M_PI * 45.0 / 180.0;
const double WIDE_IMAGE_SEARCH_BOUNDARY = 20.0;
const double WIDE_SD_IMAGE_SIMULATION = 1.0;

/**
Base class for feature measurement models that use a camera.
**/
class Camera_Feature_Measurement_Model
{
public:
  /// Constructor does nothing.
  Camera_Feature_Measurement_Model() {}
  /// Destructor does nothing.
  virtual ~Camera_Feature_Measurement_Model() {}

public:
  /// Returns the camera model. The camera is not stored in this class
  /// since derived classes might want their own specific camera models,
  /// but they must provide this function to return the camera to the user.
  virtual const WideCamera& get_camera() const = 0;
};


/** 
Base class for point feature measurement models that use a wide-angle camera.
**/
class Wide_Camera_Point_Feature_Measurement_Model :
  public Camera_Feature_Measurement_Model
{
public:
  Wide_Camera_Point_Feature_Measurement_Model(
    Motion_Model* model, const WideCamera& cam,
    double MAXIMUM_LENGTH_RATIO_,
    double MAXIMUM_ANGLE_DIFFERENCE,
    double IMAGE_SEARCH_BOUNDARY_,
    double SD_IMAGE_SIMULATION);
  
  void read_parameters(const Settings::Section& settings);

public:
  /// Returns the camera model.
  const WideCamera& get_camera() const {return m_camera;}
  
public:
  /** For visibility tests: we expect to be able to measure feature if
  distance to feature / distance when initialised is in the range
  1/MAXIMUM_LENGTH_RATIO to MAXIMUM_LENGTH_RATIO. There is also a test
  on MAXIMUM_ANGLE_DIFFERENCE, and both need to pass.*/
  const double MAXIMUM_LENGTH_RATIO;
  /** For visibility tests: we expect to be able to measure feature if
      the absolute angle between current viewpoint vector and the
      initialisation viewpoint vector is less than
      MAXIMUM_ANGLE_DIFFERENCE. There is also a test on
      MAXIMUM_LENGTH_RATIO, and both need to pass. */
  const double MAXIMUM_ANGLE_DIFFERENCE;

  /// Don't try to search this close to the edge of the image
  const double IMAGE_SEARCH_BOUNDARY;

  /// The measurement standard deviation to use for simulations
  const double SD_IMAGE_SIMULATION;

  /// The return flag from visibility_test() ORs these together
  /// to indicate the reasons for failure.
  enum visibility{LEFT_RIGHT_FAIL = 1,
                  UP_DOWN_FAIL = 2,
                  DISTANCE_FAIL = 4,
                  ANGLE_FAIL = 8,
                  BEHIND_CAMERA_FAIL = 16};

protected:
  WideCamera m_camera; ///< The camera model

  /// This class assumes 3D motion models. This is checked in the constructor,
  /// cast to a ThreeD_Motion_Model, and set at that point.

  ThreeD_Motion_Model *threed_motion_model;
};




/**
Model for a wide-angle camera making point measurements. This derives from
Fully_Initialised_Feature_Measurement_Model, registering a feature graphics
type of <code>THREED_POINT</code>. The feature state \f$ y_i = (x,y,z) \f$, i.
e. a three-vector giving the 3D world position of the feature. The measurement
state \f$ h_i = (u,v) \f$ is the image location of the feature.
*/
class Fully_Init_Wide_Point_Feature_Measurement_Model : 
  public Fully_Initialised_Feature_Measurement_Model,
  public Wide_Camera_Point_Feature_Measurement_Model 
{
public:
  /// Constructor.
  Fully_Init_Wide_Point_Feature_Measurement_Model(
    Motion_Model *motion_model,
    const WideCamera& cam = WideCamera(),
    const double MAXIMUM_LENGTH_RATIO_ = WIDE_MAXIMUM_LENGTH_RATIO,
    const double MAXIMUM_ANGLE_DIFFERENCE_ = WIDE_MAXIMUM_ANGLE_DIFFERENCE,
    const double IMAGE_SEARCH_BOUNDARY_ = WIDE_IMAGE_SEARCH_BOUNDARY,
    const double SD_IMAGE_SIMULATION = WIDE_SD_IMAGE_SIMULATION);

  ~Fully_Init_Wide_Point_Feature_Measurement_Model();

  // Redefined virtual functions from Feature_Measurement_Model
  void read_parameters(const Settings& settings);
  
  void func_yigraphics_and_Pyiyigraphics(const VNL::Vector<double> &yi,
                                         const VNL::Matrix<double> &Pyiyi);

  void func_zeroedyigraphics_and_Pzeroedyigraphics(
    const VNL::Vector<double> &yi, const VNL::Vector<double> &xv,
    const VNL::Matrix<double> &Pxx,
    const VNL::Matrix<double> &Pxyi,
    const VNL::Matrix<double> &Pyiyi);

  void func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(
    const VNL::Vector<double> &yi, const VNL::Vector<double> &xp);

  void func_Ri(const VNL::Vector<double> &hi);

  unsigned int visibility_test(const VNL::Vector<double> &xp, 
                               const VNL::Vector<double> &yi,
                               const VNL::Vector<double> &xp_orig,
                               const VNL::Vector<double> &hi);

  double selection_score(const VNL::Matrix<double> &Si);

  // Redefined virtual functions from 
  // Fully_Initialised_Feature_Measurement_Model
  void func_hi_and_dhi_by_dxp_and_dhi_by_dyi(const VNL::Vector<double> &yi, 
                                             const VNL::Vector<double> &xp);


  void func_nui(const VNL::Vector<double> &hi, const VNL::Vector<double> &zi);

  void func_hi_noisy(const VNL::Vector<double> &yi_true, 
                     const VNL::Vector<double> &xp_true);
};






/** 
Feature measurement model for the partially-initialised stage of point.
When we've only seen a feature once we initialise the 3D ray it lies on
as a SLAM feature. A partially initialised feature has a state
\f[
  \vct{y}_{pi} = \evct{\vct{r}_i^W \\ \hat{\vct{h}}_{Li}^W}
\f]
where \f$ \vct{r}_i^W = \vct{x}_P^{orig} \f$ is the camera position when the
feature was observed, and \f$ \hat{\vct{h}}_{Li}^W \f$ is the normalised
direction vector towards the feature. The 3D ray that the feature lies on is
defined by
\f[
  \vct{y}_i = \vct{r}_i^W + \lambda \hat{\vct{h}}_{Li}^W
\f]
with \f$ \lambda \f$ being the free parameter which defines the depth of the
feature.
**/
class Line_Init_Wide_Point_Feature_Measurement_Model : 
  public Partially_Initialised_Feature_Measurement_Model,
  public Wide_Camera_Point_Feature_Measurement_Model
{
 public:
  // Constructor
  Line_Init_Wide_Point_Feature_Measurement_Model(
    Motion_Model *motion_model,
    Feature_Measurement_Model *fully_init_f_m_m,
    const WideCamera& cam = WideCamera(),
    double MAXIMUM_LENGTH_RATIO = WIDE_MAXIMUM_LENGTH_RATIO,
    double MAXIMUM_ANGLE_DIFFERENCE = WIDE_MAXIMUM_ANGLE_DIFFERENCE,
    double IMAGE_SEARCH_BOUNDARY = WIDE_IMAGE_SEARCH_BOUNDARY,
    double SD_IMAGE_SIMULATION = WIDE_SD_IMAGE_SIMULATION
);

  ~Line_Init_Wide_Point_Feature_Measurement_Model();

  // Redefined virtual functions from Feature_Measurement_Model
  void read_parameters(const Settings& settings);

  void func_yigraphics_and_Pyiyigraphics(const VNL::Vector<double> &yi,
                                         const VNL::Matrix<double> &Pyiyi);

  void func_zeroedyigraphics_and_Pzeroedyigraphics(
    const VNL::Vector<double> &yi, const VNL::Vector<double> &xv,
    const VNL::Matrix<double> &Pxx,
    const VNL::Matrix<double> &Pxyi,
    const VNL::Matrix<double> &Pyiyi);

  void func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(
    const VNL::Vector<double> &yi, const VNL::Vector<double> &xp);

  void func_Ri(const VNL::Vector<double> &hi);

  unsigned int visibility_test(const VNL::Vector<double> &xp, 
                               const VNL::Vector<double> &yi,
                               const VNL::Vector<double> &xp_orig,
                               const VNL::Vector<double> &hi);

  double selection_score(const VNL::Matrix<double> &Si);

  // Redefined virtual functions from 
  // Partially_Initialised_Fully_Initialised_Feature_Measurement_Model
  void func_ypi_and_dypi_by_dxp_and_dypi_by_dhi_and_Ri(
         const VNL::Vector<double> &hi, const VNL::Vector<double> &xp);

  void func_hpi_and_dhpi_by_dxp_and_dhpi_by_dyi(
    const VNL::Vector<double> &yi, 
    const VNL::Vector<double> &xp,
    const VNL::Vector<double> &lambda);

  void func_yfi_and_dyfi_by_dypi_and_dyfi_by_dlambda(
    const VNL::Vector<double> &ypi, const VNL::Vector<double> &lambda);

  // Special functions: extract r and hhat parts of line
  void func_ri(const VNL::Vector<double> &ypi);
  void func_hhati(const VNL::Vector<double> &ypi);

 protected:
  // Splitting out the ri and hhati components of ypi
  VW::Vector3D riRES, hhatiRES;
};

/**
@}
*/

#endif
