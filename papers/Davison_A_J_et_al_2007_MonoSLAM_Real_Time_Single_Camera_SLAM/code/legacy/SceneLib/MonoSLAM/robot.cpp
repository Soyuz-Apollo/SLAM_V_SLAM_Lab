/*  SceneLib: software for real-time SLAM

    robot.cpp
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

#include "VNL/Algo/cholesky.h"
#include "VW/Utils/timer.h"
#include "Scene/models_base.h"
#include "SceneImproc/improc.h"
#include "Scene/scene_single.h"
#include "robot.h"

#ifdef DEBUGDUMP
#undef DEBUGDUMP
#endif
#define DEBUGDUMP false

using namespace std;

/*******************************Robot Class***********************************/

// Constructor

Robot::Robot()
  : Sim_Or_Rob("ROBOT")
{
  location_selected_flag = false;
}

Robot::~Robot()
{

}



/** Make a measurement of a feature. This function calls elliptical_search() to
find the best match within three standard deviations of the predicted location.
@param id The identifier for this feature (in this case an image patch)
@param z The best image location match for the feature, to be filled in by this
function
@param h The expected image location
@param S The expected location covariance, used to specify the search region.
*/
bool Robot::measure_feature(Identifier id,
                            VNL::Vector<double> &z, 
                            const VNL::Vector<double> &h, 
                            const VNL::Matrix<double> &S)
{
  VNL::Cholesky<double> S_cholesky(S);
  VNL::Matrix<double> Sinv = S_cholesky.Inverse();

  unsigned int u_found, v_found;

  if(elliptical_search(*image,
                       *dynamic_cast<ImageMonoExtraData*>(id),
                       h, Sinv, &u_found, &v_found, BOXSIZE) != true)
  {
    if(Scene_Single::STATUSDUMP) cout << "Feature not successfully matched." 
                                      << endl;
    return false;
  }

  z.Put(0, (double) u_found);
  z.Put(1, (double) v_found);

  return true;
}

/**
Initialise a known feature. In this case it is assumed that a known feature
is an image patch to be loaded from a file <code>known_patch?.pgm</code>.
*/
Identifier Robot::initialise_known_feature(
  Feature_Measurement_Model*,
  const VNL::Vector<double> &,
  const unsigned int known_feature_label)
{
    ostringstream os;
    os << known_point_patch_stem << known_feature_label << ".pgm";

    string name = os.str();

    cout << "Reading patch " << name << endl;

    ImageMonoExtraData *patch = new ImageMonoExtraData;
    if(patch->ReadImage(name) == false)
    {
        delete patch;
        patch = NULL;
    }

    return patch;
}

/**
Initialise a known feature, in this case by loading an image file. The name of
the file is read from the Settings Section passed to this function, with the
entry <code>Identifier=</code>.
*/
Identifier Robot::initialise_known_feature(
  Feature_Measurement_Model*,
  const VNL::Vector<double> &,
  const Settings::Section& settings)
{
  string name = settings.get_entry("Identifier");

  cout << "Reading patch " << name << endl;

  ImageMonoExtraData *patch = new ImageMonoExtraData;
  if(patch->ReadImage(name) == false)
  {
      delete patch;
      patch = NULL;
  }

  return patch;
}

void Robot::load_new_image(const VW::ImageMono<unsigned char> *imageptr)
{
  assert(imageptr != NULL);

  image = imageptr;
}

// Little service function to copy image region centred at uu, vv into patch
void copy_into_patch(const VW::ImageMono<unsigned char> *im,
                     VW::ImageMono<unsigned char> *patch,
                     const unsigned int uu,
                     const unsigned int vv)
{
  for(unsigned int r = 0; r < BOXSIZE; r++)
    for(unsigned int c = 0; c < BOXSIZE; c++) {
      patch->SetPixel(c, r,
                      im->GetPixel(c + uu - (BOXSIZE - 1) / 2,
                                   r + vv - (BOXSIZE - 1) / 2));
    }
}

/**************************Write Image Patch to Disk**************************/

/**
Save the currently-selected patch to a file.
@param name The filename to use.
*/
void Robot::write_patch(const string& name) const
{
  VW::ImageMono<unsigned char> *hip =
    new VW::ImageMono<unsigned char>(BOXSIZE, BOXSIZE);

  if (location_selected_flag)
  {
    /* Copy the selected patch to the save space patch */
    copy_into_patch(image, hip, uu, vv);
    hip->WriteImage(name);
    cout << "Written patch " << name << endl;
  }
  else
  {
    cout << "No patch selected." << endl;
  }

  delete hip;
}

