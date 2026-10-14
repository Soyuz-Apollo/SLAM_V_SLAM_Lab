/*  SceneLib: software for real-time SLAM

    monoslam.h
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

// Master control class for MonoSLAM applications


#ifndef _monoslam_monoslam_h_
#define _monoslam_monoslam_h_

#include <vector>

// VW includes
#include <VNL/vector.h>
#include <VW/image.h>
#include <VW/Utils/timer.h>

// Scene includes
#include "Scene/models_base.h"
#include "Scene/kalman.h"
#include "Scene/sim_or_rob.h"

// Local includes
#include "robot.h"


const char *const initial_state_file = "initial_state_cameravw";
const char *const known_features_file = "known_features_cameravw";

/**
@addtogroup gMonoSLAM
@{
*/

/**
Master class which defines and runs the MonoSLAM real-time single
camera SLAM application. This class is GUI-independent, and only provides the
core interface, for clarity and simplicity. The class MonoSLAMInterface
derives from this and should usually be used instead, as it provides the rest
of the interface.
*/
class MonoSLAM
{

 public:
  MonoSLAM(const std::string& initialisation_file,
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
           unsigned int erase_partially_init_feature_after_this_many_attempts);
 
  // At the moment I'm really assuming we only have one MonoSLAM class per
  // program so this destructor doesn't do the things it should
  virtual ~MonoSLAM() {};

  /*******************The main function which does one step*******************/
 public:
  // This function can be overridden by a derived class if necessary
  bool GoOneStep(const VW::ImageMono<unsigned char> *new_image,
                 double delta_t,
                 bool currently_mapping_flag);

  /**********Functions which are called by the main loop or from outside******/
 public:
  // Initialise feature manually at selected position
  void InitialiseFeature();

  // Find a good new feature and initialise it
  bool AutoInitialiseFeature(const VNL::Vector<double> &u, double delta_t);

  // Delete the currently marked feature from Scene
  void DeleteFeature();

  // Save the currently selected image patch to file
  void SavePatch();

  // Print the camera position
  void PrintRobotState();

  // Make measurements to initialise features
  void MatchPartiallyInitialisedFeatures();

  /******************************Data members*********************************/
 protected:

  /******************************Main classes*********************************/

  // This will be the member of feature_measurement_model_array which we will
  // use for initialisation
  Partially_Initialised_Feature_Measurement_Model 
  *default_feature_type_for_initialisation;

  Sim_Or_Rob *sim_or_rob;
  Robot *robot;
  Scene_Single *scene;
  Kalman *kalman;

  /******************************Parameters***********************************/

  // Image dimensions
  unsigned int CAMERA_WIDTH;
  unsigned int CAMERA_HEIGHT;

  // Parameters for map management
  unsigned int NUMBER_OF_FEATURES_TO_SELECT;
  unsigned int NUMBER_OF_FEATURES_TO_KEEP_VISIBLE;
  unsigned int MAX_FEATURES_TO_INIT_AT_ONCE;

  // Parameters for feature initialisation
  // Maximum and minimum feature depth
  double MIN_LAMBDA;
  double MAX_LAMBDA;
  unsigned int NUMBER_OF_PARTICLES;
  // Initialise as point when standard deviation / depth is less than this
  double STANDARD_DEVIATION_DEPTH_RATIO;
  // Give up on particles if number drops below this
  unsigned int MIN_NUMBER_OF_PARTICLES;
  // Prune particle if probability/(number of particles) falls below this 
  double PRUNE_PROBABILITY_THRESHOLD;
  unsigned int 
    ERASE_PARTIALLY_INIT_FEATURE_AFTER_THIS_MANY_ATTEMPTS;

  // Corners of box we search for new features: save so we can display it
  int init_feature_search_ustart;
  int init_feature_search_vstart; 
  int init_feature_search_ufinish; 
  int init_feature_search_vfinish; 
  bool init_feature_search_region_defined_flag;

  // Timing information for performance analysis
  // Global timer reset at program start
  VW::Timer timer0;
  // Frame timer reset as each new frame is acquired
  VW::Timer timer1;

  // Remember this so that the user can query it
  unsigned int number_of_visible_features;
  unsigned int number_of_matched_features;
  VNL::VectorFixed<3, double> velocity;
};

/**
@}
*/

#endif
