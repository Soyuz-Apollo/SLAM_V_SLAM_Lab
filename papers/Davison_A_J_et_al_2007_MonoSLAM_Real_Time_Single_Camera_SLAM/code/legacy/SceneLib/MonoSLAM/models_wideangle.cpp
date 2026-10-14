/*  SceneLib: software for real-time SLAM

    models_wideangle.cpp
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

#include "models_wideangle.h"
#include <VNL/trace.h>
#include <VNL/sample.h>

#include <Scene/exceptions.h>

#ifdef DEBUGDUMP
#undef DEBUGDUMP
#endif
#define DEBUGDUMP 0

using namespace std;

/**
Constructor.
@param cam The camera model to use
@param SD_image_filter_ See SD_image_filter property
@param SD_image_simulation_ See SD_image_simulation property
@param MAXIMUM_LENGTH_RATIO_ See MAXIMUM_LENGTH_RATIO property
@param MAXIMUM_ANGLE_DIFFERENCE_ See MAXIMUM_ANGLE_DIFFERENCE property
@param IMAGE_SEARCH_BOUNDARY_ See IMAGE_SEARCH_BOUNDARY property
@param motion_model The motion model describing the robot motion. This must be derived from ThreeD_Motion_Model
**/
Wide_Camera_Point_Feature_Measurement_Model::
Wide_Camera_Point_Feature_Measurement_Model(
  Motion_Model *motion_model,
  const WideCamera& cam,
  double MAXIMUM_LENGTH_RATIO_,
  double MAXIMUM_ANGLE_DIFFERENCE_,
  double IMAGE_SEARCH_BOUNDARY_,
  double SD_IMAGE_SIMULATION_)
  : MAXIMUM_LENGTH_RATIO(MAXIMUM_LENGTH_RATIO_),
    MAXIMUM_ANGLE_DIFFERENCE(MAXIMUM_ANGLE_DIFFERENCE_),
    IMAGE_SEARCH_BOUNDARY(IMAGE_SEARCH_BOUNDARY_),
    SD_IMAGE_SIMULATION(SD_IMAGE_SIMULATION_),
    m_camera(cam)
{
  if(string(motion_model->motion_model_dimensionality_type) != 
                    string("THREED"))
  {
    ostringstream os;
    os << "Motion Model given to the Feature Measurement Model is of type \""
       << motion_model->motion_model_dimensionality_type 
       << "\", not type \"THREED\"." << endl;
    throw Scene::InitialisationError(os.str());
  }
  threed_motion_model = dynamic_cast<ThreeD_Motion_Model*>(motion_model);
}


/**
Read the parameters of the camera from the settings.
*/
void Wide_Camera_Point_Feature_Measurement_Model::read_parameters(
  const Settings::Section& settings)
{
  string params = settings.get_entry("CameraParameters");
  if(params == "")
  {
    ostringstream error;
    error << "No CameraParameters entry found in initialisation file section ["
          << settings.Label() << "]." << endl;
    throw Scene::InitialisationError(error.str());
  }
  istringstream stream(params);
  stream >> m_camera;
}


// Model for single camera making point measurements
/**
Constructor.
@param cam The camera model to use
@param SD_image_filter_ See SD_image_filter property
@param SD_image_simulation_ See SD_image_simulation property
@param MAXIMUM_LENGTH_RATIO_ See MAXIMUM_LENGTH_RATIO property
@param MAXIMUM_ANGLE_DIFFERENCE_ See MAXIMUM_ANGLE_DIFFERENCE property
@param IMAGE_SEARCH_BOUNDARY_ See IMAGE_SEARCH_BOUNDARY property
@param motion_model The motion model describing the robot motion. This
must be derived from ThreeD_Motion_Model
@param feature_type The name for this type of feature measurement model
**/
Fully_Init_Wide_Point_Feature_Measurement_Model::
Fully_Init_Wide_Point_Feature_Measurement_Model(
      Motion_Model *motion_model,
      const WideCamera& cam,
      double MAXIMUM_LENGTH_RATIO_,
      double MAXIMUM_ANGLE_DIFFERENCE_,
      double IMAGE_SEARCH_BOUNDARY_,
      double SD_IMAGE_SIMULATION_)
  : Fully_Initialised_Feature_Measurement_Model(
      2, 3, 3, motion_model, "CAMERA_WIDE_POINT", "THREED_POINT"),
    Wide_Camera_Point_Feature_Measurement_Model(
      motion_model, cam,
      MAXIMUM_LENGTH_RATIO_,
      MAXIMUM_ANGLE_DIFFERENCE_,
      IMAGE_SEARCH_BOUNDARY_,
      SD_IMAGE_SIMULATION_)
{
}