/**
Make the initial measurement of the currently-selected feature. This fills in
the location and the identifier (a copy of the image patch) for the current
feature. The feature location is set using set_image_selection_automatically()
or manually by the user using set_image_selection(). This function just calls
partially_initialise_point_feature() to fill in the measurement and identifier.
@param z The measurement vector to be filled in.
@param id_ptr The identifier to be filled in.
@returns <code>true</code> on success, or <code>false</code> on failure (e.g.
no patch is currently selected).
*/
bool Robot::make_initial_measurement_of_feature(
  VNL::Vector<double> &z, 
  Identifier& id_ptr,
  Partially_Initialised_Feature_Measurement_Model *)
{
  id_ptr = partially_initialise_point_feature(z);

  if (id_ptr != NULL)
    return true;
  else
    return false;
}

/** 
Creates a new ImageMonoExtraData to represent the currently-selected image
patch, and also returns its location in the parameter z. The current
image patch is set using set_image_selection_automatically() or manually by the
user using set_image_selection().
@param z The measurement vector to be filled in
@returns The ImageMonoExtraData holding this image patch
information (created using <code>new</code>, so must be
<code>delete</code>d elsewhere), or <code>NULL</code> if no patch is currently
selected.
*/
ImageMonoExtraData* Robot::partially_initialise_point_feature(
  VNL::Vector<double> &z)
{
  if (location_selected_flag) // Patch selected
  {
    // Go and initialise it in scene + 3D  
    ImageMonoExtraData *hip = 
      new ImageMonoExtraData(BOXSIZE, BOXSIZE);

    // Final save of fixated image patch
    copy_into_patch(image, hip, uu, vv);

    // And set measurement
    z.Put(0, uu);
    z.Put(1, vv);

    //    cout << "z" << z;

    return hip;
  }
  else
  {
    cerr << "No patch selected." << endl;
    return NULL;
  }
}

/** Search a region for the best image patch, and set the current selection to
this. This just calls find_best_patch_inside_region() to find a patch using the
Shi and Tomasi criterion.
@param ustart The x-cordinate of the start of the region
@param vstart The y-cordinate of the start of the region
@param ufinish The x-cordinate of the end of the region
@param vfinish The y-cordinate of the end of the region
@returns The smallest eigenvalue of the best patch (high means good for
correlation)
*/
double Robot::set_image_selection_automatically(unsigned int ustart,
                                                unsigned int vstart,
                                                unsigned int ufinish,
                                                unsigned int vfinish)
{
  double evbest = 0;

  find_best_patch_inside_region(*image, &uu, &vv, &evbest, BOXSIZE, 
                                ustart, vstart, ufinish, vfinish); 
  location_selected_flag = true;
  if (DEBUGDUMP) cout << "Found patch with score " << evbest << endl;

  return evbest;
}


/** Make measurements of a feature which is represented by a set of particles.
This is typically a partially-initialised feature (see
Partially_Initialised_Feature_Measurement_Model), where the particles represent
the probability distribution in the direction of the free parameters.
@param id The Identified for this feature (in this case this will be an image
patch)
@param particle_vector The vector of particles. The covariance of each particle
is used to define the region over which the feature is matched and measured.
*/
void Robot::measure_feature_with_multiple_priors(
  Identifier id, 
  std::vector<Particle> &particle_vector)
{
  // VW::Timer timerlocal;

  //  cout << "MFWMP timer start: " << timerlocal << endl;

  SearchMultipleOverlappingEllipses ellipse_search(
    *image, *dynamic_cast<ImageMonoExtraData*>(id), BOXSIZE);

  for (vector<Particle>::const_iterator part = particle_vector.begin();
       part != particle_vector.end();
       part++)
  {
    ellipse_search.add_ellipse(part->get_SInv(), part->get_h());

        if (DEBUGDUMP)  cout << "Particle at " << part->get_lambda()
                             << " h " << part->get_h()
                             << " SInv " << part->get_SInv() << endl;
  }

  // cout << "MFWMP timer before search_multiple: " << timerlocal << endl;

  ellipse_search.search();

  // cout << "MFWMP timer after search_multiple: " << timerlocal << endl;

  // Turn results into matrix forms
  SearchMultipleOverlappingEllipses::SearchData::const_iterator e =
    ellipse_search.begin();
  for (vector<Particle>::iterator it = particle_vector.begin();
       it != particle_vector.end();
       it++, e++)
  {
    if(e->result_flag)
    {
      // Save the measurement location back into the particle
      VNL::VectorFixed<2,double> z_local(e->result_u, e->result_v);
      it->set_z(z_local);
      it->set_successful_measurement_flag(true);
    }
    else
    {
      it->set_successful_measurement_flag(false);
    }
  }

  // cout << "MFWMP timer end: " << timerlocal << endl;
}


