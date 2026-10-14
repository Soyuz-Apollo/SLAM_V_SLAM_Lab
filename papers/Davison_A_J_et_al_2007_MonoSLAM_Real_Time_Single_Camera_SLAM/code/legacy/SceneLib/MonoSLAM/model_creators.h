/*  SceneLib: software for real-time SLAM

    model_creators.h
    Copyright (C) 2005 University of Oxford

    Author
    Paul Smith
    pas@robots.ox.ac.uk

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

#ifndef _monoslam_model_creators_h_
#define _monoslam_model_creators_h_

#include "Scene/models_base.h"

/** 
Class to create an instance of a motion model class given its type string. 
Default creator for MonoSLAM; make your own if you need other models.
@ingroup gMonoSLAM
**/
class MonoSLAM_Motion_Model_Creator : public Motion_Model_Creator
{
 public:
  MonoSLAM_Motion_Model_Creator() {} ///< Constructor. Does nothing.
  ~MonoSLAM_Motion_Model_Creator() {} ///< Destructor. Does nothing.

  Motion_Model* create_model(const std::string& type);
};

/** 
Class to create an instance of a feature measurement model class 
given its type string. 
Default creator for MonoSLAM; make your own if you need other models.
@ingroup gMonoSLAM
**/
class MonoSLAM_Feature_Measurement_Model_Creator 
  : public Feature_Measurement_Model_Creator
{
public:
  /// Constructor.
  MonoSLAM_Feature_Measurement_Model_Creator() {} 
  
  ~MonoSLAM_Feature_Measurement_Model_Creator() {} ///< Destruct. Does nothing.

  Feature_Measurement_Model* create_model(const std::string& type,
					  Motion_Model *motion_model);
};

#endif