/** Destructor. Does nothing */
Fully_Init_Wide_Point_Feature_Measurement_Model::
~Fully_Init_Wide_Point_Feature_Measurement_Model()
{
}


/** Read the parameters of this feature measurement model from the settings.
Initialises the base classes by calling
Fully_Initialised_Feature_Measurement_Model::read_parameters() and
Wide_Camera_Point_Feature_Measurement_Model::read_parameters().*/
void Fully_Init_Wide_Point_Feature_Measurement_Model::read_parameters(
  const Settings& settings)
{
  // Initialise any base class parameters
  Fully_Initialised_Feature_Measurement_Model::read_parameters(settings);

  // Initialise my camera model
  const Settings::Section* section = settings.get_section(feature_type);
  if(section == 0)
  {
    cerr << "Warning: no [" << feature_type
         << "] section in the intialisation data. "
         "The feature measurement model will be initialised with the default "
         "camera parameters." << endl;
  }
  else
  {
    // Read the camera model. This will be the name of another section
    string camera = section->get_entry("Camera");
    if(camera == "")
    {
      cerr << "Warning: no Camera entry in the [" << feature_type
            << "] section in the intialisation data. "
            "The feature measurement model will be initialised with the "
            "default camera parameters." << endl;
    }
    else
    {
      section = settings.get_section(camera);
      if(section == 0)
      {
        cerr << "Warning: no [" << camera
            << "] section in the intialisation data. "
            "The feature measurement model will be initialised with the "
            " default camera parameters." << endl;
      }
      else
        Wide_Camera_Point_Feature_Measurement_Model::read_parameters(*section);
    }
  }
}


/**
In this case the graphics representation \f$ y_i^{graphics} \f$ and its
covariance is the same as the feature state \f$ y_i \f$ and covariance.
*/
void Fully_Init_Wide_Point_Feature_Measurement_Model::
func_yigraphics_and_Pyiyigraphics(const VNL::Vector<double> &yi,
                                  const VNL::Matrix<double> &Pyiyi)
{
  // The graphics representation is the same as the state 
  yigraphicsRES.Update(yi);
  PyiyigraphicsRES.Update(Pyiyi);
}


void Fully_Init_Wide_Point_Feature_Measurement_Model::
func_zeroedyigraphics_and_Pzeroedyigraphics(
  const VNL::Vector<double> &yi, const VNL::Vector<double> &xv,
  const VNL::Matrix<double> &Pxx,
  const VNL::Matrix<double> &Pxyi,
  const VNL::Matrix<double> &Pyiyi)
{
  threed_motion_model->func_xp(xv);

  // In this case (where the feature state is the same as the graphics
  // state) zeroedyigraphics is the same as zeroedyi
  func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(yi, 
    threed_motion_model->get_xpRES());
  zeroedyigraphicsRES.Update(zeroedyiRES);

  VNL::Matrix<double> dzeroedyigraphics_by_dxv = 
    dzeroedyi_by_dxpRES * threed_motion_model->get_dxp_by_dxvRES();

  PzeroedyigraphicsRES.Update(
    dzeroedyigraphics_by_dxv * Pxx * dzeroedyigraphics_by_dxv.Transpose() +
    dzeroedyi_by_dyiRES * Pxyi.Transpose() * 
    dzeroedyigraphics_by_dxv.Transpose() +
    dzeroedyigraphics_by_dxv * Pxyi * dzeroedyi_by_dyiRES.Transpose() +
    dzeroedyi_by_dyiRES * Pyiyi * dzeroedyi_by_dyiRES.Transpose()
    );
}


