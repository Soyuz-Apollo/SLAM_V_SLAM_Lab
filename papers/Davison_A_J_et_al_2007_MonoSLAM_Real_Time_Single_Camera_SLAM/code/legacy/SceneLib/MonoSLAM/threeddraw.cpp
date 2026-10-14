/*  SceneLib: software for real-time SLAM

    threeddraw.cpp
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

#include <VNL/matlabprint.h>
#include <VW/Utils/timer.h>
#include <VWGL/Display/drawfirei.h>

#include "threeddraw.h"
#include "identifier_imagemono.h"

/**
@addtogroup gMonoSLAM
@{
*/

// q in state vector is qWR between world frame and Scene robot frame
// What we need to plot though uses GL object frame O
// Know qRO: pi rotation about y axis
// qWO = qWR * qRO
VW::Quaternion qRO (0.0, 1.0, 0.0, 0.0);


/**
Draw scene from external point of view xp_draw.
@param scene The SLAM map
@param threedtool The GL viewer to use
@param display_camera_flag Draw the camera?
@param display_camera_uncertainty_flag Draw the camera uncertainty?
@param display_3d_features_flag Draw the world features?
@param display_3d_feature_uncertainties_flag Draw the world feature
uncertainties
@param display_axes_flag Draw axes
@param display_annotations_flag Draw annotations
**/
void External3DDraw( const Scene_Single *scene,
                     VW::ThreeDToolGLCommon &threedtool,
                     const vector< VNL::VectorFixed<3, double> >&
                       trajectory_store,
                     const bool display_camera_flag,
                     const bool display_camera_uncertainty_flag,
                     const bool display_camera_trajectory_flag,
                     const bool display_3d_features_flag,
                     const bool display_3d_feature_uncertainties_flag,
                     const bool display_axes_flag,
                     const bool display_annotations_flag )
{

ThreeD_Motion_Model *threed_motion_model =
    (ThreeD_Motion_Model *) scene->get_motion_model();

  if (display_camera_flag)
  {
    threed_motion_model->func_xp(scene->get_xv());
    threed_motion_model->func_r(threed_motion_model->get_xpRES());
    threed_motion_model->func_q(threed_motion_model->get_xpRES());

    VW::Quaternion qWO = threed_motion_model->get_qRES() * qRO;
    VW::Vector3D r_local = threed_motion_model->get_rRES();

    DrawCamera(threedtool, r_local, qWO);
  }

  if (display_camera_uncertainty_flag)
  {
    // This is a convoluted way to pull out the camera position covariance
    // from the main state covariance but it is nice and generic
    threed_motion_model->func_xp(scene->get_xv());
    threed_motion_model->func_dxp_by_dxv(scene->get_xv());
    threed_motion_model->func_r(threed_motion_model->get_xpRES());
    threed_motion_model->func_dr_by_dxp(threed_motion_model->get_xpRES());

    VW::Vector3D local_r = threed_motion_model->get_rRES();

    VNL::Matrix<double> Pxpxp =
      threed_motion_model->get_dxp_by_dxvRES() * scene->get_Pxx() * 
                      threed_motion_model->get_dxp_by_dxvRES().Transpose();
    VNL::MatrixFixed<3,3,double> Prr = 
      threed_motion_model->get_dr_by_dxpRES() * Pxpxp * 
                 threed_motion_model->get_dr_by_dxpRES().Transpose();

    DrawCameraCovariance(threedtool, local_r, Prr);
  }

  if (display_camera_trajectory_flag)
  {
    DrawCameraTrajectory(threedtool, trajectory_store);
  }

  if (display_3d_features_flag || display_3d_feature_uncertainties_flag)
  {
    for (vector<Feature *>::const_iterator it = 
           scene->get_feature_list().begin();
         it != scene->get_feature_list().end(); it++)
    {

      (*it)->get_feature_measurement_model()->
        func_yigraphics_and_Pyiyigraphics((*it)->get_y(), (*it)->get_Pyy());

      SetFeatureColour(threedtool, (*it)->get_selected_flag(),
                       (*it)->get_successful_measurement_flag(),
                       scene->get_marked_feature_label() ==
                         int((*it)->get_label()));

      if (display_3d_features_flag)
      {
        if (strcmp((*it)->get_feature_measurement_model()->
          feature_graphics_type, "THREED_POINT") == 0)
        {
          DrawEstimatedPoint(threedtool,
            (*it)->get_feature_measurement_model()->get_yigraphicsRES(),
            (*it)->get_label() + 1);
        }
        else if (strcmp((*it)->get_feature_measurement_model()->
          feature_graphics_type,
          "THREED_SEMI_INFINITE_LINE") == 0)
        {
          DrawEstimatedSemiInfiniteLine(threedtool,
            (*it)->get_feature_measurement_model()->get_yigraphicsRES(),
            SEMI_INFINITE_LINE_LENGTH, (*it)->get_label() + 1);
        }
      }

      if (display_3d_feature_uncertainties_flag)
      {
        if (strcmp((*it)->get_feature_measurement_model()->
              feature_graphics_type,
            "THREED_POINT") == 0)
        {
          DrawPointCovariance(threedtool,
            (*it)->get_feature_measurement_model()->get_yigraphicsRES(),
            (*it)->get_feature_measurement_model()->get_PyiyigraphicsRES(),
            (*it)->get_label() + 1);

        }
      }
    }
  }

  if (display_axes_flag)
  {
    DrawAxes(threedtool);
  }

  if (display_annotations_flag)
  {
    DrawAnnotations(threedtool);
  }
}


