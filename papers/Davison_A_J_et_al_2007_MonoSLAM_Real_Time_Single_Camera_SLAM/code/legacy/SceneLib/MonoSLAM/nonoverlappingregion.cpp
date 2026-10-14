/*  SceneLib: software for real-time SLAM

    nonoverlappingregion.cpp
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

#include <VW/Utils/timer.h>
#include "nonoverlappingregion.h"


#ifdef DEBUGDUMP
#undef DEBUGDUMP
#endif
const bool DEBUGDUMP = false;

/**
@addtogroup gMonoSLAM
@{
*/

using namespace std;

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
			      const double FEATURE_INIT_DEPTH_HYPOTHESIS)
{
  ThreeD_Motion_Model *threed_motion_model = 
    (ThreeD_Motion_Model *) scene->get_motion_model();

  VNL::Vector<double> local_xv = scene->get_xv();

  for (unsigned int i = 0; i < FEATURE_INIT_STEPS_TO_PREDICT; i++) {
    scene->get_motion_model()->func_fv_and_dfv_by_dxv(local_xv, local_u, 
						      delta_t);
    local_xv.Update(scene->get_motion_model()->get_fvRES());
  }

  threed_motion_model->func_xp(local_xv);
  VNL::Vector<double> local_xp = threed_motion_model->get_xpRES();

  threed_motion_model->func_r(local_xp);
  VW::Vector3D rW = threed_motion_model->get_rRES();
  threed_motion_model->func_q(local_xp);
  VW::Quaternion qWR = threed_motion_model->get_qRES();

  // yW =  rW + RWR hR
  VW::Vector3D hR (0.0, 0.0, FEATURE_INIT_DEPTH_HYPOTHESIS);

  // Used Inverse + transpose because this was't compiling the normal way
  VW::Vector3D yW = rW + qWR.RotationMatrix() * hR;

  // Then project that point into the current camera position
  scene->get_motion_model()->func_xp(scene->get_xv());

  Fully_Initialised_Feature_Measurement_Model *fully_init_fmm = 
    (Fully_Initialised_Feature_Measurement_Model *) 
    (default_feature_type_for_initialisation->
     more_initialised_feature_measurement_model);

  
  VNL::Vector<double> yWVNL = yW.GetVNL3();
  fully_init_fmm->
    func_hi_and_dhi_by_dxp_and_dhi_by_dyi(yWVNL, 
				  scene->get_motion_model()->get_xpRES());

  if (DEBUGDUMP) cerr << "Suggested place to initialise feature: (u, v) = (" 
		      << fully_init_fmm->get_hiRES()(0) << ", " 
		      << fully_init_fmm->get_hiRES()(1) << ")" << endl;

  // Now, this defines roughly how much we expect a feature initialised 
  // to move
  double predicted_motion_u = camera_width / 2.0 
    - fully_init_fmm->get_hiRES()(0);
  double predicted_motion_v = camera_height / 2.0 
    - fully_init_fmm->get_hiRES()(1);
  


  // So, the limits of a "safe" region within which we can initialise
  // features so that they end up staying within the screen
  // (Making the approximation that the whole screen moves like the 
  // centre point)
  int safe_feature_search_ustart = (int) (-predicted_motion_u);
  int safe_feature_search_vstart = (int) (-predicted_motion_v);
  int safe_feature_search_ufinish = (int) (camera_width - predicted_motion_u);
  int safe_feature_search_vfinish = (int) (camera_height - predicted_motion_v);

  if (safe_feature_search_ustart < ((int) ((BOXSIZE - 1)/2) + 1))
    safe_feature_search_ustart = (BOXSIZE - 1)/2 + 1;
  if (safe_feature_search_ufinish > 
      (int) camera_width - ((int) ((BOXSIZE - 1)/2) + 1) )
    safe_feature_search_ufinish = (int) camera_width - 
                                     (BOXSIZE - 1)/2 - 1;
  if (safe_feature_search_vstart < ((int) ((BOXSIZE - 1)/2) + 1))  
    safe_feature_search_vstart = (BOXSIZE - 1)/2 + 1;
  if (safe_feature_search_vfinish > (int) camera_height - 
      ((int) ((BOXSIZE - 1)/2) + 1) )
    safe_feature_search_vfinish = camera_height - (BOXSIZE - 1)/2 - 1;

  if (DEBUGDUMP) cout << "sfsus " << safe_feature_search_ustart
		      << "sfsvs " << safe_feature_search_vstart
		      << "sfsuf " << safe_feature_search_ufinish
		      << "sfsvf " << safe_feature_search_vfinish << endl;

  return FindNonOverlappingRegionNoPredict(safe_feature_search_ustart,
					   safe_feature_search_vstart,
					   safe_feature_search_ufinish,
					   safe_feature_search_vfinish,
					   scene,
					   init_feature_search_ustart, 
					   init_feature_search_vstart, 
					   init_feature_search_ufinish,
					   init_feature_search_vfinish);
}