void Fully_Init_Wide_Point_Feature_Measurement_Model::
func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(
		const VNL::Vector<double> &yi, const VNL::Vector<double> &xp)
{
  // Extract cartesian and quaternion components of xp
  threed_motion_model->func_r(xp);
  threed_motion_model->func_q(xp);

  VW::Vector3D yWiminusrW = VW::Vector3D(yi) - threed_motion_model->get_rRES();

  VW::Quaternion qRW = threed_motion_model->get_qRES().Inverse();
  VNL::MatrixFixed<4,4,double> dqRW_by_dq = dqbar_by_dq();

  // Rotation RRW
  VW::RotationMatrix RRW = qRW.RotationMatrix();

  // Position of feature relative to robot in robot frame
  VW::Vector3D zeroedyi = RRW * yWiminusrW;
  zeroedyiRES.Update(zeroedyi.GetVNL3());

  // Now calculate Jacobians
  // dzeroedyi_by_dyi is RRW
  dzeroedyi_by_dyiRES.Update(RRW);

  // dzeroedyi_by_dxp has 2 partitions:
  // dzeroedyi_by_dr (3 * 3)
  // dzeroedyi_by_dq (3 * 4)
  VNL::MatrixFixed<3,3,double> dzeroedyi_by_dr = RRW;
  dzeroedyi_by_dr *= -1.0;

  VNL::MatrixFixed<3,4,double> dzeroedyi_by_dqRW =
    dRq_times_a_by_dq(qRW, yWiminusrW);
  VNL::MatrixFixed<3,4,double> dzeroedyi_by_dq =
    dzeroedyi_by_dqRW * dqRW_by_dq;

  dzeroedyi_by_dxpRES.Update(dzeroedyi_by_dr, 0, 0);
  dzeroedyi_by_dxpRES.Update(dzeroedyi_by_dq, 0, 3);
}


void Fully_Init_Wide_Point_Feature_Measurement_Model::
func_Ri(const VNL::Vector<double> &hi)
{
  RiRES = m_camera.MeasurementNoise(hi);
}

unsigned int Fully_Init_Wide_Point_Feature_Measurement_Model::
  visibility_test(const VNL::Vector<double> &xp, 
                  const VNL::Vector<double> &yi,
                  const VNL::Vector<double> &xp_orig,
                  const VNL::Vector<double> &hi)
{
  unsigned int cant_see_flag = 0;

  /* Test image boundaries */
  if ( hi(0) < 0.0 + IMAGE_SEARCH_BOUNDARY || 
       hi(0) >  (double) (m_camera.ImageWidth() - 1 - IMAGE_SEARCH_BOUNDARY)) {
    cant_see_flag |= LEFT_RIGHT_FAIL;
    if (DEBUGDUMP) cerr << "Left / right fail." << endl;
  }
  if ( hi(1) < 0.0 + IMAGE_SEARCH_BOUNDARY || 
       hi(1) >  (double) (m_camera.ImageHeight() - 1 - IMAGE_SEARCH_BOUNDARY)) {
    cant_see_flag |= UP_DOWN_FAIL;
    if (DEBUGDUMP) cerr << "Up / down fail." << endl;
  }

  /* Do tests on length and angle of predicted view */

  // hLWi is current predicted vector from head to feature in 
  // world frame

  // This function gives relative position of feature
  func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(yi, xp);


  // Test the feature's not behind the camera (because projection
  // may do strange things)
  if (zeroedyiRES(2) <= 0) {
    cant_see_flag |= BEHIND_CAMERA_FAIL;
    if (DEBUGDUMP) cerr << "Behind camera fail." << endl;
  }
    

  threed_motion_model->func_q(xp);
  VW::RotationMatrix RWR = threed_motion_model->get_qRES().RotationMatrix();

  VW::Vector3D hLWi = RWR * VW::Vector3D(zeroedyiRES);

  // hLWi_orig is vector from head to feature in world frame
  // WHEN THAT FEATURE WAS FIRST MEASURED: i.e. when the image
  // patch was saved
  func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(yi, xp_orig);

  threed_motion_model->func_q(xp_orig);
  VW::RotationMatrix RWR_orig = threed_motion_model->
                                              get_qRES().RotationMatrix();
  
  VW::Vector3D hLWi_orig = RWR_orig * VW::Vector3D(zeroedyiRES);

  // Compare hLWi and hLWi_orig for length and orientation
  double mod_hLWi = hLWi.Norm();
  double mod_hLWi_orig = hLWi_orig.Norm();

  double length_ratio = mod_hLWi / mod_hLWi_orig;
  if ( length_ratio > MAXIMUM_LENGTH_RATIO || 
       length_ratio < (1.0 / MAXIMUM_LENGTH_RATIO) ) {
    cant_see_flag |= DISTANCE_FAIL;
    if (DEBUGDUMP) cerr << "Distance fail." << endl;
  }

  double dot_prod = hLWi * hLWi_orig;

  double angle = acos(dot_prod / (mod_hLWi * mod_hLWi_orig));
  angle = (angle >= 0.0 ? angle : -angle);  // Make angle positive
  if (angle > MAXIMUM_ANGLE_DIFFERENCE) {
    cant_see_flag |= ANGLE_FAIL;
    if (DEBUGDUMP) cerr << "Angle fail." << endl;
  }

  return cant_see_flag;   // 0 if OK, otherwise error code
}

