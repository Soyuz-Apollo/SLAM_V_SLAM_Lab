/** 
 * @file   displayglcommon.h
 * @author BJ Tordoff
 * @date   29/01/2002
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
 * @class VW::DisplayGLCommon
 * @ingroup Displays
 * @author BJ Tordoff
 * @date 01/01/2001
 * @brief Base class for OpenGL displays.
 *
 * This class defines the common GL bits that all
 * GL based displays need.
 */

#ifndef _DISPLAY_GL_COMMON_H
#define _DISPLAY_GL_COMMON_H


// This abstract class provides the functions that the DrawGL needs to see
#include <iostream>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <VW/Graphics/displaybase.h>


namespace VW {

class DisplayGLCommon : public DisplayBase {
 public:
  DisplayGLCommon (const std::string title = "") : 
    DisplayBase (title),
    texName(0)
  {}
  DisplayGLCommon (const unsigned int width, 
		   const unsigned int height,
		   const std::string title = "") :
    DisplayBase (width, height, title),
    texName(0)
  {
    assert(width>0);
    assert(height>0);
    ResetCoords(width,height);
  }
  
  virtual ~DisplayGLCommon() {};

  virtual void MakeCurrent() = 0;


  // Primitive drawing operations
  void Reset(unsigned char red=0,
	     unsigned char green=0,
	     unsigned char blue=127);

  void SetPixel(const int x, const int y, 
		const unsigned char red, 
		const unsigned char green, 
		const unsigned char blue, 
		const unsigned char alpha=0);

  void SetPixel(const int x, const int y);

  void SetPenColour(const unsigned char red, 
		    const unsigned char green, 
		    const unsigned char blue, 
		    const unsigned char alpha=0);

   void GetPenColour(unsigned char& red, unsigned char& green, 
		     unsigned char& blue, unsigned char& alpha) const;
  void DrawPoint(const float x, const float y);
  void DrawLine(const float x0, const float y0,
		const float x1, const float y1);

  /** Convert pixel coords into GL coords for drawing.
   */
  void GetGLCoords(float pixelx, float pixely,
		   float& glx, float& gly) const;
  /** Convert pixel coords into GL coords for drawing.
   */
  void ConvertToGLCoords(float& x, float& y) const;  
  /** Convert GL coords into pixel coords.
   */
  void GetPixelCoords(float pixelx, float pixely,
		   float& glx, float& gly) const;
  /** Convert GL coords into pixel coords.
   */
  void ConvertToPixelCoords(float& x, float& y) const;
  
  virtual unsigned int GetTextureName();
  virtual unsigned int GetTextureWidth() const;
  virtual unsigned int GetTextureHeight() const;
  virtual void GenerateTexture(unsigned int desired_width,
			       unsigned int desired_height);

  float GetImageDrawingDepth() const;
  float GetFeatureDrawingDepth() const;

  /** Force display to the screen. */
  void Flush();

 protected:
  virtual void SwapBuffers() = 0;
  void ResetCoords(double width, double height);

  // Initialise the GL settings
  void InitGL();
  void SetGLView();


  // GL texture stuff
  unsigned int texName;
  unsigned int texture_width;
  unsigned int texture_height;
  float _xscale,_xoffset;
  float _yscale,_yoffset;

  // Pen colour (to allow GetColour to be const)
  unsigned char _red,_green,_blue,_alpha;

  // Last click pos
  int last_x;
  int last_y;
  // Whether we're mid-drag
  bool dragging;


};

} // end namespace VW