void RectifiedInternal3DDraw(const Scene_Single *scene, 
  VW::ThreeDToolGLCommon &threedtool,
  const vector< VNL::VectorFixed<3,double> >
    &trajectory_store,
  const VW::ImageMono<unsigned char> *grabbed_image,
  const double Graphics_U0,
  const double Graphics_V0,
  const double Graphics_Kd1,   // Radial distortion
  const bool display_camera_flag,
  const bool display_camera_trajectory_flag,
  const bool display_3d_features_flag,
  const bool display_3d_feature_uncertainties_flag,
  const bool display_axes_flag,
  const bool display_annotations_flag,
  const bool display_rectified_image_flag,
  const bool)
{
  ThreeD_Motion_Model *threed_motion_model = 
    (ThreeD_Motion_Model *) scene->get_motion_model();

  VW::Vector3D r_local = threed_motion_model->get_rRES();
  VW::Quaternion qWR = threed_motion_model->get_qRES();
  VW::Quaternion qWO = qWR * qRO;

  if (display_camera_flag) {
    threed_motion_model->func_xp(scene->get_xv());
    threed_motion_model->func_r(threed_motion_model->get_xpRES());
    threed_motion_model->func_q(threed_motion_model->get_xpRES());

    DrawCamera(threedtool, r_local, qWO);
  }

  if (display_camera_trajectory_flag) {
    DrawCameraTrajectory(threedtool, trajectory_store);
  }

  if (display_3d_features_flag || display_3d_feature_uncertainties_flag) {

    threedtool.PushDrawingPosition();
    threedtool.TranslateDrawingPosition(r_local);
    threedtool.RotateDrawingPosition(qWR);

    for (vector<Feature *>::const_iterator it = 
      scene->get_feature_list().begin();
      it != scene->get_feature_list().end(); it++) {
      Feature_Measurement_Model *fmm = (*it)->get_feature_measurement_model();

      SetFeatureColour(threedtool, (*it)->get_selected_flag(),
        (*it)->get_successful_measurement_flag(),
        scene->get_marked_feature_label() == int((*it)->get_label()));

      // Form state and covariance for drawing
      fmm->func_zeroedyigraphics_and_Pzeroedyigraphics((*it)->get_y(),
                                                       scene->get_xv(),
                                                       scene->get_Pxx(),
                                                       (*it)->get_Pxy(),
                                                       (*it)->get_Pyy());

      if (display_3d_features_flag)
      {
        if(strcmp(fmm->feature_graphics_type,
            "THREED_POINT") == 0)
        {
          DrawEstimatedPoint(threedtool, fmm->
                get_zeroedyigraphicsRES(),
                (*it)->get_label() + 1);
        }
        else if(strcmp(fmm->feature_graphics_type,
              "THREED_SEMI_INFINITE_LINE") == 0)
        {
          DrawEstimatedSemiInfiniteLine(threedtool,
            fmm->get_zeroedyigraphicsRES(),
            SEMI_INFINITE_LINE_LENGTH,
            (*it)->get_label() + 1);
        }
      }

      if(display_3d_feature_uncertainties_flag)
      {
        if(strcmp(fmm->feature_graphics_type,
              "THREED_POINT") == 0)
        {
          // The covariance we draw is relative to the robot
          DrawPointCovariance(threedtool,
                  fmm->get_zeroedyigraphicsRES(),
                  fmm->get_PzeroedyigraphicsRES(),
                  (*it)->get_label() + 1);
        }
        // Note that we don't yet draw uncertainty on lines
      }
    }

    threedtool.PopDrawingPosition();
  }

  if (display_axes_flag) {
    DrawAxes(threedtool);
  }


  if (display_annotations_flag) {
    DrawAnnotations(threedtool);
  }

  if (display_rectified_image_flag)
  {
    VW::Point2D distortion_centre(Graphics_U0, Graphics_V0);
    threedtool.DrawCinemaScreen(100, *grabbed_image, -Graphics_Kd1, 
                                distortion_centre);
  }
}