double Fully_Init_Wide_Point_Feature_Measurement_Model::
selection_score(const VNL::Matrix<double> &Si)
{
  // Return the trace of the innovation covariance
  return VNL::Trace(Si);
}


void Fully_Init_Wide_Point_Feature_Measurement_Model::
func_hi_and_dhi_by_dxp_and_dhi_by_dyi(const VNL::Vector<double> &yi, 
				      const VNL::Vector<double> &xp)
{
  // This function gives relative position of feature: also call this hR
  // (vector from camera to feature in robot frame)
  func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(yi, xp);

  // Project this into the image using our camera
  hiRES = m_camera.Project(get_zeroedyiRES());

  // And ask the camera what the Jacobian of this projection was
  VNL::MatrixFixed<2,3> dhid_by_dzeroedyi = m_camera.ProjectionJacobian();

  // Form the required Jacobians
  dhi_by_dxpRES = dhid_by_dzeroedyi * dzeroedyi_by_dxpRES;
  dhi_by_dyiRES = dhid_by_dzeroedyi * dzeroedyi_by_dyiRES;
}



void Fully_Init_Wide_Point_Feature_Measurement_Model::
func_nui(const VNL::Vector<double> &hi, const VNL::Vector<double> &zi)
{
  nuiRES.Update(zi - hi);
}

void Fully_Init_Wide_Point_Feature_Measurement_Model::
func_hi_noisy(const VNL::Vector<double> &yi_true, 
              const VNL::Vector<double> &xp_true)
{
  func_hi_and_dhi_by_dxp_and_dhi_by_dyi(yi_true, xp_true);

  hi_noisyRES.Put(0, VNL::SampleNormal(hiRES(0), SD_IMAGE_SIMULATION));
  hi_noisyRES.Put(1, VNL::SampleNormal(hiRES(1), SD_IMAGE_SIMULATION));
}






/**
Constructor.
@param cam The camera model to use
@param SD_image_filter_ See SD_image_filter property
@param SD_image_simulation_ See SD_image_simulation property
@param MAXIMUM_LENGTH_RATIO_ See MAXIMUM_LENGTH_RATIO property
@param MAXIMUM_ANGLE_DIFFERENCE_ See MAXIMUM_ANGLE_DIFFERENCE property
@param IMAGE_SEARCH_BOUNDARY_ See IMAGE_SEARCH_BOUNDARY property
@param motion_model The motion model describing the robot motion. This must be
derived from ThreeD_Motion_Model
@param feature_type The name for this type of feature measurement model
@param fully_init_f_m_m The type of feature measurement model to convert to when
the feature is fully initialised.
**/
Line_Init_Wide_Point_Feature_Measurement_Model::
Line_Init_Wide_Point_Feature_Measurement_Model(
  Motion_Model *motion_model,
  Feature_Measurement_Model *fully_init_f_m_m,
  const WideCamera& cam, 
  double MAXIMUM_LENGTH_RATIO_,
  double MAXIMUM_ANGLE_DIFFERENCE_,
  double IMAGE_SEARCH_BOUNDARY_,
  double SD_IMAGE_SIMULATION_) :
    Partially_Initialised_Feature_Measurement_Model(
      2, 6, 6, motion_model, "CAMERA_WIDE_LINE_INIT",
      "THREED_SEMI_INFINITE_LINE", 1, fully_init_f_m_m),
    Wide_Camera_Point_Feature_Measurement_Model(
      motion_model, cam,
      MAXIMUM_LENGTH_RATIO_,
      MAXIMUM_ANGLE_DIFFERENCE_,
      IMAGE_SEARCH_BOUNDARY_,
      SD_IMAGE_SIMULATION_)
{
}