inline void 
VW::DisplayGLCommon::Reset(unsigned char red,
			   unsigned char green,
			   unsigned char blue) 
{
  MakeCurrent();
  glClearColor(float(red)/255,float(green)/255,float(blue)/255,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
};

inline unsigned int
VW::DisplayGLCommon::GetTextureName()
{
  return texName;
}

inline unsigned int 
VW::DisplayGLCommon::GetTextureWidth() const 
{
  return(texture_width);
}

inline unsigned int 
VW::DisplayGLCommon::GetTextureHeight() const 
{
  return(texture_height);
}

inline void
VW::DisplayGLCommon::GetGLCoords(float pixelx, float pixely,
				 float& glx, float& gly) const 
{
  glx = _xscale * (pixelx - _xoffset);
  gly = _yscale * (pixely - _yoffset);
}

inline void
VW::DisplayGLCommon::ConvertToGLCoords(float& x, float& y) const 
{
  x = _xscale * (x - _xoffset);
  y = _yscale * (y - _yoffset);
}

inline void
VW::DisplayGLCommon::GetPixelCoords(float glx, float gly,
				    float& pixelx, float& pixely) const 
{
  pixelx = (glx / _xscale) + _xoffset;
  pixely = (gly / _yscale) + _yoffset;
}

inline void
VW::DisplayGLCommon::ConvertToPixelCoords(float& x, float& y) const 
{
  x = (x / _xscale) + _xoffset;
  y = (y / _yscale) + _yoffset;
}

inline float 
VW::DisplayGLCommon::GetImageDrawingDepth() const
{
  return 0.5;
}

inline float 
VW::DisplayGLCommon::GetFeatureDrawingDepth() const
{
  return 0;
}

inline void 
VW::DisplayGLCommon::Flush () 
{
  MakeCurrent(); 
  SwapBuffers();
}

// Drawing primitves
inline void 
VW::DisplayGLCommon::SetPixel(const int x, const int y,
			      const unsigned char red, 
			      const unsigned char green,
			      const unsigned char blue, 
			      const unsigned char alpha) 
{
  // normalise coords to -w/2<->w/2
  float xn=0,yn=0;
  GetGLCoords(float(x),float(y),xn,yn);

  //  std::cout << "**Error: Setting pixels directly in GL displays is not supported yet" << std::endl;
  MakeCurrent(); 
  glColor4f(GLfloat(red/255.0),
	    GLfloat(green/255.0),
	    GLfloat(blue/255.0),
	    GLfloat(alpha/255.0));
  
  glBegin(GL_POINTS); {
    glVertex3f(xn,yn,GetFeatureDrawingDepth());
  }; glEnd();

  // Reset to previous colour
  glColor4f(GLfloat(_red/255.0),
	    GLfloat(_green/255.0),
	    GLfloat(_blue/255.0),
	    GLfloat(_alpha/255.0));
}

inline void 
VW::DisplayGLCommon::SetPixel(const int x, const int y) 
{
  // normalise coords to -w/2<->w/2
  float xn=0,yn=0;
  GetGLCoords(float(x),float(y),xn,yn);
  MakeCurrent(); 
  glBegin(GL_POINTS); {
    glVertex3f(xn,yn,GetFeatureDrawingDepth());
  }; glEnd();
}

inline void 
VW::DisplayGLCommon::DrawPoint(const float x, const float y) 
{
  // normalise coords to -w/2<->w/2
  float x0n=0,y0n=0;
  GetGLCoords(float(x),float(y),x0n,y0n);

  MakeCurrent();
  {
    glDisable(GL_LIGHTING);  // So that colours work OK
    glEnable(GL_COLOR_MATERIAL);
      glBegin(GL_POINTS);
      // Z part just makes sure it is infront of the image
      glVertex3d(x0n,y0n,GetFeatureDrawingDepth());
      glEnd();

   glDisable(GL_COLOR_MATERIAL);
   glEnable(GL_LIGHTING);
  }
}


inline void 
VW::DisplayGLCommon::DrawLine(const float x0, const float y0,
			      const float x1, const float y1) 
{
  MakeCurrent();
  // normalise coords to -w/2<->w/2

  // NB: inverted 'Y'
  float x0n=0.0f,y0n=0.0f,x1n=0.0f,y1n=0.0f;
  GetGLCoords(x0,y0,x0n,y0n);
  GetGLCoords(x1,y1,x1n,y1n);

  // std::cerr << "Asked for line from ("<<x0<<","<<y0<<") to (" <<x1<<","<<y1<<")"<<std::endl;
  // std::cerr << "Drawing line from ("<<x0n<<","<<y0n<<","<<GetFeatureDrawingDepth()<<") to (" <<x1n<<","<<y1n<<","<<GetFeatureDrawingDepth()<<")"<<std::endl;
  {
    glDisable(GL_LIGHTING);  // So that colours work OK
    glEnable(GL_COLOR_MATERIAL);
    glBegin(GL_LINE_STRIP); {
      //    glEnable(GL_LINE_SMOOTH);
      //      glLineWidth(3.0);
      // Z part just makes sure it is infront of the image
      glVertex3d(x0n,y0n,GetFeatureDrawingDepth());
      glVertex3d(x1n,y1n,GetFeatureDrawingDepth());
    }; glEnd();
  }
}
inline void  
VW::DisplayGLCommon::SetPenColour(const unsigned char red, 
				  const unsigned char green, 
				  const unsigned char blue, 
				  const unsigned char alpha) 
{
  MakeCurrent();
  glColor4f(GLfloat(red/255.0),
	  GLfloat(green/255.0),
	  GLfloat(blue/255.0),
		GLfloat(alpha/255.0));
  _red=red;
  _green=green;
  _blue=blue;
  _alpha=alpha;
}

inline void  
VW::DisplayGLCommon::GetPenColour(unsigned char& red, 
				  unsigned char& green, 
				  unsigned char& blue, 
				  unsigned char& alpha) const 
{
  red=_red;
  green=_green;
  blue=_blue;
  alpha=_alpha;
}
 
inline void VW::DisplayGLCommon::ResetCoords(double width,double height)
{
  _xoffset = float(0.5*width);
  _xscale = float(1.0 / _xoffset);
  _yoffset = float(0.5*height);
  _yscale = -float(1.0 / _yoffset);
  //std::cerr << "*** GL stuff "<<_xoffset<<","<<_yoffset<<","<<_xscale<<","<<_yscale<<std::endl; 
 }


#endif
