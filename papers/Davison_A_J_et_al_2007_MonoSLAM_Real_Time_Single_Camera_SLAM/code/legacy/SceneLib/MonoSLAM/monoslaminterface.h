/*  SceneLib: software for real-time SLAM

    monoslaminterface.h
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

// MonoSLAMInterface class which supplies a lot of extra access functions
// for use by a GUI, especially if the GUI is not part of the same program
// Kept separate here only for clarity of the main MonoSLAM class

#ifndef _monoslam_monoslaminterface_h_
#define _monoslam_monoslaminterface_h_

#include "monoslam.h"

/**
Provides an enhanced interface to the MonoSLAM real-time single-camera SLAM
application. The MonoSLAM class provides the core interface, while
MonoSLAMInterface provides a richer set of functions that a final application
might need, such as for application control and user-feedback. Both this
class and MonoSLAM are GUI-independent.
@ingroup gMonoSLAM
*/
class MonoSLAMInterface : public MonoSLAM {

 public:

  MonoSLAMInterface(const std::string& initialisation_file,
		    Motion_Model_Creator *mm_creator,
		    Feature_Measurement_Model_Creator *fmm_creator,
		    Internal_Measurement_Model_Creator *imm_creator,
		    unsigned int number_of_features_to_select,
		    unsigned int number_of_features_to_keep_visible,
		    unsigned int max_features_to_init_at_once,
		    double min_lambda,
		    double max_lambda,
		    unsigned int number_of_particles,
		    double standard_deviation_depth_ratio,
		    unsigned int min_number_of_particles,
		    double prune_probability_threshold,
	    unsigned int erase_partially_init_feature_after_this_many_attempts)
    : MonoSLAM(initialisation_file,
               mm_creator,
               fmm_creator,
               imm_creator,
               number_of_features_to_select,
               number_of_features_to_keep_visible,
               max_features_to_init_at_once,
               min_lambda,
               max_lambda,
               number_of_particles,
               standard_deviation_depth_ratio,
               min_number_of_particles,
               prune_probability_threshold,
               erase_partially_init_feature_after_this_many_attempts) {}
  ~MonoSLAMInterface() {}

  // Getters and setters for data members of MonoSLAM class that an 
  // interface might want to use
  void SetNumberOfFeaturesToSelect(unsigned int newval)
  {NUMBER_OF_FEATURES_TO_SELECT = newval;}
  unsigned int GetNumberOfFeaturesToSelect()
  {return NUMBER_OF_FEATURES_TO_SELECT;}

  void SetNumberOfFeaturesToKeepVisible(unsigned int newval)
  {NUMBER_OF_FEATURES_TO_KEEP_VISIBLE = newval;}
  unsigned int GetNumberOfFeaturesToKeepVisible()
  {return NUMBER_OF_FEATURES_TO_KEEP_VISIBLE;}

  void SetMaxFeaturesToInitAtOnce(unsigned int newval)
  {MAX_FEATURES_TO_INIT_AT_ONCE = newval;}
  unsigned int GetMaxFeaturesToInitAtOnce()
  {return MAX_FEATURES_TO_INIT_AT_ONCE;}

  void SetMinLambda(double newval)
  {MIN_LAMBDA = newval;}
  double GetMinLambda()
  {return MIN_LAMBDA;}

  void SetMaxLambda(double newval)
  {MAX_LAMBDA = newval;}
  double GetMaxLambda()
  {return MAX_LAMBDA;}

  void SetNumberOfParticles(unsigned int newval)
  {NUMBER_OF_PARTICLES = newval;}
  unsigned int GetNumberOfParticles()
  {return NUMBER_OF_PARTICLES;}
  
  void SetStandardDeviationDepthRatio(double newval)
  {STANDARD_DEVIATION_DEPTH_RATIO = newval;}
  double GetStandardDeviationDepthRatio()
  {return STANDARD_DEVIATION_DEPTH_RATIO;}

  void SetMinNumberOfParticles(unsigned int newval)
  {MIN_NUMBER_OF_PARTICLES = newval;}
  unsigned int GetMinNumberOfParticles()
  {return MIN_NUMBER_OF_PARTICLES;}
  
