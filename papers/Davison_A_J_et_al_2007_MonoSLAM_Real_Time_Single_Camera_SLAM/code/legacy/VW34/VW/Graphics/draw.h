/** @file draw.h
 *  @author BJ Tordoff
 *  @date   02/08/02
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


#ifndef _DRAW_H_
#define _DRAW_H_

#include <iostream>

#include "displaybase.h"
#include <VW/GeomObjects/normalisation.h>
#include <VW/GeomObjects/trajectorymap.h>
#include <VW/Improc/edgel.h>

#include "drawtypes.h" // for colour, shape etc

namespace VW {

  // Forward declare stuff
  // Stuff from Image
  template <class T> class ImageMono;
  template <class T> class ImageRGB;
  template <class T> class ImageYUV;
  template <class T> class ImageHSV;
  template <class T, int i> class ImageInterleave;

  // Stuff from Geom
  class Point2D;
  class LineSeg2D;

  // Stuff from Improc
  class LineSegFitted;
  class PlesseyCorner;
  class CornerFeature;
  class SIFTFeature;
  class PatchFeature;
  class Covariance2D;

  /**
   * @author BJ Tordoff, JGH Knight
   * @date   02/08/2002
   * @brief Draw things on displays.
   *
   * This file can draw things on general displays using primitives only. The 
   * functions are therefore slow and an optimised version should be used 
   * where available (these are normally provided with the implementations
   * of the display).
   */
namespace DrawFunctions {


  // Images

  void Draw(DisplayBase& display, const ImageMono<unsigned char>& image);
//  void Draw(DisplayBase& display, const ImageInterleave<unsigned char,4>& image);
//  void Draw(DisplayBase& display, const ImageInterleave<unsigned char,2>& image);
  void Draw(DisplayBase& display, const ImageRGB<unsigned char>& image);
  void Draw(DisplayBase& display, const ImageHSV<unsigned char>& image);

  // Geometric objects
  void Draw(DisplayBase& display, const Point2D& p,
	    Normalisation C = Normalisation(),
	    float size = 2, CornerShape shape = PLUS);
  void Draw(DisplayBase& display, const std::vector<Point2D>& pl,
	    Normalisation C = Normalisation(),
	    float size = 2, CornerShape shape = PLUS);
  void Draw(DisplayBase& display, const Point2D& p, 
	    float size = 2, CornerShape shape = PLUS);
  void Draw(DisplayBase& display, const VW::CornerFeature& p,
	    Normalisation C = Normalisation(),
	    float size = 2, CornerShape shape = PLUS);
  void Draw(DisplayBase& display, const VW::CornerFeature& p, 
	    float size = 2, CornerShape shape = PLUS); 
  void Draw(DisplayBase& display, const VW::SIFTFeature& p,
	    Normalisation C = Normalisation());
  void Draw(DisplayBase& display, const std::vector<VW::SIFTFeature>& p,
	    Normalisation C = Normalisation());
  void Draw(DisplayBase& display, const VW::PatchFeature& p,
	    Normalisation C = Normalisation());
  void Draw(DisplayBase& display, const std::vector<VW::PatchFeature>& p,
	    Normalisation C = Normalisation());



  void Draw(DisplayBase& display, const LineSeg2D& ls, Normalisation C = Normalisation());
  void Draw(DisplayBase& display, const std::list<LineSeg2D>& ls);
  void Draw(DisplayBase& display, const std::list<LineSegFitted>& ls);

  void Draw(DisplayBase& display, const Line2D& l, Normalisation C = Normalisation());

  // Trajectories
  void Draw(DisplayBase& display, const Trajectory<Point2D>& t,
	    Normalisation C = Normalisation(), bool = true);
  void Draw(DisplayBase& display, const Trajectory<Point2D>& t, bool b); // ditto
  void Draw(DisplayBase& display, const Trajectory<VW::PlesseyCorner>& t,
	    Normalisation C = Normalisation(), bool = true);
  void Draw(DisplayBase& display, const Trajectory<VW::PlesseyCorner>& t, bool b); // ditto

  // Trajectory maps
  template<class Type> 
    void Draw(DisplayBase& display, const TrajectoryMap<Type>& tm,
	      Normalisation C = Normalisation());

  // Processed objects
  void Draw(DisplayBase& display, 
	    const EdgeData& edat);
  void Draw(DisplayBase& display,
	    const EdgeString& estring);
  void Draw(DisplayBase& display, 
	    const std::vector<VW::CornerFeature>& corners,
	    Normalisation C = Normalisation(),
	    float size = 2, CornerShape shape = PLUS,
	    const std::vector<Colour> *colours = 0);

  void Draw(DisplayBase& display, 
	    const Point2D& location,
	    const VW::Covariance2D& covar);
  void DrawCovariance(DisplayBase& display, 
		      const Point2D& location,
		      const VNL::MatrixFixed<2,2,double>& covar);


    void DrawRectangleOutline(DisplayBase& display,
			      unsigned int x0, unsigned int y0,
			      unsigned int x1, unsigned int y1,
			      unsigned int linewidth=1);

















template<class Type> 
inline void 
Draw(DisplayBase& display, 
			const TrajectoryMap<Type>& tm,
//IDR for g++2.95	     Normalisation C = Normalisation())
			     Normalisation C)
{
  for (typename TrajectoryMap<Type>::const_iterator i=tm.begin();
       i!=tm.end();i++) 
    Draw(display,*i, C);
};


}; // end namespace DrawFunctions

} // end namespace VW


// Make this universally available without specification
using namespace VW::DrawFunctions;

#endif
