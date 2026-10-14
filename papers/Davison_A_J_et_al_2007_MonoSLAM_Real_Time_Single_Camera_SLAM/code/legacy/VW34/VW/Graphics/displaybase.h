/** @file displaybase.h 
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
 * @class VW::DisplayBase
 * @ingroup Displays
 * @ingroup CoreGraphics
 * @author BJ Tordoff
 * @date 01/01/2001
 * @brief Base class for displays.
 *
 * This class defines the minimum display functionality.
 * It is an abstract class, and everything must be 
 * overloaded. The aim is that enough functionality is
 * provided here for generic non-speed critical drawing
 * but more intensive stuff (image drawing) is handled
 * by specifcally overloaded draw functions.
 */


#ifndef _DISPLAYBASE_H
#define _DISPLAYBASE_H

#include <string>
#include <VW/events.h>

namespace VW {

class DisplayBase {
public:

  /** Bog standard constructor.
   * Using this constructor the display is not put on screen.
   * To get it on-screen, call the SetSize function below.
   */
  DisplayBase(const std::string title = "") 
    : _initialised(false),
      _visible(false),
      _my_title(title)
  {}; 

  /** Detailed constructor.
   * Using this constructor the display is initialised
   * and immediately stuck onto the screen.
   */
  DisplayBase(const unsigned int width,
				  const unsigned int height, 
				  const std::string title = "") 
    : _initialised (false),
      _visible(false),
      _my_title(title)
  {};

  /** Destructor. */
  virtual ~DisplayBase() {};


  inline bool IsInitialised() {return(_initialised);};

  /** Return the name (displayed in title bar if visible) of the display. */
  std::string GetTitle() const;

 /** Set (or reset) the size.
   * If the display is not currently onscreen, it is shown. Otherwise
   * it is resized (which often involves first hiding it from view, so
   * don't expect it to be fast!).
   */
  virtual void SetSize(const unsigned int new_width,
		       const unsigned int new_height) = 0;

  virtual unsigned int GetWidth() = 0;
  virtual unsigned int GetHeight() = 0;


  /** Set the colour used for drawing lines and points. */
  virtual void SetPenColour(const unsigned char red, 
									 const unsigned char green, 
									 const unsigned char blue, 
									 const unsigned char alpha=0) = 0;

  /** Recover the colour used for drawing lines and points. */
  virtual void GetPenColour(unsigned char& red, 
									 unsigned char& green, 
									 unsigned char& blue, 
									 unsigned char& alpha) const = 0;

  /** Set a single pixel to the current pen colour. */
  virtual void SetPixel(const int x, const int y) = 0;

  /** Set the colour of a single pixel. */
  virtual void SetPixel(const int x, const int y,
								const unsigned char red, 
								const unsigned char green, 
								const unsigned char blue, 
								const unsigned char alpha) = 0;

  /** Draw a point at \f$(x,y)\f$. */
  virtual void DrawPoint(const float x, const float y) = 0;
  /** Draw a line from \f$(x_0,y_0)\f$ to \f$(x_1,y_1)\f$. */
  virtual void DrawLine(const float x0, const float y0, 
			const float x1, const float y1) = 0;
  /** Convenience function to draw a rectangle (just calls four lots
   * of DrawLine)
   */
  void DrawRectangle(const float x0, const float y0, 
		     const float x1, const float y1);

  /** Convenience function to draw a circle (made of line segments)
   */
  void DrawCircle(const float x0, const float y0, 
		  const float radius);

  /** Convenience function to draw a right ellipse (ie major and minor axes
   * aligned with x, y axes).
   * @param x0 x coord of centre
   * @param y0 y coord of centre
   * @param rx max radius in x-direction
   * @param ry max radius in y direction 
   */
  void DrawRightEllipse(const float x0, const float y0, 
			const float rx, const float ry);

  /** Convenience function to draw a general ellipse.
   *
   * The parameters are such that they are easily recovered from the matrix
   * equation for an ellipse:
   *
   * ( x-x0  y-y0 ) C^-1 ( x-x0 ) = sigma^2
   *                     ( y-y0 )
   * 
   * where sigma gives the "radius" (number of std devs) and C is
   *
   *   C = [ rx    0  ] [ rx  skew ]
   *       [ skew  ry ] [ 0   ry   ]
   *
   * @param x0   x coord of centre
   * @param y0   y coord of centre
   * @param rx   radius in x-direction at y=0
   * @param ry   radius in y-direction at x=0
   * @param skew effect of y on x, where skew=0 gives a right ellipse
   */
  void DrawEllipse(const float x0, const float y0, 
		   const float rx, const float ry, const float skew);


  /** Force drawing to screen (flush buffers).
   * This should be called once all drawing has
   * been done, and forces X, GL or whatever to
   * put the stuff onscreen (which they don't always
   * do otherwise).
   */
  virtual void Flush(void) = 0;

  // Events
  VWEvents::Event3<void,int,int,int> ClickEvent;
  VWEvents::Event5<void,int,int,int,int,int> DragEvent;
  VWEvents::Event5<void,int,int,int,int,int> DragFinishedEvent;

 protected:
  bool _initialised;
  bool _visible;
  std::string _my_title;
};

}  // end namespace VW



// Inlines

inline void 
VW::DisplayBase::DrawRectangle(const float x0, const float y0, 
			       const float x1, const float y1)
{
  this->DrawLine(x0,y0,x1,y0);
  this->DrawLine(x1,y0,x1,y1);
  this->DrawLine(x1,y1,x0,y1);
  this->DrawLine(x0,y1,x0,y0);
}
inline std::string 
VW::DisplayBase::GetTitle() const
{
  return _my_title;
}

inline void 
VW::DisplayBase::DrawCircle(const float x0, const float y0, 
			    const float radius)
{
  DrawRightEllipse(x0,y0,radius,radius);
}

#endif