  void SetPruneProbabilityThreshold(double newval)
  {PRUNE_PROBABILITY_THRESHOLD = newval;}
  double GetPruneProbabilityThreshold()
  {return PRUNE_PROBABILITY_THRESHOLD;}

  void SetErasePartiallyInitFeatureAfterThisManyAttempts(unsigned int newval)
  {ERASE_PARTIALLY_INIT_FEATURE_AFTER_THIS_MANY_ATTEMPTS = newval;}
  unsigned int GetErasePartiallyInitFeatureAfterThisManyAttempts()
  {return ERASE_PARTIALLY_INIT_FEATURE_AFTER_THIS_MANY_ATTEMPTS;}

  void SetCameraWidth(unsigned int newval)
  {CAMERA_WIDTH = newval;}
  unsigned int GetCameraWidth() const
  {return CAMERA_WIDTH;}

  void SetCameraHeight(unsigned int newval)
  {CAMERA_HEIGHT = newval;}
  unsigned int GetCameraHeight() const
  {return CAMERA_HEIGHT;}

  void SetInitFeatureSearchUStart(unsigned int newval)
  {init_feature_search_ustart = newval;}
  unsigned int GetInitFeatureSearchUStart() const
  {return init_feature_search_ustart;}

  void SetInitFeatureSearchVStart(unsigned int newval)
  {init_feature_search_vstart = newval;}
  unsigned int GetInitFeatureSearchVStart() const
  {return init_feature_search_vstart;}

  void SetInitFeatureSearchUFinish(unsigned int newval)
  {init_feature_search_ufinish = newval;}
  unsigned int GetInitFeatureSearchUFinish() const
  {return init_feature_search_ufinish;}
  
  void SetInitFeatureSearchVFinish(unsigned int newval)
  {init_feature_search_vfinish = newval;}
  unsigned int GetInitFeatureSearchVFinish() const
  {return init_feature_search_vfinish;}
  
  void SetInitFeatureSearchRegionDefinedFlag(bool newstate)
  {init_feature_search_region_defined_flag = newstate;}
  bool GetInitFeatureSearchRegionDefinedFlag() const
  {return init_feature_search_region_defined_flag;}
  
  bool GetLocationSelectedFlag() const
  {return robot->location_selected();}
  
  unsigned int GetUU() const
  {return robot->get_uu();}
  
  unsigned int GetVV() const
  {return robot->get_vv();}
  
  const Partially_Initialised_Feature_Measurement_Model 
    *GetDefaultFeatureTypeForInitialisation() const
    {return default_feature_type_for_initialisation;}
  
  const VW::Timer &GetTimer0() const {return timer0;}
  const VW::Timer &GetTimer1() const {return timer1;}
  void ResetTimer1() {timer1.Start();}
  
  const Scene_Single *GetScene() const {return scene;}
  Scene_Single *GetSceneNoConst() const {return scene;}
  
  const Robot *GetRobot() const {return robot;}
  Robot *GetRobotNoConst() const {return robot;}

  /** What is the total number of features (full and partially-initialised)
  in the system? */
  unsigned int GetNumberOfFeatures() const
  {return GetScene()->get_feature_list().size();}
  
  /** How many features are currently partially initalised? */
  unsigned int GetNumberOfPartiallyInitialisedFeatures() const
  {return GetScene()->get_feature_init_info_vector().size();}

  /** How many features are currently selected? */
  unsigned int GetNumberOfSelectedFeatures() const
  {return GetScene()->get_selected_feature_list().size();}

  /** How many features were matched in the last call to
  GoOneStep()? */
  unsigned int GetNumberOfVisibleFeatures() const
  {return number_of_visible_features;}

  /** How many features were matched in the last call to
  GoOneStep()? */
  unsigned int GetNumberOfMatchedFeatures() const
  {return number_of_matched_features;}

  /** What was the velocity estimated in the last call to 
  GoOneStep()? */
  const VNL::VectorFixed<3, double>& Velocity() const
  {return velocity;}

};

#endif