Line_Init_Wide_Point_Feature_Measurement_Model::
~Line_Init_Wide_Point_Feature_Measurement_Model()
{
}

// Redefined virtual functions
void Line_Init_Wide_Point_Feature_Measurement_Model::read_parameters(
  const Settings& settings)
{
  // Initialise any base class parameters
  Partially_Initialised_Feature_Measurement_Model::read_parameters(settings);

  // Initialise my camera model
  const Settings::Section* section = settings.get_section(feature_type);
  if(section == 0)
  {
    cerr << "Warning: no [" << feature_type
         << "] section in the intialisation data. "
         "The feature measurement model will be initialised with the default "
         "camera parameters." << endl;
  }
  else
  {
    // Read the camera model. This will be the name of another section
    string camera = section->get_entry("Camera");
    if(camera == "")
    {
      cerr << "Warning: no Camera entry in the [" << feature_type
            << "] section in the intialisation data. "
            "The feature measurement model will be initialised with the "
            "default camera parameters." << endl;
    }
    else
    {
      section = settings.get_section(camera);
      if(section == 0)
      {
        cerr << "Warning: no [" << camera
            << "] section in the intialisation data. "
            "The feature measurement model will be initialised with the "
            " default camera parameters." << endl;
      }
      else
        Wide_Camera_Point_Feature_Measurement_Model::read_parameters(*section);
    }
  }
}


void Line_Init_Wide_Point_Feature_Measurement_Model::
  func_yigraphics_and_Pyiyigraphics(const VNL::Vector<double> &yi, 
                                    const VNL::Matrix<double> &Pyiyi)
{
  yigraphicsRES.Update(yi);
  PyiyigraphicsRES.Update(Pyiyi);
}

void Line_Init_Wide_Point_Feature_Measurement_Model::
func_zeroedyigraphics_and_Pzeroedyigraphics(
	   const VNL::Vector<double> &yi, const VNL::Vector<double> &xv,
	   const VNL::Matrix<double> &Pxx,
	   const VNL::Matrix<double> &Pxyi,
	   const VNL::Matrix<double> &Pyiyi)
{
  threed_motion_model->func_xp(xv);

  // In this case (where the feature state is the same as the graphics
  // state) zeroedyigraphics is the same as zeroedyi
  func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(yi, 
				       	   threed_motion_model->get_xpRES());
  zeroedyigraphicsRES.Update(zeroedyiRES);

  VNL::Matrix<double> dzeroedyigraphics_by_dxv = 
    dzeroedyi_by_dxpRES * threed_motion_model->get_dxp_by_dxvRES();

  PzeroedyigraphicsRES.Update(
    dzeroedyigraphics_by_dxv * Pxx * dzeroedyigraphics_by_dxv.Transpose() +
    dzeroedyi_by_dyiRES * Pxyi.Transpose() * 
                                        dzeroedyigraphics_by_dxv.Transpose() +
    dzeroedyigraphics_by_dxv * Pxyi * dzeroedyi_by_dyiRES.Transpose() +
    dzeroedyi_by_dyiRES * Pyiyi * dzeroedyi_by_dyiRES.Transpose()
    );
}