bool FindNonOverlappingRegionWholeImage(Scene_Single *scene,
					unsigned int camera_width, 
					unsigned int camera_height,
					unsigned int BOXSIZE,
					int &init_feature_search_ustart, 
					int &init_feature_search_vstart, 
					int &init_feature_search_ufinish,
					int &init_feature_search_vfinish)
{
  int safe_feature_search_ustart = (BOXSIZE - 1)/2 + 1;
  int safe_feature_search_ufinish = (int) camera_width - (BOXSIZE - 1)/2 - 1;
  int safe_feature_search_vstart = (BOXSIZE - 1)/2 + 1;
  int safe_feature_search_vfinish = camera_height - (BOXSIZE - 1)/2 - 1;

  return FindNonOverlappingRegionNoPredict(safe_feature_search_ustart,
					   safe_feature_search_vstart,
					   safe_feature_search_ufinish,
					   safe_feature_search_vfinish,
					   scene,
					   init_feature_search_ustart, 
					   init_feature_search_vstart, 
					   init_feature_search_ufinish,
					   init_feature_search_vfinish);
}


bool FindNonOverlappingRegionNoPredict(int safe_feature_search_ustart,
				       int safe_feature_search_vstart,
				       int safe_feature_search_ufinish,
				       int safe_feature_search_vfinish,
				       Scene_Single *scene,
				       int &init_feature_search_ustart, 
				       int &init_feature_search_vstart, 
				       int &init_feature_search_ufinish,
				       int &init_feature_search_vfinish)
{
  VW::Timer timerlocal;

  if (DEBUGDUMP) cout << "FNOLRNP timer start: " << timerlocal << endl;

  const int INIT_FEATURE_SEARCH_WIDTH = 80;
  const int INIT_FEATURE_SEARCH_HEIGHT = 60;

  // Within this, choose a random region
  // Check that we've got some room for manouevre
  if (safe_feature_search_ufinish - safe_feature_search_ustart >
      INIT_FEATURE_SEARCH_WIDTH &&
      safe_feature_search_vfinish - safe_feature_search_vstart >
      INIT_FEATURE_SEARCH_HEIGHT) {

    // Try a few times to get one that's not overlapping with any features
    // we know about
    const int NUMBER_OF_RANDOM_INIT_FEATURE_SEARCH_REGION_TRIES = 5;
    const int FEATURE_SEPARATION_MINIMUM = 10;

    // Build vectors of feature positions so we only have to work them out once
    vector<double> u_array;
    vector<double> v_array;

    for (vector<Feature *>::const_iterator it = 
	   scene->get_feature_list().begin();
	 it != scene->get_feature_list().end(); it++) {
      if ((*it)->get_feature_measurement_model()->fully_initialised_flag) {
	Fully_Initialised_Feature_Measurement_Model *fifmm = 
	  (Fully_Initialised_Feature_Measurement_Model *) 
	  ((*it)->get_feature_measurement_model());
	fifmm->func_hi_and_dhi_by_dxp_and_dhi_by_dyi((*it)->get_y(), 
				scene->get_motion_model()->get_xpRES());
	  	  
	// Check that this is not a feature behind the camera
	if (strcmp((*it)->get_feature_measurement_model()->
		   feature_graphics_type, "THREED_POINT") == 0) {

	  fifmm->
	      func_zeroedyigraphics_and_Pzeroedyigraphics((*it)->get_y(),
							  scene->get_xv(),
							  scene->get_Pxx(),
							  (*it)->get_Pxy(),
							  (*it)->get_Pyy());

	  if (fifmm->get_zeroedyigraphicsRES()(2) > 0) {
	    u_array.push_back(fifmm->get_hiRES()(0));
	    v_array.push_back(fifmm->get_hiRES()(1));
	  }
	}    
      }     
    }

    if (DEBUGDUMP) cout << "FNOLRNP timer after functions: " << timerlocal
                        << endl;


    int i = 0;

    while (i < NUMBER_OF_RANDOM_INIT_FEATURE_SEARCH_REGION_TRIES) {

      if (DEBUGDUMP) 
	cout << "FNOLRNP timer starting iteration: " 
	     << i << ": " << timerlocal << endl;

      if (DEBUGDUMP) cout << "Search for non-overlapping region: trial " << i
                          << endl;
      int u_offset = int((safe_feature_search_ufinish - 
			  safe_feature_search_ustart
			  - INIT_FEATURE_SEARCH_WIDTH) * drand48());
      int v_offset = int((safe_feature_search_vfinish - 
			  safe_feature_search_vstart
			  - INIT_FEATURE_SEARCH_HEIGHT) * drand48());

      if (DEBUGDUMP) cout << "u_offset " << u_offset 
			  << " v_offset " << v_offset
			  << endl;


      init_feature_search_ustart = safe_feature_search_ustart + u_offset;
      init_feature_search_ufinish = 
	init_feature_search_ustart + INIT_FEATURE_SEARCH_WIDTH;
      init_feature_search_vstart = safe_feature_search_vstart + v_offset;
      init_feature_search_vfinish = 
	init_feature_search_vstart + INIT_FEATURE_SEARCH_HEIGHT;

      bool found_a_feature_in_region_flag = false;

      if (DEBUGDUMP) cout << "Trial box: us" 
	   << init_feature_search_ustart - FEATURE_SEPARATION_MINIMUM << " uf "
           << init_feature_search_ufinish + FEATURE_SEPARATION_MINIMUM <<" vs "
	   << init_feature_search_vstart - FEATURE_SEPARATION_MINIMUM << " vf "
	   << init_feature_search_vfinish + FEATURE_SEPARATION_MINIMUM
	   << endl; 

      // These arrays will be the same size
      vector<double>::const_iterator uit = u_array.begin();
      vector<double>::const_iterator vit = v_array.begin();
      while(uit != u_array.end()) {
	if (*uit >= 
	    init_feature_search_ustart - FEATURE_SEPARATION_MINIMUM &&
	    *uit < 
	    init_feature_search_ufinish + FEATURE_SEPARATION_MINIMUM &&
	    *vit >= 
	    init_feature_search_vstart - FEATURE_SEPARATION_MINIMUM &&
	    *vit < 
	    init_feature_search_vfinish + FEATURE_SEPARATION_MINIMUM) {
	  if (DEBUGDUMP) cout << "Lies in region." << endl;
	  found_a_feature_in_region_flag = true;
	  break;
	}
	uit++;
	vit++;
      }
  
      // If no clashes, fine, leave while loop
      // Else go round again
      if (!found_a_feature_in_region_flag) {
	break;
      }
      
      i++;
    }

    if (i == NUMBER_OF_RANDOM_INIT_FEATURE_SEARCH_REGION_TRIES) {
      return false;
    }



  }

  else {
    return false;
  }

  return true;
}

/**
@}
*/
