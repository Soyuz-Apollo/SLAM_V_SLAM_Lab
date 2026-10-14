/*  SceneLib: software for real-time SLAM

    robot.h
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

// Robot class to interface with hardware


#ifndef _monoslam_robot_h_
#define _monoslam_robot_h_

#include "VW/image.h"
#include "Scene/sim_or_rob.h"
#include "Scene/init_feature.h"
#include "identifier_imagemono.h"

/**********************************Constants**********************************/

const unsigned int BOXSIZE = 11;      ///< Side length of square image patches

const char *const known_point_patch_stem = "known_patch";

/*****************************************************************************/

/**
Class to model a Robot that is a hand-held greyscale camera, such as for the
MonoSLAM real-time single-camera SLAM. This class measures features of with an
Identifier of type ImageMonoExtraData, and has no control input.
@ingroup gMonoSLAM
*/
class Robot : public Sim_Or_Rob {
 public:
  Robot();
  ~Robot();

  // Virtual functions redefined from Sim_Or_Rob
  bool measure_feature(Identifier id, VNL::Vector<double> &z, 
                       const VNL::Vector<double> &h,
                       const VNL::Matrix<double> &S);

  bool set_control(const VNL::Vector<double> &, const double )
    {return true;}
  bool continue_control(const VNL::Vector<double> &, 
                        const double)
    {return true;}
  double wait_for_end_of_motion(const VNL::Vector<double> &) 
    {return 0.0;}

  bool stop_vehicle() 
    {return true;}

  Identifier initialise_known_feature(Feature_Measurement_Model *f_m_m,
                                      const VNL::Vector<double> &yi,
                                      const unsigned int known_feature_label);
  Identifier initialise_known_feature(Feature_Measurement_Model *f_m_m,
                                      const VNL::Vector<double> &yi,
                                      const Settings::Section& settings);

  bool make_initial_measurement_of_feature(VNL::Vector<double> &z,
    Identifier& id_ptr,
    Partially_Initialised_Feature_Measurement_Model *f_m_m);

  virtual ImageMonoExtraData *partially_initialise_point_feature(
    VNL::Vector<double> &z);

  /** Set the current image selection (the co-ordinates of the centre of the
  image patch.
  @param u The x-coordinate of the current image selection
  @param v The y-coordinate of the current image selection
  */
  void set_image_selection(unsigned int u, unsigned int v)
    {uu = u; vv = v; location_selected_flag = true;}
  virtual double set_image_selection_automatically(
    unsigned int ustart,
    unsigned int vstart,
    unsigned int ufinish,
    unsigned int vfinish);
  /// Cancel the current image selection.
  void nullify_image_selection()
    {location_selected_flag = false;}
  /// Is there currently an image location selected?
  bool location_selected() const
    {return location_selected_flag;}
  /// Returns the x-coordinate of the current image selection.
  unsigned int get_uu() const 
    {return uu;}
  /// Returns the y-coordinate of the current image selection.
  unsigned int get_vv() const 
    {return vv;}

  virtual void load_new_image(const VW::ImageMono<unsigned char> *imageptr);


  // Measure feature position for each of a set of Gaussian priors
  // We can do the measurement more efficiently than searching each
  // separately
  virtual void measure_feature_with_multiple_priors(Identifier id, 
    std::vector<Particle> &particle_vector);

  void write_patch(const string& name = "patch.pgm") const;

 protected:

  // Coordinates of currently selected point in image
  unsigned int uu, vv;
  bool location_selected_flag;


  const VW::ImageMono<unsigned char> *image;

};


#endif