void Line_Init_Wide_Point_Feature_Measurement_Model::
  func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(
    const VNL::Vector<double> &yi, const VNL::Vector<double> &xp)
{
  // Extract cartesian and quaternion components of xp
  threed_motion_model->func_r(xp);
  threed_motion_model->func_q(xp);

  // Extract ri and hhati components of yi = ypi
  func_ri(yi);
  func_hhati(yi);

  // ri part: transformation is just the same as in the normal point case
  // zeroedri = RRW(rWi - rW)

  // ri - r
  VW::Vector3D yWiminusrW =
    VW::Vector3D(riRES) - threed_motion_model->get_rRES();

  VW::Quaternion qRW = threed_motion_model->get_qRES().Inverse();
  VNL::MatrixFixed<4,4,double> dqRW_by_dq = dqbar_by_dq();

  // Rotation RRW
  VW::RotationMatrix RRW = qRW.RotationMatrix();

  // RRW(rWi - rW)
  VW::Vector3D zeroedri = RRW * yWiminusrW;

  // Now calculate Jacobians
  // dzeroedri_by_dri is RRW
  // dzeroedri_by_dhhati = 0
  VNL::MatrixFixed<3,3,double> dzeroedri_by_dri = RRW;

  // dzeroedyi_by_dxp:
  // dzeroedri_by_dr = -RRW
  // dzeroedri_by_dq = d_dq(RRW (ri - r))
  VNL::MatrixFixed<3,3,double> dzeroedri_by_dr = RRW * -1.0;

  VNL::MatrixFixed<3,4,double> dzeroedri_by_dqRW = 
                                    dRq_times_a_by_dq(qRW, yWiminusrW);
  VNL::MatrixFixed<3,4,double> dzeroedri_by_dq = 
                                    dzeroedri_by_dqRW * dqRW_by_dq;

  // Now for the hhati part (easier...)
  // zeroedhhati = RRW hhati
  VW::Vector3D zeroedhhati = RRW * hhatiRES;

  // Jacobians
  // dzeroedhhati_by_dr = 0
  // dzeroedhhati_by_dq = d_dq(RRW hhati)
  // dzeroedhhati_by_dhhati = RRW
  // dzeroedhhati_by_dri = 0
  VNL::MatrixFixed<3,4,double> dzeroedhhati_by_dqRW =
                  dRq_times_a_by_dq(qRW, hhatiRES);
  VNL::MatrixFixed<3,4,double> dzeroedhhati_by_dq =
                  dzeroedhhati_by_dqRW * dqRW_by_dq;
  VNL::MatrixFixed<3,3,double> dzeroedhhati_by_dhhati = RRW;

  // And put it all together
  zeroedyiRES.Update(zeroedri.GetVNL3(), 0);
  zeroedyiRES.Update(zeroedhhati.GetVNL3(), 3);

  //cout << "Line: zeroedri = " << zeroedri << "zeroedhhati = " << zeroedhhati;

  dzeroedyi_by_dxpRES.Fill(0.0);
  dzeroedyi_by_dxpRES.Update(dzeroedri_by_dr, 0, 0);
  dzeroedyi_by_dxpRES.Update(dzeroedri_by_dq, 0, 3);
  dzeroedyi_by_dxpRES.Update(dzeroedhhati_by_dq, 3, 3);

  dzeroedyi_by_dyiRES.Fill(0.0);
  dzeroedyi_by_dyiRES.Update(dzeroedri_by_dri, 0, 0);
  dzeroedyi_by_dyiRES.Update(dzeroedhhati_by_dhhati, 3, 3);
}

void Line_Init_Wide_Point_Feature_Measurement_Model::
func_Ri(const VNL::Vector<double> &hi)
{
  RiRES = m_camera.MeasurementNoise(hi);
}

unsigned int Line_Init_Wide_Point_Feature_Measurement_Model::
  visibility_test(const VNL::Vector<double> &, const VNL::Vector<double> &,
                  const VNL::Vector<double> &, const VNL::Vector<double> &)
{
  // Always visible for now
  return 0;
}

double Line_Init_Wide_Point_Feature_Measurement_Model::
selection_score(const VNL::Matrix<double> &)
{
  // Always measureable for now
  return 100000.0;
}

