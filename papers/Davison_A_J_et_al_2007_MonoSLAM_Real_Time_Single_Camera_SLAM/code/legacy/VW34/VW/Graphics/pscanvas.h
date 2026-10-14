/** 
 * @file   pscanvas.h
 * @author ID Reid
 * @date   01/09/2004
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
/**
 * @class   VW::PSCanvas
 * @ingroup Displays
 * @author  ID Reid
 * @date    01/09/2004
 * @brief   A canvas for drawing raw postscript.
 *
 */


#ifndef _PSCANVAS_H
#define _PSCANVAS_H

#include <fstream>
#include <string>
#include <vector>

// Include base class
#include <VW/Graphics/displaybase.h>

namespace VW {

class PSCanvas : public DisplayBase {
public:
  // Genesis and revelations...
//   PSCanvas(const std::string title = ""); 
  PSCanvas(const unsigned int width,
	   const unsigned int height, 
	   const std::string filename);
  ~PSCanvas(); 


  // Useful functions
  /** Open a new output file. */
  void Open(const unsigned int width,
	    const unsigned int height, 
	    const std::string filename);
  /** Set the size of the output (only if not set at construction). */
  void SetSize(const unsigned int new_width,
	       const unsigned int new_height);
  unsigned int GetWidth();
  unsigned int GetHeight();

  // Primitive drawing operations
  /** Measured in points. */
  void SetLineThickness(double thickness);
  void SetPenColour(const unsigned char red, const unsigned char green, 
		    const unsigned char blue, const unsigned char alpha=0);
  void GetPenColour(unsigned char& red, unsigned char& green, 
		    unsigned char& blue, unsigned char& alpha) const;
  void SetPixel(const int x, const int y);
  void SetPixel(const int x, const int y, const unsigned char red, 
		const unsigned char green, const unsigned char blue, const unsigned char alpha=0);
  void DrawPoint(const float x, const float y);
  void DrawLine(const float x0, const float y0, 
		const float x1, const float y1);

  // Force drawing to screen (flush buffers)
  void Flush(void);
  void Close(void);

  std::ofstream& Stream() { return _fp; }

protected:

  unsigned int _width, _height;
  std::ofstream _fp;
  std::string _filename;

  // vw kind of stuff
  int _pencolour;
  double _red,_green,_blue,_alpha;

};

} // end namespace VW

// Inliners
inline VW::PSCanvas::PSCanvas(const unsigned int w,
			      const unsigned int h, 
			      const std::string filename) 
{
  Open(w,h,filename);
}

inline unsigned int VW::PSCanvas::GetWidth(void) 
{
  return(_width);
}

inline unsigned int VW::PSCanvas::GetHeight(void) 
{
  return(_height);
}

inline void VW::PSCanvas::GetPenColour(unsigned char& red, 
				       unsigned char& green, 
				       unsigned char& blue, 
				       unsigned char& alpha) const 
{
  red = static_cast<unsigned char>(255*_red);
  green = static_cast<unsigned char>(255*_green);
  blue = static_cast<unsigned char>(255*_blue);
  alpha = static_cast<unsigned char>(255*_alpha);
}


#endif