/**
Draw the scene from the camera viewpoint, including the current image. 
The image is not rectified, so may not be from a perspective camera.
@param scene The SLAM map
@param threedtool The GL viewer to use
@param grabbed_image The current camera image
@param feature_init_info_vector The vector of partially-initialised features.
TODO: Why is this not grabbed from Scene?
@param display_2d_descriptors_flag Draw the feature descriptors
(e.g. the image patches)
@param display_2d_feature_search_regions_flag Draw the feature search regions
@param display_2d_initialisation_search_box_flag Draw the box displaying
search region for new features
@param display_raw_image_flag Draw the current image?
@param init_feature_search_region_defined_flag Is there an automatic search
region defined
@param BOXSIZE What size are the feature patches?
@param location_selected_flag Has the user selected a point?
@param uu The current user-selected x-coordinate
@param vv The current user-selected y-coordinate
@param init_feature_search_ustart The x-start of the automatic search region
@param init_feature_search_vstart The y-start of the automatic search region
@param init_feature_search_ufinish The x-finish of the automatic search region
@param init_feature_search_yfinish The y-finish of the automatic search region
**/
void RawInternal3DDraw(const Scene_Single *scene, 
                       VW::ThreeDToolGLCommon &threedtool,
                       const VW::ImageMono<unsigned char> *grabbed_image,
                       const vector<FeatureInitInfo> &feature_init_info_vector,
                       const bool display_2d_descriptors_flag,
                       const bool display_2d_feature_search_regions_flag,
                       const bool display_2d_initialisation_search_box_flag,
                       const bool display_raw_image_flag,
                       const bool init_feature_search_region_defined_flag,
                       const unsigned int BOXSIZE,
                       const bool location_selected_flag,
                       const unsigned int uu, unsigned int vv,
                       const unsigned int init_feature_search_ustart,
                       const unsigned int init_feature_search_vstart,
                       const unsigned int init_feature_search_ufinish,
                       const unsigned int init_feature_search_vfinish)
{
  // VW::Timer timerlocal;

  //  cout << "RawInternal start: " << timerlocal << endl;

  Motion_Model *motion_model = scene->get_motion_model();

  motion_model->func_xp(scene->get_xv());
  motion_model->func_dxp_by_dxv(scene->get_xv());

  if (display_2d_descriptors_flag || display_2d_feature_search_regions_flag)
  {
    for (vector<Feature *>::const_iterator it = 
      scene->get_feature_list().begin();
      it != scene->get_feature_list().end(); it++)
    {
      SetFeatureColour(threedtool, (*it)->get_selected_flag(),
         (*it)->get_successful_measurement_flag(),
         scene->get_marked_feature_label() == int((*it)->get_label()));

      if (strcmp((*it)->get_feature_measurement_model()->
        feature_graphics_type, "THREED_POINT") == 0)
      {
        // Set up some general stuff for this feature
        Fully_Initialised_Feature_Measurement_Model *fully_init_fmm =
          dynamic_cast<Fully_Initialised_Feature_Measurement_Model*>(
            (*it)->get_feature_measurement_model());
        fully_init_fmm->
          func_hi_and_dhi_by_dxp_and_dhi_by_dyi((*it)->get_y(),
                                                motion_model->get_xpRES());
        fully_init_fmm->
          func_zeroedyigraphics_and_Pzeroedyigraphics((*it)->get_y(),
                                                      scene->get_xv(),
                                                      scene->get_Pxx(),
                                                      (*it)->get_Pxy(),
                                                      (*it)->get_Pyy());

        // Only draw 2D stuff if z>0 (or we get some nasty loop-around
        // drawing of features behind the camera)
        if (fully_init_fmm->get_zeroedyigraphicsRES()(2) > 0)
        {
          // Draw search regions
          if (display_2d_feature_search_regions_flag)
          {
            if ((*it)->get_selected_flag())
            {
              Draw2DPointCovariance(threedtool, (*it)->get_h(), (*it)->get_S(),
                                    (*it)->get_label() + 1);
            }
          }

          // Draw image patches
          if (display_2d_descriptors_flag)
          {
            // We know that our descriptors are image patches
            ExtraData* feature_data = (*it)->get_identifier();
            ImageMonoExtraData* patch_ptr =
              dynamic_cast<ImageMonoExtraData*>(feature_data);

            // Where to draw the patches?
            double draw_patch_x = -1, draw_patch_y = -1;
            if((*it)->get_selected_flag() &&
               (*it)->get_successful_measurement_flag())
            {
              // If we just successfully matched this feature,
              // then draw match position
              draw_patch_x = (*it)->get_z()(0);
              draw_patch_y = (*it)->get_z()(1);
            }
            else
            {
              // Otherwise current estimated position after update
              draw_patch_x = fully_init_fmm->get_hiRES()(0);
              draw_patch_y = fully_init_fmm->get_hiRES()(1);
            }
            Draw2DPatch(threedtool, draw_patch_x, draw_patch_y,
            BOXSIZE, *patch_ptr,
            (*it)->get_label() + 1);
          }
        }
      }
      else if(strcmp((*it)->get_feature_measurement_model()->
                feature_graphics_type,
              "THREED_SEMI_INFINITE_LINE") == 0)
      {
        if (display_2d_feature_search_regions_flag)
        {
          Draw2DPartiallyInitialisedLineEllipses(scene,
                  threedtool,
                  feature_init_info_vector,
                  (*it));
        }
      }
    }
  }

  // cout << "RawInternal after drawing features: " << timerlocal << endl;

  if (display_2d_initialisation_search_box_flag)
  {
    Draw2DInitialisationBoxes(threedtool,
                              location_selected_flag,
                              init_feature_search_region_defined_flag,
                              uu, vv,
                              init_feature_search_ustart,
                              init_feature_search_vstart,
                              init_feature_search_ufinish,
                              init_feature_search_vfinish,
                              BOXSIZE);
  }

  if (display_raw_image_flag)
  {
    threedtool.DrawCinemaScreen(100, *grabbed_image);
  }

  // cout << "RawInternal end: " << timerlocal << endl;
}


