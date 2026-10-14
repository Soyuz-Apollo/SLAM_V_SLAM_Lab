/*  SceneLib: software for real-time SLAM

    model_creators.cpp
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

#include <iostream>

#include "model_creators.h"

#include "models_zeroorder.h"
#include "models_impulse.h"

#include "models_wideangle.h"


/** Returns an instance of the requested motion model (using
 <code>new</code>). Returns NULL if there was no match.
**/
Motion_Model* MonoSLAM_Motion_Model_Creator::create_model(const std::string& type)
{
  // Try creating each model that we can and see if the name is the same
  std::cout << "Creating a " << type << " motion model" << std::endl;
  Motion_Model* pModel;
	
  pModel = new ZeroOrder_ThreeD_Motion_Model;
  if(pModel->motion_model_type == type)
    return pModel;
  else
    delete pModel;

  pModel = new Impulse_ThreeD_Motion_Model;
  if(pModel->motion_model_type == type)
    return pModel;
  else
    delete pModel;

  return NULL;
}


/** Returns an instance of the requested feature measurement model
    (using <code>new</code>). Returns NULL if there was no match.
**/
Feature_Measurement_Model* MonoSLAM_Feature_Measurement_Model_Creator::
create_model(const std::string& type, Motion_Model *motion_model)
{
  // Try creating each model that we can and see if the name is the same
  std::cout << "Creating a " << type << " feature measurement model" 
            << std::endl;
  Feature_Measurement_Model* pModel;

  pModel = new
    Fully_Init_Wide_Point_Feature_Measurement_Model(motion_model);
  if(pModel->feature_type == type)
    return pModel;
  
  // The partially-initialised model also needs the fully-initalised
  // one that we have just created. Don't delete it - just store it
  Feature_Measurement_Model* pFullModel = pModel;
  pModel = new
    Line_Init_Wide_Point_Feature_Measurement_Model(motion_model, pFullModel);
  if(pModel->feature_type == type)
    return pModel;
  else
  {
    delete pModel;
    delete pFullModel;
  }

  return NULL;
}
