/** 
 * @file   edgel.h
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


#ifndef _edgel_h_
#define _edgel_h_

#include <vector>
#include <deque>
#include <VW/GeomObjects/point2d.h>

// This file defines
// (1) the edgel class,
// (2) the edgestring class
// (3) the edgedata class

//////////////////////////////////////////

namespace VW {

class Edgel : public Point2D {             

 protected:

  float s; // To avoid sqrting, this is grey-level strength SQUARED
  float a;
  int   ia,ix,iy;
  bool  linked;

 public:

  /** Construct with default values. */
  Edgel();
  /** Construct explicitly. 
   * @param fs edge-strength squared
   * @param fa edge orientation (angle from vertical)
   * @param fx precise x coordinate
   * @param fy precise y coordinate
   * @param iia integer orientation
   * @param iix pixel x-coord
   * @param iiy pixel y-coord
   */
  Edgel(float fs,float fa,float fx,float fy,int iia,int iix,int iiy); 
  ~Edgel();               // destructor

  /** The edge strength (gradient squared) */
  inline float GetS(void)  {return(s);};

  /** Edge orientation (angle clockwise from vertical) */
  inline float GetA(void)  {return(a);};

  /** Integer angle (-8<=ia<8, clockwise from vertical) */
  inline int   GetIA(void) {return(ia);};

  /** Pixel x position (use GetX() for precise value). */
  inline int   GetIX(void) {return(ix);};

  /** Pixel y position (use GetY() for precise value). */
  inline int   GetIY(void) {return(iy);};

  inline void SetLinked(void) {linked=true;};
  inline void UnSetLinked(void) {linked=false;};
  inline bool GetLinked(void) {return(linked);};
};

//////////////////////////////////////////

typedef std::deque<Edgel *> EdgeString;

class EdgeData {             

 protected:

  int    c0, r0;    // referred to original image.
  int    cl, rl;

 public:

  EdgeData();    // constructor
  EdgeData(EdgeData&) 
	{ 
		using namespace std;
		cerr << "EdgeData copy constructor called in error\n"; 
			exit(1); 
  }
  ~EdgeData();   // destructor
  void Clear(); // clears an edgemap.

  inline int GetNEdgels(void) const { return int(edgels.size()); };
  inline int GetNStrings(void) const { return int(strings.size()); };
  inline int GetWidth(void) const { return(cl-c0); };
  inline int GetHeight(void) const { return(rl-r0); };


public: // data
  std::vector<Edgel *>      edgels;
  std::vector<EdgeString *> strings;

private:

};


//////////////////////////////////////////

}; // end namespace VW

#endif /* _edgel_h_ */