/**
Draw a representation of a Fire-i camera.
@param threedtool The GL viewer
@param rW The position
@param qW0 The orientation quaternion
**/
void DrawCamera(VW::ThreeDToolGLCommon &threedtool,
                const VW::Vector3D rW,
                const VW::Quaternion qWO)
{
  threedtool.SetPenColour(120, 120, 120, 0);
  VW::DrawFireI(threedtool, rW, qWO);
}


void DrawCameraCovariance(VW::ThreeDToolGLCommon &threedtool,
			  const VW::Vector3D rW,
			  VNL::MatrixFixed<3,3,double> Prr)
{
  threedtool.SetPenColour(0, 120, 120, 0);
  threedtool.DrawCovariance(rW, Prr, COVARIANCES_NUMBER_OF_SIGMA);
}

/**
Draw a trajectory.
@param threedtool The GL viewer
@param trajectory_store The list of points on the trajectory
**/
void DrawCameraTrajectory(VW::ThreeDToolGLCommon &threedtool,
  const vector< VNL::VectorFixed<3,double> > &trajectory_store)
{
  threedtool.SetPenColour(255, 255, 0, 0);

  if (trajectory_store.size() >= 2)
  {
    vector< VNL::VectorFixed<3,double> >::const_iterator previous
      = trajectory_store.begin();
    for (vector< VNL::VectorFixed<3,double> >::const_iterator it 
      = trajectory_store.begin() + 1; it != trajectory_store.end();
      it++)
    {
      threedtool.DrawLine((*previous), (*it));
      previous++;
    }
  }
}

