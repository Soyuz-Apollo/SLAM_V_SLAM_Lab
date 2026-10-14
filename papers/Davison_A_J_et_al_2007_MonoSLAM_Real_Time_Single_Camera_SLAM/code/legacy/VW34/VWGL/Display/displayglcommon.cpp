/** 
 * @file   displayglcommon.cpp
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

#ifdef _WIN32
#include <windows.h>
#endif

#include "displayglcommon.h"

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

using namespace std;

// Get the first power of 2 greater than or equal to number
static inline unsigned int GetNextPowerOf2(const unsigned int number)
{
  // Note that the iterative version (below) is *MUCH* faster than
  // taking logs for small powers of 2 (ie. sizes and the like). For
  // a typical number like 4000 -> 4096, it's about 10x faster.


  // Log version
  //const double lognum = log(double(w+i*2)) / log(2.0);
  //result = int (pow(2.0, ceil(lognum)));


  // Iterative version
  unsigned int result=1;
  unsigned int testnum=2;
  while (testnum<number) {
    testnum *= 2;
    result++;
  }
  return testnum;
}



void VW::DisplayGLCommon::SetGLView() {
  if (VERBOSE) cerr << "DisplayGLCommon::SetGLView" << endl;
  // Set the camera position
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();  // Reset The Projection Matrix
  // Orthographic projection
  glOrtho(-1,1, -1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
}

void VW::DisplayGLCommon::InitGL() {

    // To use fast 3D rendering, images are rendered as textures
    if (VERBOSE) cerr << "DisplayGLCommon::InitGL()" << endl;
    if ((VERBOSE) && (!_visible)) 
      cerr << "InitGL: Warning - attempting to initialise before the display is visible." << endl;

    
    // *** Start GL
    GLfloat mat_ambient[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {50.0};
    GLfloat light_position[] = {1.0, 1.0, -1.0, 0.0};
    GLfloat model_ambient[] = {0.5, 0.5, 0.5, 1.0};
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
    
    // Set up lighting and stuff
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    
    // Indicate no texture
    texName=0;
    texture_width=0;
    texture_height=0;

  // Set camera position
  SetGLView();
  
  // *** End GL
  
  _initialised=true;
  if (VERBOSE) cerr << "End DisplayGLCommon::InitGL()" << endl;
}


void VW::DisplayGLCommon::GenerateTexture(unsigned int desired_width,
					  unsigned int desired_height)
{
  if (VERBOSE) cerr << "VW::DisplayGLCommon::GenerateTexture:"<< endl;
  texture_width  = GetNextPowerOf2(desired_width);
  texture_height = GetNextPowerOf2(desired_height);

  // Allocate dummy memory area of texture size
  if (texName > 0) {
    // Texture already exists, so reallocate
    glDeleteTextures(1, &texName);
  }
  glGenTextures(1, &texName);

  if (VERBOSE) cerr << "   Generated"<< endl;
  // Set the texture params
  glBindTexture(GL_TEXTURE_2D, texName);
  if (VERBOSE) cerr << "   Bound"<< endl;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  
  GLubyte *dummy_texture = new GLubyte [texture_width * texture_height * 4];
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 
	       0, GL_RGBA, GL_UNSIGNED_BYTE, dummy_texture);
  if (VERBOSE) cerr << "   Filled"<< endl;
  
  delete dummy_texture;
  if (VERBOSE) cerr << "   Done"<< endl;

  // Make sure GL coords reflect this new image
  ResetCoords(desired_width,desired_height); 
}
