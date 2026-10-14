/** 
 * @file    getplesseycorner.cpp
 * @author  BJ Tordoff
 * @date    25/11/2004
 *
 *              (c) Copyright University of Cambridge
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

#include <VW/Improc/pyramid.h>
#include <VW/Image/imagemono.h>
#include <VW/Utils/timer.h>

#include "getplesseycorner.h"
#include "patchfeature.h"

#define VERBOSE false
#define TWOPI 6.2831853071795
#define PIBYTWO 1.5707963267948

using namespace std;


//////////// GetPlesseyCorner




VW::GetPlesseyCorner::GetPlesseyCorner(const std::vector<VW::CornerFeature>& locations,
				       const VW::ImageMono<unsigned char>& image,
				       unsigned int patch_size,
				       std::vector<VW::PlesseyCorner>& resulting_features)
{
  resulting_features.clear();
  resulting_features.reserve(locations.size());
  
  if (VERBOSE) cerr << "VW::Plessey::GetPatches(yuv): do " << locations.size() << " locations" << endl;
  // This is a temporary hack
  bool use_subpixel = true;

  const unsigned int half_size = patch_size/2;
  const float min_x = image.ROI().Left() + half_size;
  const float min_y = image.ROI().Top() + half_size;
  const float max_x = image.ROI().Right() - 1 - half_size;
  const float max_y = image.ROI().Bottom() - 1 - half_size;

  if (use_subpixel) {
    // Create interpolated patches
    for (unsigned int i=0; i<locations.size(); i++) {
      const float x = locations[i].GetX();
      const float y = locations[i].GetY();
      if ((x>min_x) && (x<max_x) 
	  && (y>min_y) && (y<max_y)) {
        resulting_features.push_back(VW::PlesseyCorner(locations[i]));
	VW::PlesseyCorner& feature = resulting_features[resulting_features.size()-1];
	feature.SetPatchSize(patch_size);
	GetInterpolatedImagePatch(image,x,y,*(feature.GetImagePatch()));
      }
    }
  } else {
    // Create non-interpolated patches
    for (unsigned int i=0; i<locations.size(); i++) {
      const float x = locations[i].GetX();
      const float y = locations[i].GetY();
      if ((x>min_x) && (x<max_x) 
	  && (y>min_y) && (y<max_y)) {
        resulting_features.push_back(VW::PlesseyCorner(locations[i]));
	VW::PlesseyCorner& feature = resulting_features[resulting_features.size()-1];
	feature.SetPatchSize(patch_size);
	GetImagePatch(image,x,y,*(feature.GetImagePatch()));
      }
    }
  }
  if (VERBOSE) cerr << "Done" << endl;
}


VW::GetPlesseyCorner::GetPlesseyCorner(const std::vector<VW::CornerFeature>& locations,
				       const VW::ImageYUV<unsigned char>& image,
				       unsigned int patch_size,
				       std::vector<VW::PlesseyCorner>& resulting_features)
{
  resulting_features.clear();
  resulting_features.reserve(locations.size());
  
  if (VERBOSE) cerr << "VW::Plessey::GetPatches(yuv): do " << locations.size() << " locations" << endl;
  // This is a temporary hack
  bool use_subpixel = true;

  const unsigned int half_size = patch_size/2;
  const float min_x = image.ROI().Left() + half_size;
  const float min_y = image.ROI().Top() + half_size;
  const float max_x = image.ROI().Right() - 1 - half_size;
  const float max_y = image.ROI().Bottom() - 1 - half_size;

  if (use_subpixel) {
    // Create interpolated patches
    for (unsigned int i=0; i<locations.size(); i++) {
      const float x = locations[i].GetX();
      const float y = locations[i].GetY();
      if ((x>min_x) && (x<max_x) 
	  && (y>min_y) && (y<max_y)) {
        resulting_features.push_back(VW::PlesseyCorner(locations[i]));
	VW::PlesseyCorner& feature = resulting_features[resulting_features.size()-1];
	feature.SetPatchSize(patch_size);
	GetInterpolatedImagePatch(image,x,y,*(feature.GetImagePatch()));
      }
    }
  } else {
    // Create non-interpolated patches
    for (unsigned int i=0; i<locations.size(); i++) {
      const float x = locations[i].GetX();
      const float y = locations[i].GetY();
      if ((x>min_x) && (x<max_x) 
	  && (y>min_y) && (y<max_y)) {
        resulting_features.push_back(VW::PlesseyCorner(locations[i]));
	VW::PlesseyCorner& feature = resulting_features[resulting_features.size()-1];
	feature.SetPatchSize(patch_size);
	GetImagePatch(image,x,y,*(feature.GetImagePatch()));
      }
    }
  }
  if (VERBOSE) cerr << "Done" << endl;
}