// Partial initialisation function
/**
Calculates the state vector \f$ \vct{y}_{pi} \f$ and Jacobians for a new
partially-initialised feature from an initial observation.
@param hi The initial measurement \f$ \vct{h}_i \f$, in this case the \f$(x,y)
\f$ image location of the feature.
@param xp The robot position state \f$ \vct{x}_p \f$ from which the initial
observation was made.
*/
void Line_Init_Wide_Point_Feature_Measurement_Model::
func_ypi_and_dypi_by_dxp_and_dypi_by_dhi_and_Ri(
         const VNL::Vector<double> &hi, const VNL::Vector<double> &xp)
{
  // Representation of a line here:
  // ypi = (rWi    )
  //       (hLhatWi)

  // Form the ray (in camera co-ordinates by unprojecting this image location
  VW::Vector3D hLRi = m_camera.Unproject(hi);
  
  if (DEBUGDUMP) cerr << "debug hLRi" << hLRi;

  // Form hLhatRi from hLRi
  // Normalise
  VW::Vector3D hLhatRi = hLRi;
  hLhatRi.Normalise();

  VNL::MatrixFixed<3,3,double> dhLhatRi_by_dhLRi = dvnorm_by_dv(hLRi); 

  if (DEBUGDUMP) cerr << "debug hLhatRi" << hLhatRi;

  // Now convert this into a direction in world co-ordinates by rotating
  // Form hLhatWi from hLhatRi
  // Rotate
  threed_motion_model->func_q(xp);
  VW::RotationMatrix RWR = threed_motion_model->get_qRES().RotationMatrix();
  VW::Vector3D hLhatWi(RWR * hLhatRi);

  if (DEBUGDUMP) cerr << "debug hLhatWi" << hLhatWi;

  // Extract rW from xp
  threed_motion_model->func_r(xp);

  // And form ypiRES
  ypiRES.Update(threed_motion_model->get_rRES().GetVNL3(), 0);
  ypiRES.Update(hLhatWi.GetVNL3(), 3);


  if(DEBUGDUMP) cerr << "debug ypiRES" << ypiRES;

  // Form Jacobians dypi_by_dxp and dypi_by_dhi

  // dypi_by_dxp = (drWi_by_dr     drWi_by_dq    )
  //               (dhLhatWi_by_dr dhLhatWi_by_dq)
  //             = (I              0             )
  //               (0              dhLhatWi_by_dq)

  // hLhatWi = RWR * hLhatRi
  // => dhLhatWi_by_dq = d/dq ( R(qWR) * hLhatRi)

  VNL::MatrixFixed<3,4,double> dhLhatWi_by_dq =
            dRq_times_a_by_dq(threed_motion_model->get_qRES(), hLhatRi);

  // Put dypi_by_dxp together
  dypi_by_dxpRES.Fill(0.0);
  dypi_by_dxpRES.Put(0, 0, 1.0);
  dypi_by_dxpRES.Put(1, 1, 1.0);
  dypi_by_dxpRES.Put(2, 2, 1.0);
  dypi_by_dxpRES.Update(dhLhatWi_by_dq, 3, 3);

  //  cout << "dypi_by_dxpRES" <<  dypi_by_dxpRES;


  // dypi_by_dhi = (drWi_by_dhi    )
  //               (dhLhatWi_by_dhi)
  //             = (0              )
  //               (dhLhatWi_by_dhi)

  // hLhatWi = RWR * hLhatRi
  // Need to work out derivative for this
  // dhLhatWi_by_dhi = RWR * dhLhatRi_by_dhLRi * dhLRi_by_dhi

  VNL::MatrixFixed<3,2> dhLhatWi_by_dhi =
    RWR * dhLhatRi_by_dhLRi * m_camera.UnprojectionJacobian();
  
  dypi_by_dhiRES.Fill(0.0);
  dypi_by_dhiRES.Update(dhLhatWi_by_dhi, 3, 0);

  //  cout << "dypi_by_dhiRES" <<  dypi_by_dhiRES;

  // And construct Ri
  func_Ri(hi);

  if (DEBUGDUMP) cerr << "func_ypi: hi = " << hi << endl
                      << "xp = " << xp << endl
                      << "ypiRES = " << ypiRES << endl;
}

