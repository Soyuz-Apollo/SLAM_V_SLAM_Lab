/** 
 * @file   drawgtkgl.cc
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
#include <VW/image.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/Improc/plesseycorner.h>
#include "drawgl.h"
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef VERBOSE
#  undef VEERBOSE
#endif
#define VERBOSE false

// GL_NEAREST for fast, GL_LINEAR for linear interpolation
#define TEXTURE_INTERPOLATION GL_NEAREST

using namespace std;
using namespace VW;




void DrawFunctions::Draw(DisplayGLCommon& display, 
			 const ImageMono<unsigned char>& image) 
{
  if (VERBOSE) cerr << "DrawFunctions::DrawGL(ImageMono)"<< endl;
  if (image.IsAlloced() == false) {
    cerr << "DrawFunctions::Draw(image): Image is not allocated!" << endl;
    return;
  }

  // Make the GL widget 'current'
  display.MakeCurrent();
  display.Reset(0,0,0);

    // Texture method
   const float depth_3d = display.GetImageDrawingDepth();
   
  glDisable(GL_COLOR_MATERIAL);
  glShadeModel(GL_FLAT);

  if ((display.GetTextureWidth() < image.GetWidth())
      || (display.GetTextureHeight() < image.GetHeight())) {

    
    if (VERBOSE) cerr << "   Current texture too small ("
		      <<display.GetTextureWidth()<<"x"
		      <<display.GetTextureHeight()<<")"<< endl;

      // Current texture is too small
      display.GenerateTexture(image.GetWidth(),
			      image.GetHeight());
  }

  // Bind the texture
  glBindTexture(GL_TEXTURE_2D, display.GetTextureName());
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  if (VERBOSE) cerr << "Texture "<<display.GetTextureName()<<" ("<< display.GetTextureName() 
		    << ") width " << display.GetTextureWidth()
		    << " height " << display.GetTextureHeight() << endl;


  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		  image.GetWidth(), 
		  image.GetHeight(), 
		  GL_LUMINANCE,
		  GL_UNSIGNED_BYTE, 
		  image.GetRawBuffer());

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // Save the current drawing matrix 
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity(); 

  // Transform into Object frame
  float useful_width = float(image.GetWidth()) /
                       float(display.GetTextureWidth());
  float useful_height = float(image.GetHeight()) /
                        float(display.GetTextureHeight());
  if (VERBOSE) cerr << "useful_width " << useful_width << " useful_height "
		    << useful_height << endl;

  glRotatef(180.0, 1.0, 0.0, 0.0);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);   
  glVertex3f(-1,  -1, depth_3d);
  glTexCoord2f(0.0,          useful_height); 
  glVertex3f(-1, 1, depth_3d);
  glTexCoord2f(useful_width, useful_height); 
  glVertex3f( 1,1, depth_3d);
  glTexCoord2f(useful_width, 0.0);           
  glVertex3f( 1,  -1, depth_3d);
  glEnd();

  glPopMatrix();

  glDisable(GL_TEXTURE_2D);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);


}


void DrawFunctions::Draw(DisplayGLCommon& display, 
			 const ImageRGB<unsigned char>& image) 
{
  if (image.IsAlloced() == false) {
    cerr << "DrawFunctions::Draw(image): Image is not allocated!" << endl;
    return;
  }

  // Make the GL widget 'current'
  display.MakeCurrent();
  display.Reset(0,0,0);

    // Texture method
   const float depth_3d = display.GetImageDrawingDepth();
   
  glDisable(GL_COLOR_MATERIAL);
  glShadeModel(GL_FLAT);

  if ((display.GetTextureWidth() < image.GetWidth())
      || (display.GetTextureHeight() < image.GetHeight())) {

    
    if (VERBOSE) cerr << "   Current texture too small ("
		      <<display.GetTextureWidth()<<"x"
		      <<display.GetTextureHeight()<<")"<< endl;

      // Current texture is too small
      display.GenerateTexture(image.GetWidth(),
				image.GetHeight());
  }

  // Bind the texture
  glBindTexture(GL_TEXTURE_2D, display.GetTextureName());
  
  if (VERBOSE) cerr << "Texture ("<< display.GetTextureName() 
		    << ") width " << display.GetTextureWidth()
		    << " height " << display.GetTextureHeight() << endl;


#ifdef _WIN32
	// Windows does not have the BGRA flag! Aaargh!
	// Convert to a RGBA buffer
	unsigned char* buff = new unsigned char[image.GetWidth()*image.GetHeight()*4];
	VW::ImageConversions::VWImage_to_RGBABuffer(&image,buff);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		  image.GetWidth(), 
		  image.GetHeight(), 
		  GL_RGBA,
		  GL_UNSIGNED_BYTE, 
		  buff);
#else
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		  image.GetWidth(), 
		  image.GetHeight(), 
		  GL_BGRA,
		  GL_UNSIGNED_BYTE, 
		  image.GetRawBuffer());
#endif

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // Save the current drawing matrix 
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity(); 

  // Transform into Object frame
  float useful_width = float(image.GetWidth()) /
                       float(display.GetTextureWidth());
  float useful_height = float(image.GetHeight()) /
                        float(display.GetTextureHeight());
  if (VERBOSE) cerr << "useful_width " << useful_width << " useful_height "
		    << useful_height << endl;

  glRotatef(180.0, 1.0, 0.0, 0.0);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);   
  glVertex3f(-1,  -1, depth_3d);
  glTexCoord2f(0.0,          useful_height); 
  glVertex3f(-1, 1, depth_3d);
  glTexCoord2f(useful_width, useful_height); 
  glVertex3f( 1,1, depth_3d);
  glTexCoord2f(useful_width, 0.0);           
  glVertex3f( 1,  -1, depth_3d);
  glEnd();

  glPopMatrix();

  glDisable(GL_TEXTURE_2D);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);

}

// {{{ Draw(DisplayGTKGLCommon& display, const ImageYUV<unsigned char>& image)

void DrawFunctions::Draw(DisplayGLCommon& display, const ImageYUV<unsigned char>& image) 
{
  // Unfortunately, GL *requires* data to be RGB or BGR, so must copy first
  ImageRGB<unsigned char> rgb_image(image.GetWidth(),image.GetHeight());


  ImageCopy(image,rgb_image);
  //  ImageConversions::VWImage_to_VWImage(&image,&rgb_image);

  Draw(display,rgb_image);
}

// }}}
// {{{ Draw(DisplayGTKGLCommon& display, const ImageHSV<unsigned char>& image)

void DrawFunctions::Draw(DisplayGLCommon& display, const ImageHSV<unsigned char>& image) {
   // Unfortunately, GL *requires* data to be RGB or BGR, so must copy first
  ImageRGB<unsigned char> rgb_image(image.GetWidth(),image.GetHeight());


  ImageCopy(image,rgb_image);
  //  ImageConversions::VWImage_to_VWImage(&image,&rgb_image);

  Draw(display,rgb_image);
}

// }}}

// {{{ Draw(DisplayGTKGLCommon& display, const vector<PlesseyCorner>& corners)

void DrawFunctions::Draw(DisplayGLCommon& display, const vector<PlesseyCorner>& corners,
			 VW::Normalisation C,
			 float size, CornerShape shape,
			 const vector<Colour> *colours) 
{
if (VERBOSE)  cout << "DrawFunctions::Draw (vector<PlesseyCorner>): going to draw "<<corners.size()<<" corners." << endl;

  display.MakeCurrent();
    glDisable(GL_LIGHTING);  // So that colours work OK
    glEnable(GL_COLOR_MATERIAL);

  unsigned char orig_red, orig_green, orig_blue, orig_alpha;
  display.GetPenColour (orig_red, orig_green, orig_blue, orig_alpha);

  vector<Colour>::const_iterator col_it;
  if (colours != 0) col_it = colours->begin ();
  bool done = false;

  const float depth_3d = display.GetFeatureDrawingDepth();

  for (vector<PlesseyCorner>::const_iterator it = corners.begin ();
       it != corners.end (); it++) {
    //    cout << "Source point ("<<it->GetX()<<","<<it->GetY();
    const PlesseyCorner& p = C.UnNormalise (*it, VW::Point2D());

    float x  = float(p.GetX());
    float y  = float(p.GetY());
    float xm = x - (size * p.GetFeatureScale());
    float xp = x + (size * p.GetFeatureScale());
    float ym = y - (size * p.GetFeatureScale());
    float yp = y + (size * p.GetFeatureScale());
 
//    cout << "DrawFunctions::Draw (vector<PlesseyCorner>): coords = (" 
// 	<< x << " "
// 	<< y << ") ("
// 	<< xm << " "
// 	<< ym << ") ("
// 	<< xp << " "
// 	<< yp << ") "
// 	<< endl;   // Get GL Coords
//    cout << ") unnorm point ("<<x<<","<<y;
    display.ConvertToGLCoords(x,y);
    display.ConvertToGLCoords(xm,ym);
    display.ConvertToGLCoords(xp,yp);
    //    cout <<") GL point = ("<<x<<","<<y<<")"<<endl;

//     cout << "DrawFunctions::Draw (vector<PlesseyCorner>): coords = (" 
// 	<< x << " "
// 	<< y << ") ("
// 	<< xm << " "
// 	<< ym << ") ("
// 	<< xp << " "
// 	<< yp << ") "
// 	<< endl;

    // Set the colour (if any)
    if (colours != 0 && !done) {
      // If colours aren't provided, pen colour will just be unchanged
      // If provided array isn't long enough, remaining corners will be
      // original pen colour
      if (col_it == colours->end ()) {
	done = true;
	display.SetPenColour (orig_red, orig_green, orig_blue, orig_alpha);
      }
      else {
	display.SetPenColour (col_it->_r, col_it->_g, col_it->_b, col_it->_a);
	col_it++;
      }
    }
      
      // Also note that the texture is not square
      switch(shape) {
      default:
      case CROSS:
	// Draw a point as a cross symbol
	glBegin(GL_LINE_STRIP);
	glVertex3d(xm, ym, depth_3d);
	glVertex3d(xp, yp, depth_3d);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3d(xp, ym, depth_3d);
	glVertex3d(xm, yp, depth_3d);
	glEnd();
	break;
      case PLUS:
	// Draw a point as a plus symbol
	glBegin(GL_LINE_STRIP);
	glVertex3d(xm,y, depth_3d);
	glVertex3d(xp,y, depth_3d);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3d(x,ym, depth_3d);
	glVertex3d(x,yp, depth_3d);
	glEnd();
 	break;
      case SQUARE:
	// Draw a point as a square
	glBegin(GL_LINE_STRIP); {
	  glVertex3d(xm, ym, depth_3d);
	  glVertex3d(xp, ym, depth_3d);
	  glVertex3d(xp, yp, depth_3d);
	  glVertex3d(xm, yp, depth_3d);
	  glVertex3d(xm, ym, depth_3d);
	}; glEnd();
	break;
      }	
  }    
  if (VERBOSE) cout << "DrawFunctions::Draw (vector<PlesseyCorner>): Done" << endl;
}

// }}}