void SetFeatureColour(VW::ThreeDToolGLCommon &threedtool,
                      const bool selected_flag,
                      const bool successful_measurement_flag,
                      const bool marked_flag)
{
  if (marked_flag) {
    // Marked feature green
    threedtool.SetPenColour(0, 255, 0, 0);
  }
  else {
    if (selected_flag) {
      if (successful_measurement_flag)
        // Successfully measured feature red
        threedtool.SetPenColour(255, 0, 0, 0);
      else
        // Failed measured feature blue
        threedtool.SetPenColour(0, 0, 255, 0);
    }
    else {
      // Unselected feature yellow
      threedtool.SetPenColour(255, 255, 0, 0);
    }
  }
}


void DrawEstimatedPoint(VW::ThreeDToolGLCommon &threedtool,
                        const VNL::VectorFixed<3,double> &yigraphics,
                        const unsigned int name_to_draw)
{
  threedtool.DrawPoint(yigraphics, name_to_draw);
}

// Draw the uncertainty ellipsoid around a point
void DrawPointCovariance(VW::ThreeDToolGLCommon &threedtool,
                         const VNL::VectorFixed<3,double> &yigraphics,
                         const VNL::MatrixFixed<3,3,double> &Pyiyigraphics,
                         const unsigned int name_to_draw)
{
  threedtool.DrawCovariance(yigraphics, Pyiyigraphics, 
                            COVARIANCES_NUMBER_OF_SIGMA, name_to_draw);
}

void DrawEstimatedSemiInfiniteLine(VW::ThreeDToolGLCommon &,
  const VNL::VectorFixed<6,double> &yigraphics,
  const double line_length,
  const unsigned int name_to_draw)
{
  // Semi-infinite line representation is end point and normalised
  // direction vector yigraphics = (x, y, z, hhatx, hhaty, hhatz)
  // Since we aren't really going to draw a semi-infinite line,
  // draw it some length
  VW::Vector3D y0(yigraphics(0), yigraphics(1), yigraphics(2));
  VW::Vector3D hhat(yigraphics(3), yigraphics(4), yigraphics(5));
  VW::Vector3D y1 = y0 + hhat * line_length;

  // Draw line with raw GL to avoid including it in rotation centre
  glLoadName(name_to_draw);
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);

  glBegin(GL_LINE_STRIP); {
    glVertex3d(y0.GetX(), y0.GetY(), y0.GetZ());
    glVertex3d(y1.GetX(), y1.GetY(), y1.GetZ());
  }; glEnd();

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLoadName(0);
}

void DrawAxes(VW::ThreeDToolGLCommon &threedtool)
{
  threedtool.SetPenColour(255, 255, 255, 0);
  threedtool.DrawAxes();
}

void DrawAnnotations(VW::ThreeDToolGLCommon &)
{

}

void Draw2DPointCovariance(VW::ThreeDToolGLCommon &threedtool,
                           const VNL::VectorFixed<2,double> &h,
                           const VNL::MatrixFixed<2,2,double> &S,
                           const unsigned int name_to_draw)
{
  threedtool.Draw2DCovariance(h(0), h(1), S, 
                              COVARIANCES_NUMBER_OF_SIGMA, 1.0,
                              name_to_draw);
}

void Draw2DPatch(VW::ThreeDToolGLCommon &threedtool, 
                 double draw_patch_x, double draw_patch_y,
                 unsigned int BOXSIZE,
                 const VW::ImageMono<unsigned char> &patch,
                 const unsigned int name_to_draw)
{
  threedtool.Draw2DTexturedRectangle(draw_patch_x, draw_patch_y,
                                     BOXSIZE, BOXSIZE, patch,
                                     name_to_draw);
  threedtool.Draw2DRectangle(draw_patch_x, draw_patch_y,
                             BOXSIZE + 2, BOXSIZE + 2,
                             name_to_draw);
}

