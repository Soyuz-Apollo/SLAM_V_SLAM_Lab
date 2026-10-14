/*  SceneLib: software for real-time SLAM

    nonoverlappingregion.h
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



#ifndef _monoslam_nonoverlappingregion_h_
#define _monoslam_nonoverlappingregion_h_

#include "Scene/scene_single.h"
#include "VW/image.h"

// Function to find a region in an image guided by current motion prediction
// which doesn't overlap existing features
bool FindNonOverlappingRegion(Scene_Single *scene,
			      VNL::Vector<double> local_u,
			      double delta_t,
			      Partially_Initialised_Feature_Measurement_Model 
			      *default_feature_type_for_initialisation,
			      unsigned int camera_width, 
			      unsigned int camera_height,
			      unsigned int BOXSIZE,
			      int &init_feature_search_ustart, 
			      int &init_feature_search_vstart, 
			      int &init_feature_search_ufinish,
			      int &init_feature_search_vfinish,
			      const unsigned int FEATURE_INIT_STEPS_TO_PREDICT,
			      const double FEATURE_INIT_DEPTH_HYPOTHESIS);

// Function to find non-overlapping region within whole image (no prediction)
bool FindNonOverlappingRegionWholeImage(Scene_Single *scene,
					unsigned int camera_width, 
					unsigned int camera_height,
					unsigned int BOXSIZE,
					int &init_feature_search_ustart, 
					int &init_feature_search_vstart, 
					int &init_feature_search_ufinish,
					int &init_feature_search_vfinish);

// Function to find non-overlapping region over without prediction --- 
// this is really the service function called by both the above
bool FindNonOverlappingRegionNoPredict(int safe_feature_search_ustart,
				       int safe_feature_search_vstart,
				       int safe_feature_search_ufinish,
				       int safe_feature_search_vfinish,
				       Scene_Single *scene,
				       int &init_feature_search_ustart, 
				       int &init_feature_search_vstart, 
				       int &init_feature_search_ufinish,
				       int &init_feature_search_vfinish);


#endif