/** Predict the image location \f$ \vct{h}_i = (x,y) \f$ for
partially-initialised feature with state \f$ \vct{y}_i \f$, given the current
camera location \f$ \vct{x}_p \f$ and the depth parameter \f$ \lambda \f$.
*/
void Line_Init_Wide_Point_Feature_Measurement_Model::
  func_hpi_and_dhpi_by_dxp_and_dhpi_by_dyi(const VNL::Vector<double> &yi, 
                                           const VNL::Vector<double> &xp,
                                           const VNL::Vector<double> &lambda)
{
  // This function gives relative position of feature: also call this hR
  // (vector from camera to feature in robot frame)
  func_zeroedyi_and_dzeroedyi_by_dxp_and_dzeroedyi_by_dyi(yi, xp);


  // Parameters of vector hLR from camera to feature in robot frame
  // hLR = zeroedri + lambda * zeroedhhati
  // Calculate the vector from the camera to the feature given the current
  // lambda
  VNL::VectorFixed<3> hLR =
    zeroedyiRES.Extract(3,0) + lambda[0] * zeroedyiRES.Extract(3,3);

  // Project this into the image
  hpiRES = m_camera.Project(hLR);

  // What is the Jacobian of this projection?
  VNL::MatrixFixed<2,3> dhpi_by_dhLRi = m_camera.ProjectionJacobian();

  // Calculate the required result Jacobians

  // Now how the vector to the feature depends on the parameterised line
  // (this is a function of lambda)
  double b[18] = {1.0, 0.0, 0.0, lambda(0), 0.0, 0.0,
                  0.0, 1.0, 0.0, 0.0, lambda(0), 0.0,
                  0.0, 0.0, 1.0, 0.0, 0.0, lambda(0)};
  VNL::MatrixFixed<3,6> dhLRi_by_dzeroedyi(b);
  
  dhpi_by_dxpRES = dhpi_by_dhLRi * dhLRi_by_dzeroedyi * dzeroedyi_by_dxpRES;
  dhpi_by_dyiRES = dhpi_by_dhLRi * dhLRi_by_dzeroedyi * dzeroedyi_by_dyiRES;
  
  if (DEBUGDUMP) cerr << "func_hpi: yi = " << yi << endl
                      << "xp = " << xp << endl
                      << "lambda = " << lambda << endl
                      << "hpiRES = " << hpiRES << endl;

}


// Conversion function: how to turn partially initialised feature ypi
// into fully initialised one yfi
void Line_Init_Wide_Point_Feature_Measurement_Model::
func_yfi_and_dyfi_by_dypi_and_dyfi_by_dlambda(
            const VNL::Vector<double> &ypi, const VNL::Vector<double> &lambda)
{
  // Simple: lambda is a scalar
  // yfi = ri + lambda hhati

  func_ri(ypi);
  func_hhati(ypi);

  double a[3] = {riRES.GetX() + lambda(0) * hhatiRES.GetX(),
                 riRES.GetY() + lambda(0) * hhatiRES.GetY(),
                 riRES.GetZ() + lambda(0) * hhatiRES.GetZ()};
  yfiRES.CopyIn(a); 

  double b[18] = {1.0, 0.0, 0.0, lambda(0), 0.0, 0.0,
                  0.0, 1.0, 0.0, 0.0, lambda(0), 0.0,
                  0.0, 0.0, 1.0, 0.0, 0.0, lambda(0)};
  dyfi_by_dypiRES.CopyIn(b);

  dyfi_by_dlambdaRES.Put(0, 0, hhatiRES.GetX());
  dyfi_by_dlambdaRES.Put(1, 0, hhatiRES.GetY());
  dyfi_by_dlambdaRES.Put(2, 0, hhatiRES.GetZ());
}

void Line_Init_Wide_Point_Feature_Measurement_Model::
func_ri(const VNL::Vector<double> &ypi)
{
  riRES.SetVNL3(ypi.Extract(3, 0));
}

void Line_Init_Wide_Point_Feature_Measurement_Model::
func_hhati(const VNL::Vector<double> &ypi)
{
  hhatiRES.SetVNL3(ypi.Extract(3, 3));
}