/**
Draw the search box for new features.
@param threedtool The GL viewer
@param location_selected_flag Has the user selected a new feature?
@param init_feature_search_region_defined_flag Is there an automatic search region defined
@param uu The current user-selected x-coordinate
@param vv The current user-selected y-coordinate
@param init_feature_search_ustart The x-start of the automatic search region
@param init_feature_search_vstart The y-start of the automatic search region
@param init_feature_search_ufinish The x-finish of the automatic search region
@param init_feature_search_yfinish The y-finish of the automatic search region
**/
void Draw2DInitialisationBoxes(VW::ThreeDToolGLCommon &threedtool, 
  const bool location_selected_flag,
  const bool init_feature_search_region_defined_flag,
  const unsigned int uu, unsigned int vv,
  const unsigned int init_feature_search_ustart,
  const unsigned int init_feature_search_vstart,
  const unsigned int init_feature_search_ufinish,
  const unsigned int init_feature_search_vfinish,
  const unsigned int BOXSIZE)
{
  threedtool.SetPenColour(0, 255, 0, 0);

  // Draw box selected by user
  if (location_selected_flag) {
    threedtool.Draw2DRectangle(uu, vv,BOXSIZE, BOXSIZE);
  }

  // Draw initialisation box
  if (init_feature_search_region_defined_flag) {
    threedtool.Draw2DRectangle(
      (init_feature_search_ustart + init_feature_search_ufinish) / 2.0,
      (init_feature_search_vstart + init_feature_search_vfinish) / 2.0,
      init_feature_search_ufinish - init_feature_search_ustart,
      init_feature_search_vfinish - init_feature_search_vstart);
  }
}


/**
Draw a partially-initialised feature into a GL display as a series of ellipses.
This steps through the particles representing different values for the free
parameters and draws ellipses representing. Not every particle is drawn: only
every n are drawn, determined by the constant DRAW_N_OVERLAPPING_ELLIPSE (a
value of zero for this means draw every particle). The ellipses represent the
number of standard deviations set by COVARIANCES_NUMBER_OF_SIGMA.
@param scene The SLAM map to use
@param threedtool The GL display to draw to
@param feature_init_info_vector The partially-initialised features (TODO: Why
is this not just taken from scene?)
@param fp The particular feature to draw
*/
void Draw2DPartiallyInitialisedLineEllipses(
  const Scene_Single *scene, 
  VW::ThreeDToolGLCommon &threedtool,
  const vector<FeatureInitInfo> &feature_init_info_vector,
  Feature *fp)
{
  // Find this particular feature in the list of information about
  // partially-initalised features.
  vector<FeatureInitInfo>::const_iterator feat;
  for (feat = feature_init_info_vector.begin(); 
       feat != feature_init_info_vector.end(); feat++)
  {
    if (feat->get_fp() == fp)
      break;
  }
  assert (feat->get_fp() == fp);

  // Counter so we only draw every Nth ellipse
  unsigned int draw_counter = 1;

  // Loop over different values of depth (lambda) to draw some ellipses
  // First see how many to skip when drawing?
  unsigned int particles_step;
  if(DRAW_N_OVERLAPPING_ELLIPSES != 0)
    particles_step =
      max(feat->get_particle_vector().size() / DRAW_N_OVERLAPPING_ELLIPSES,
          FeatureInitInfo::ParticleVector::size_type(1));
  else
    particles_step = 1;
    
  for (vector<Particle>::const_iterator it = 
  feat->get_particle_vector().begin();
       it != feat->get_particle_vector().end(); it++)
  {

    if (draw_counter != particles_step)
    {
      draw_counter++;
    }
    else
    {
      draw_counter = 1;

      scene->get_motion_model()->func_xp(scene->get_xv());
      VNL::Vector<double> lambda_vector = it->get_lambda();

      Partially_Initialised_Feature_Measurement_Model *partial_init_fmm =
        dynamic_cast<Partially_Initialised_Feature_Measurement_Model*>(
          fp->get_feature_measurement_model());
      partial_init_fmm->
        func_hpi_and_dhpi_by_dxp_and_dhpi_by_dyi(fp->get_y(),
          scene->get_motion_model()->get_xpRES(), lambda_vector);

      VNL::Vector<double> local_hpi = partial_init_fmm->get_hpiRES();
      VNL::Matrix<double> local_dhpi_by_dyi = 
      partial_init_fmm->get_dhpi_by_dyiRES();

      scene->get_motion_model()->func_dxp_by_dxv(scene->get_xv());
      VNL::Matrix<double> local_dhpi_by_dxv = 
        partial_init_fmm->get_dhpi_by_dxpRES() *
          scene->get_motion_model()->get_dxp_by_dxvRES();

      fp->get_feature_measurement_model()->func_Ri(local_hpi);  
      VNL::Matrix<double> local_Ri = 
        fp->get_feature_measurement_model()->get_RiRES();

      fp->get_feature_measurement_model()->func_Si(scene->get_Pxx(), 
        fp->get_Pxy(), fp->get_Pyy(),
        local_dhpi_by_dxv, local_dhpi_by_dyi, local_Ri);

      // Draw ellipses with brightness determined by probability
      threedtool.SetPenColour(255, 255, 0, 0);
      threedtool.Draw2DCovariance(local_hpi(0), local_hpi(1),
        fp->get_feature_measurement_model()->get_SiRES(),
        COVARIANCES_NUMBER_OF_SIGMA, 1.0);
      //cerr << "Particle: ";
      //MatlabPrint(cerr, local_hpi); cerr << endl;
      //MatlabPrint(cerr, fp->get_feature_measurement_model()->get_SiRES());
    }
  }
}

