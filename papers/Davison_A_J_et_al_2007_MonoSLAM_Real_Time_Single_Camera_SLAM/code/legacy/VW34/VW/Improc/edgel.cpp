/** 
 * @file   edgel.cpp
 * @author ID Reid
 * @date   02/08/2002
 *
 *              (c) Copyright University of Oxford
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#pragma warning(disable:4786)

#include "edgel.h"

using namespace std;
using namespace VW;

//////////////////////////////////////////

Edgel::Edgel(float fs, float fa, float fx, float fy,
	     int iia, int iix, int iiy) : Point2D(fx,fy)
{
  s = fs;   a = fa;
  ia = iia; ix = iix; iy = iiy;
  linked = 0;
}


//////////////////////////////////////////

Edgel::~Edgel()
{
}


//////////////////////////////////////////

EdgeData::EdgeData()
{
  // Constructor
}

//////////////////////////////////////////

EdgeData::~EdgeData() 
{
// Destructor
  // edgels is a vector of pointers to edgels. Hence we
  // should delete the edgels. No need to delete the
  // vector here, as the system will do this.
  for(unsigned int i=0; i< edgels.size();i++)  delete edgels[i];
  // Similar here. BUT NOTE that it is assumed that all of the
  // actual edgels that were linked into strings were originally
  // in the vector of edgels.
  for(unsigned int j=0; j< strings.size();j++)  delete strings[j];
  
}

void EdgeData::Clear() 
{
// This is not the destructor, but appears to do more.
// In the destructor, the system eventually deletes the
// vectors "edgels" and "strings", here they are not
// deleted, but explicitly reset to size zero.
  for(unsigned int i=0; i< edgels.size();i++)  delete edgels[i];
  edgels.clear();
  for(unsigned int j=0; j< strings.size();j++)  delete strings[j];
  strings.clear();
}

