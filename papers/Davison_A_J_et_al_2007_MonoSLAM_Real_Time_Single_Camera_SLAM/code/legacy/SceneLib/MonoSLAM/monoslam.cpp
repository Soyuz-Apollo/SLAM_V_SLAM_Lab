/*  SceneLib: software for real-time SLAM

    monoslam.cpp
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

#include <fstream>

// VW includes
#include "VNL/Algo/cholesky.h"
#include "VNL/Algo/determinant.h"

// Scene Includes
#include "Scene/control_general.h"
#include "Scene/exceptions.h"
#include "Scene/settings.h"


// Local
#include "nonoverlappingregion.h"
#include "threeddraw.h"
#include "monoslam.h"
#include "model_creators.h"
#include "models_wideangle.h"
#include "identifier_imagemono.h"

#ifdef DEBUGDUMP
#undef DEBUGDUMP
#endif
#define DEBUGDUMP false

using namespace std;

/** Constructor. 
@param initialisation_file The initialisation file to read. This specifies
the motion- and feature-measurement models to use, the initial state and
known features.
@param mm_creator The factory to use to create motion models.
@param fmm_creator The factory to use to create feature measurement models
@param imm_creator The factory to use to create internal measurement models
@param number_of_features_to_select The number of features to select for
measurement at each time step
@param number_of_features_to_keep_visible The requried number of visible
features. If fewer than this number are visible at any time step, the creation
of a new feature is initiated
@param min_lambda The minimum distance from the camera (in metres) for a new
feature
@param max_lambda The maximum distance from the camera (in metres) for a new
feature
@param number_of_particles The number of particles to use for new features
(distributed evenly in space between min_lambda and max_lambda)
@param standard_deviation_depth_ratio The ratio between standard deviation and
mean to use to identify when a partially-initialised feature should be
converted to a fully-initialised one
@param min_number_of_particles The minimum number of particles below which a
partially-initalised feature is deleted
@param prune_probability_threshold The threshold below which a particle with
low probability is deleted
@param erase_partially_init_feature_after_this_many_attempts The number of
failed match attempts before a partially initialised feature is deleted.
*/
MonoSLAM::MonoSLAM(
  const string& initialisation_file,
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
  : NUMBER_OF_FEATURES_TO_SELECT(number_of_features_to_select),
    NUMBER_OF_FEATURES_TO_KEEP_VISIBLE(number_of_features_to_keep_visible),
    MAX_FEATURES_TO_INIT_AT_ONCE(max_features_to_init_at_once),
    MIN_LAMBDA(min_lambda),
    MAX_LAMBDA(max_lambda),
    NUMBER_OF_PARTICLES(number_of_particles),
    STANDARD_DEVIATION_DEPTH_RATIO(standard_deviation_depth_ratio),
    MIN_NUMBER_OF_PARTICLES(min_number_of_particles),
    PRUNE_PROBABILITY_THRESHOLD(prune_probability_threshold),
    ERASE_PARTIALLY_INIT_FEATURE_AFTER_THIS_MANY_ATTEMPTS(
    erase_partially_init_feature_after_this_many_attempts),
    number_of_visible_features(0),
    number_of_matched_features(0)
{
  // Create the Settings class by reading from the initialisation file
  ifstream stream(initialisation_file.c_str());
  if(!stream)
  {
    ostringstream os;
    os << "Unable to open initialisation file \""
       << initialisation_file
       << "\"." << endl;
    throw Scene::InitialisationError(os.str());
  }
  
  Settings settings(stream);

  // Create the Scene class. This also constructs the motion model and 
  // internal measurement models and sets the initial state
  scene = new Scene_Single(settings, mm_creator, imm_creator);

  // Now sort out the feature types
  string feature_init_type = settings.get_entry(
    "Models", "NewFeatureMeasurementModel");
  Feature_Measurement_Model* fm_model =
    fmm_creator->create_model(feature_init_type, scene->get_motion_model());

  if(fm_model == NULL)
  {
    ostringstream os;
    os << "Unable to create a feature measurement motion model of type \"" 
       << feature_init_type 
       << "\" as requested in initalisation file \"" << initial_state_file 
       << "\". " << endl;
    throw Scene::InitialisationError(os.str());
  }

  // Initialise this motion model
  fm_model->read_parameters(settings);
  
  // Check that this is a partially-initialised feature type
  if(fm_model->fully_initialised_flag)
  {
    ostringstream os;
    os << "Feature measurement motion model \"" << feature_init_type 
       << " as requested in initalisation file \"" << initial_state_file 
       << "\" is not a partially-initialised feature type. " << endl;
    throw Scene::InitialisationError(os.str());
  }

  default_feature_type_for_initialisation =
    dynamic_cast<Partially_Initialised_Feature_Measurement_Model*>(fm_model);

  // We hope that features are viewed through a camera! If so,
  // the feature measurement class should derive from
  // Camera_Feature_Measurement_Model
  const Camera_Feature_Measurement_Model* cfmm =
    dynamic_cast<const Camera_Feature_Measurement_Model*>(fm_model);

  if(cfmm == 0)
  {
    // Oops - the feature measurement model is not derived from
    // Camera_Feature_Measurement_Model!
    ostringstream os;
    os << "The default feature measurement motion model \""
       << fm_model->feature_type
       << "\" is not derived from Camera_Feature_Measurement_Model!" << endl;
    throw Scene::InitialisationError(os.str());
  }

  CAMERA_WIDTH = cfmm->get_camera().ImageWidth();
  CAMERA_HEIGHT = cfmm->get_camera().ImageHeight();

  kalman = new Kalman;
  robot = new Robot();
  sim_or_rob = dynamic_cast<Sim_Or_Rob*>(robot);

  // Initialise any known features
  initialise_known_features(settings, fmm_creator, sim_or_rob, scene);

  // Save the time the program was started
  timer0.Start();

  // Various flags
  init_feature_search_region_defined_flag = false;
}

/**
Step the MonoSLAM application on by one frame. This should be called every time
a new frame is captured (and care should be taken to avoid skipping frames).
Before calling this function, Scene_Single::load_new_image() should be called
(e.g. using
<code>monoslaminterface.GetRobotNoConst()->load_new_image()</code>), since the
first parameter to GoOneStep() is currently ignored.
@param delta_t The time between frames
@param currently_mapping_flag Set to be true if new features should be detected
and added to the map.

GoOneStep() performs the following processing steps:
- Kalman filter prediction step (by calling Kalman::predict_filter_fast(),
with a zero control vector)
- Select a set of features to make measurements from (set by
SetNumberOfFeaturesToSelect())
- Predict the locations and and make measurements of those features
- Kalman filter update step
- Delete any bad features (those that have repeatedly failed to be matched)
- If we are not currently initialising a enough new features, and the camera is
translating, and <code>currently_mapping_flag</code> is set, initialise a new
feature somewhere sensible
- Update the partially-initialised features
*/
bool MonoSLAM::GoOneStep(const VW::ImageMono<unsigned char>*,
                         double delta_t, bool currently_mapping_flag)
{
  // Nullify image selection
  robot->nullify_image_selection();

  init_feature_search_region_defined_flag = false;


  // Control vector of accelerations
  VNL::Vector<double> u(3);
  u.Fill(0.0);

  sim_or_rob->set_control(u, delta_t);

  // Record the current position so that I can estimate velocity
  // (We can guarantee that the state vector has position; we can't 
  // guarantee that it has velocity.)
  VNL::Vector<double> xv = scene->get_xv();
  scene->get_motion_model()->func_xp(xv);
  VNL::VectorFixed<3, double> prev_xp_pos = 
    scene->get_motion_model()->get_xpRES().Extract(3);
  
  // Prediction step
  kalman->predict_filter_fast(scene, u, delta_t);

  number_of_visible_features =
    scene->auto_select_n_features(NUMBER_OF_FEATURES_TO_SELECT);

  if(Scene_Single::STATUSDUMP) scene->print_selected_features();

  if (scene->get_no_selected() != 0)
  {
//    scene->predict_measurements();
    if (DEBUGDUMP) cout << "Time after predicting measurements " 
                        << timer1 << endl;

    // Calls function in control_general.cc
    number_of_matched_features = make_measurements(scene, sim_or_rob);

    if (DEBUGDUMP) cout << "Time after making measurements " << timer1 << endl;

    if (scene->get_successful_measurement_vector_size() != 0) {
      kalman->total_update_filter_slow(scene);

      if (DEBUGDUMP) cout << "Time after total_update_filter " 
			  << timer1 << endl;

      scene->normalise_state();

      if (DEBUGDUMP) cout << "Time after normalise_state " << timer1 << endl;
    }
  }

  scene->delete_bad_features();

  // Let's enforce symmetry of covariance matrix...
  // Add to transpose and divide by 2
  VNL::Matrix<double> Pxx(scene->get_total_state_size(),
                          scene->get_total_state_size());
  scene->construct_total_covariance(Pxx);
  VNL::Matrix<double> PxxT = Pxx.Transpose();

  Pxx.Update(Pxx * 0.5 + PxxT * 0.5);
  scene->fill_covariances(Pxx);

  // Look at camera speed estimate
  // Get the current position and estimate the speed from it
  xv = scene->get_xv();
  scene->get_motion_model()->func_xp(xv);
  VNL::VectorFixed<3, double> xp_pos = 
    scene->get_motion_model()->get_xpRES().Extract(3);
  velocity = (xp_pos - prev_xp_pos) / delta_t;
  double speed = sqrt(velocity.SquaredMagnitude());
  if (DEBUGDUMP) cout << "Camera speed " << speed << " ms^-1 " << endl;

  if (speed > 0.2 &&
    currently_mapping_flag) {
    if (number_of_visible_features < NUMBER_OF_FEATURES_TO_KEEP_VISIBLE && 
        scene->get_feature_init_info_vector().size() <
          (unsigned int)(MAX_FEATURES_TO_INIT_AT_ONCE)) {
      AutoInitialiseFeature(u, delta_t);
    }
  }

  if (DEBUGDUMP) cout << "Time after matching point features: " 
                      << timer1 << endl;

  MatchPartiallyInitialisedFeatures();

  if (DEBUGDUMP) cout << "Time after matching partially init. feature: " 
                      << timer1 << endl;

  return true;
}


/**
Initialise a feature at the currently-selected image location. The image
selection is usually either chosen automatically with
Robot::set_image_selection_automatically() or manually by the user by calling
Robot::set_image_selection().
*/
void MonoSLAM::InitialiseFeature()
{
  VNL::Vector<double> 
    z(default_feature_type_for_initialisation->MEASUREMENT_SIZE);

  if (DEBUGDUMP) cout << "Making measurement to initialise new " 
        << default_feature_type_for_initialisation->feature_type 
        << " feature." << endl;

  // Fill in z and id for the currently-selected feature
  Identifier id;
  if (robot->make_initial_measurement_of_feature(z, id,
          default_feature_type_for_initialisation) == false)
    cerr << "Failed in initialisation of feature." << endl;
  else
  {
    const FeatureInitInfo* feat =
      scene->add_new_partially_initialised_feature(
        id, z, default_feature_type_for_initialisation);
    if (DEBUGDUMP) cout << "Initialising feature with label "
                        << feat->get_fp()->get_label()
                        << " at z = " << z << endl;

    // And set up a particle distribution for this partially initialised
    // feature
    double lambda_step = (1.0 / double(NUMBER_OF_PARTICLES)) * 
      (MAX_LAMBDA - MIN_LAMBDA);
    const double uniform_probability = 1.0 / double(NUMBER_OF_PARTICLES);
    // Initialise particle set with uniform prior
    VNL::Vector<double> lambda(1);
    lambda(0) = MIN_LAMBDA;
    for (unsigned int i = 0; i < NUMBER_OF_PARTICLES; i++) {
      scene->add_particle_to_newest_partially_init_feature(lambda, 
                uniform_probability);

      lambda(0) += lambda_step;
    }
  }
}


/**
Initialise a feature at a position determined automatically. This predicts
where the image centre will be soon (in 10 frames), and tries initialising a
feature near there. This may not necessarily give a new feature - the score
could not be good enough, or no suitable non-overlapping region might be found.
@param u The input control vector (zero in the MonoSLAM application)
@param delta_t The time between frames
@returns <code>true</code> on success, or <code>false</code> on failure (i.e.
if no non-overlapping region is found, or if no feature could be found with a
good enough score).
*/
bool MonoSLAM::AutoInitialiseFeature(const VNL::Vector<double> &u,
                                     double delta_t)
{  
  // A cute method for deciding where to centre our search for a new feature
  // Idea: look for a point in a position that we expect to be near the
  // image centre soon

  // Predict the camera position a few steps into the future
  const unsigned int FEATURE_INIT_STEPS_TO_PREDICT = 10;
  

  // Project a point a "reasonable" distance forward from there along
  // the optic axis
  const double FEATURE_INIT_DEPTH_HYPOTHESIS = 2.5; 

  // First find a suitable patch
  const double SUITABLE_PATCH_SCORE_THRESHOLD = 20000;

  VNL::Vector<double> local_u (scene->get_motion_model()->CONTROL_SIZE);
  if (FindNonOverlappingRegion(scene,
                               u,
                               delta_t,
                               default_feature_type_for_initialisation,
                               CAMERA_WIDTH,
                               CAMERA_HEIGHT,
                               BOXSIZE,
                               init_feature_search_ustart,
                               init_feature_search_vstart,
                               init_feature_search_ufinish,
                               init_feature_search_vfinish,
                               FEATURE_INIT_STEPS_TO_PREDICT,
                               FEATURE_INIT_DEPTH_HYPOTHESIS)) {
    init_feature_search_region_defined_flag = true;
    if (robot->set_image_selection_automatically(
      init_feature_search_ustart, init_feature_search_vstart,
      init_feature_search_ufinish, init_feature_search_vfinish) >
        SUITABLE_PATCH_SCORE_THRESHOLD) {
      // Then initialise it
      InitialiseFeature();
    }
    else {
      if (DEBUGDUMP) cout << "Score not good enough; feature not initialised."
                          << endl;
      return false;
    }
  }
  else {
      if (DEBUGDUMP) 
        cout << "No non-overlapping region found; feature not initialised." 
             << endl;
      return false;
  }
  return true;
}

/**
Delete the currently-marked feature. This calls Scene_Single::delete_feature().
*/
void MonoSLAM::DeleteFeature()
{
  scene->delete_feature();
}

/**
Save the currently-selected patch as an image file. This calls
Robot::write_patch().
*/
void MonoSLAM::SavePatch()
{
  robot->write_patch();
}


/**
Print out the current robot state \f$ x_v \f$ and covariance \f$ P_{xx} \f$.
This calls Scene_Single::print_robot_state().
*/
void MonoSLAM::PrintRobotState()
{
  scene->print_robot_state();
}


/**
Try to match the partially-initialised features, then update their
distributions. If possible (if the standard deviation ratio \f$ \Sigma_{11} /
\mu_1 \f$ is small enough) this also converts them to fully-initalised 
features. In addition, this also calls 
Scene_Single::delete_partially_initialised_features_past_sell_by_date() to
delete any that have not collapses fast enough.
*/
void MonoSLAM::MatchPartiallyInitialisedFeatures()
{
  // Go through all partially initialised features and decide which ones
  // to try to measure; predict measurements for these
  scene->predict_partially_initialised_feature_measurements();

  if (DEBUGDUMP) cout << "Match: time after setting up search stuff " 
                      << timer1 << endl;

  // Loop through partially initialised features
  for (vector<FeatureInitInfo>::iterator feat = 
    scene->get_feature_init_info_vector_noconst().begin();
       feat != scene->get_feature_init_info_vector().end(); feat++)
  {

    if (feat->get_making_measurement_on_this_step_flag())
    {

      if (DEBUGDUMP) cout
        << "Making partially initialised measurements for feature label "
        << feat->get_fp()->get_label() << endl;

      // Make search for this feature over overlapping ellipses of particles
      robot->measure_feature_with_multiple_priors(
        feat->get_fp()->get_identifier(),
        feat->get_particle_vector_noconst());
    }
  }

  if (DEBUGDUMP) cout << "Match: time after search " << timer1 << endl;

  // Update particle distributions with measurements, normalise, prune, etc.
  scene->update_partially_initialised_feature_probabilities(
    PRUNE_PROBABILITY_THRESHOLD);

  // Now go through the features again and decide whether to convert
  // any to fully-initialised features
  // Do this here rather than generically in Scene because the test
  // for conversion is specific to this application

  for (vector<FeatureInitInfo>::iterator feat =
    scene->get_feature_init_info_vector_noconst().begin();
    feat != scene->get_feature_init_info_vector().end(); feat++)
  {
    // Only try to convert if we've been making measurements!
    if (feat->get_making_measurement_on_this_step_flag())
    {
      double mean_sd_ratio = sqrt(feat->get_covariance()(0, 0)) /
                                            feat->get_mean()(0);

      if (DEBUGDUMP) cout << "Checking whether to convert feature "
                          << feat->get_fp()->get_label() << ": ratio "
                          << mean_sd_ratio << ", using threshold "
                          << STANDARD_DEVIATION_DEPTH_RATIO << endl;

      if (mean_sd_ratio < STANDARD_DEVIATION_DEPTH_RATIO && 
          feat->get_particle_vector().size() > MIN_NUMBER_OF_PARTICLES)
      {
        if (DEBUGDUMP) cout << "Yes, converting feature "
                            << feat->get_fp()->get_label()
                            << " to a fully-initialised feature."
                            << endl;
        feat->get_fp_noconst()->
          convert_from_partially_to_fully_initialised(feat->get_mean(),
                                                      feat->get_covariance(),
                                                      scene);
        scene->get_feature_init_info_vector_noconst().erase(feat--);
      }
    }
  }

  // Go through and get rid of any partially initialised features whose
  // distributions haven't collapsed fast enough 
  scene->delete_partially_initialised_features_past_sell_by_date(
    ERASE_PARTIALLY_INIT_FEATURE_AFTER_THIS_MANY_ATTEMPTS,
    MIN_NUMBER_OF_PARTICLES);
}
