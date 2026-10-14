/*  SceneLib: software for real-time SLAM

    threeddraw.h
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

#ifndef THREEDDRAW_H__
#define THREEDDRAW_H__

#include <vector>
#include "VNL/vectorfixed.h"
#include "VWGL/Display/threedtoolglcommon.h"
#include "Scene/scene_single.h"


// Functions for 3D drawing of the status of MonoSLAM tracking using
// the VW 3D display

const double SEMI_INFINITE_LINE_LENGTH = 10.0;
const double COVARIANCES_NUMBER_OF_SIGMA = 3;
const int DRAW_N_OVERLAPPING_ELLIPSES = 10;

// Draw the scene from an external Euclidean point of view
void External3DDraw(const Scene_Single *scene, 
		    VW::ThreeDToolGLCommon &threedtool,
		    const std::vector< VNL::VectorFixed<3,double> > 
		      &trajectory_store,
		    const bool display_camera_flag,
		    const bool display_camera_uncertainty_flag,
		    const bool display_camera_trajectory_flag,
		    const bool display_3d_features_flag,
		    const bool display_3d_feature_uncertainties_flag,
		    const bool display_axes_flag,
		    const bool display_annotations_flag);


// Draw the scene from an internal Euclidean point of view
// (in which the image is rectified to perspective)
void RectifiedInternal3DDraw(const Scene_Single *scene, 
			     VW::ThreeDToolGLCommon &threedtool,
			     const std::vector< VNL::VectorFixed<3,double> > 
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
			     const bool lock_to_camera_flag);

// Draw the scene from an internal raw image point of view
// (with the raw image which may not be perspective)
void RawInternal3DDraw(const Scene_Single *scene, 
		       VW::ThreeDToolGLCommon &threedtool,
		       const VW::ImageMono<unsigned char> *grabbed_image,
		       const std::vector<FeatureInitInfo> &feature_init_info_vector,
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
		       const unsigned int init_feature_search_vfinish);

// Draw a camera
void DrawCamera(VW::ThreeDToolGLCommon &threedtool,
		const VW::Vector3D rW,
		const VW::Quaternion qWO);

void DrawCameraCovariance(VW::ThreeDToolGLCommon &threedtool,
			  const VW::Vector3D rW,
			  VNL::MatrixFixed<3,3,double> Prr);

// Draw camera trajectory
void DrawCameraTrajectory(VW::ThreeDToolGLCommon &threedtool,
  const std::vector< VNL::VectorFixed<3,double> > &trajectory_store);

// Set colour for feature drawing
void SetFeatureColour(VW::ThreeDToolGLCommon &threedtool,
		      const bool selected_flag, 
		      const bool successful_measurement_flag,
		      const bool marked_flag);

// Draw a point
void DrawEstimatedPoint(VW::ThreeDToolGLCommon &threedtool,
			const VNL::VectorFixed<3,double> &yigraphics,
			const unsigned int name_to_draw);

// Draw the uncertainty ellipsoid around a point
void DrawPointCovariance(VW::ThreeDToolGLCommon &threedtool,
			 const VNL::VectorFixed<3,double> &yigraphics,
			 const VNL::MatrixFixed<3,3,double> &Pyiyigraphics,
			 const unsigned int name_to_draw);

// Draw a semi-infinite line
void DrawEstimatedSemiInfiniteLine(VW::ThreeDToolGLCommon &threedtool,
				  const VNL::VectorFixed<6,double> &yigraphics,
				   const double line_length,
				   const unsigned int name_to_draw);


// Draw axes
void DrawAxes(VW::ThreeDToolGLCommon &threedtool);

// Draw annotations
void DrawAnnotations(VW::ThreeDToolGLCommon &threedtool);


// Draw a 2D point covariance
void Draw2DPointCovariance(VW::ThreeDToolGLCommon &threedtool,
			   const VNL::VectorFixed<2,double> &h,
			   const VNL::MatrixFixed<2,2,double> &S,
			   const unsigned int name_to_draw);
			   
void Draw2DPatch(VW::ThreeDToolGLCommon &threedtool, 
		 double draw_patch_x, double draw_patch_y,
		 unsigned int BOXSIZE,
		 const VW::ImageMono<unsigned char> &patch,
		 const unsigned int name_to_draw);


void Draw2DInitialisationBoxes(VW::ThreeDToolGLCommon &threedtool, 
			       const bool location_selected_flag,
		       const bool init_feature_search_region_defined_flag,
			       const unsigned int uu, unsigned int vv,
			       const unsigned int init_feature_search_ustart,
			       const unsigned int init_feature_search_vstart,
			       const unsigned int init_feature_search_ufinish,
			       const unsigned int init_feature_search_vfinish,
			       const unsigned int BOXSIZE);

// Draw the ellipses around a semi-infinite line
void Draw2DPartiallyInitialisedLineEllipses(const Scene_Single *scene,
					    VW::ThreeDToolGLCommon &threedtool,
    		       const std::vector<FeatureInitInfo> &feature_init_info_vector,
					    Feature *fp);

void Draw3DPartiallyInitialisedLineParticles(
  const Scene_Single *scene, 
  VW::ThreeDToolGLCommon &threedtool,
  const std::vector<FeatureInitInfo>& feature_init_info_vector,
  Feature *fp,
  unsigned int draw_every_n = 1);

#endif