/**
Draw a the particles of a partially-initialised feature into a GL display as a
line of spheres. This steps through the particles representing different
values for the free parameters and draws a sphere for each particle, scaled
according to its probability. Not every particle is drawn: only every nth is
drawn, determined by the parameter draw_every_n. This function is slow - don't
use it in real-time operation!
@param scene The SLAM map to use
@param threedtool The GL display to draw to
@param feature_init_info_vector The partially-initialised features (TODO: Why
is this not just taken from scene?)
@param fp The particular feature to draw
@param draw_every_n The number of particles to skip on before drawing the next
one.
*/
void Draw3DPartiallyInitialisedLineParticles(
  const Scene_Single *scene, 
  VW::ThreeDToolGLCommon &threedtool,
  const vector<FeatureInitInfo> &feature_init_info_vector,
  Feature *fp,
  unsigned int draw_every_n)
{
  // Find this particular feature in the list of information about
  // partially-initalised features.
  vector<FeatureInitInfo>::const_iterator feat;
  for (feat = feature_init_info_vector.begin(); 
       feat != feature_init_info_vector.end(); feat++)
  {
    if (feat->get_fp() == fp)
      break;
  }
  assert (feat->get_fp() == fp);

  // Counter so we only draw every Nth ellipse
  unsigned int draw_counter = 1;

  // Loop over different values of depth (lambda) to draw some ellipses
  for (vector<Particle>::const_iterator it = 
  feat->get_particle_vector().begin();
       it != feat->get_particle_vector().end(); it++)
  {
    if (draw_counter != draw_every_n)
    {
      draw_counter++;
    }
    else
    {
      draw_counter = 1;

      scene->get_motion_model()->func_xp(scene->get_xv());
      VNL::Vector<double> lambda_vector = it->get_lambda();

      Partially_Initialised_Feature_Measurement_Model *partial_init_fmm =
        dynamic_cast<Partially_Initialised_Feature_Measurement_Model*>(
          fp->get_feature_measurement_model());
      assert(partial_init_fmm != 0);

      // What is this particle as a fully-ionitialised feature?
      partial_init_fmm->func_yfi_and_dyfi_by_dypi_and_dyfi_by_dlambda(
        fp->get_y(), lambda_vector);

      VNL::VectorFixed<3> yfi = partial_init_fmm->get_yfiRES();

      threedtool.SetPenColour(128, 255, 0, 0);
      threedtool.DrawSphere(yfi, it->get_probability());
    }
  }
}

/**
@}
*/
