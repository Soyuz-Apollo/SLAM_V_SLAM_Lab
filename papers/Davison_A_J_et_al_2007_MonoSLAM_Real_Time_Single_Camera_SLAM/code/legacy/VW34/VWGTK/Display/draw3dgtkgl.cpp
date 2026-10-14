/** 
 * @file   draw3dgtkgl.cc
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


#include "draw3dgtkgl.h"// Default image height (so that it exactly fills the view)#define DEF_IMAGE_HALF_HEIGHT 50#define Z_ADJ 1using namespace VW;// {{{ Draw(DisplayGTKGLCommon& display, const ImageMono<unsigned char>& image, pose)void DrawFunctions::Draw(DisplayGTKGLCommon& display, 			 const ImageMono<unsigned char>& image,			 const vnl_vector_fixed<float,3> position,			 const vnl_quaternion<float> orientation) {  //cout << "Draw Monoimage using GTK/GL optimised code\r";  // Make the GL widget 'current'  display.MakeCurrent();//   // Work out the zoom level//   float zx=float(display.GetWidth())/float(image.GetWidth());//   float zy=float(display.GetHeight())/float(image.GetHeight());  // Set the model position  glMatrixMode(GL_MODELVIEW);  //glPushMatrix(); // Save position for later  glLoadIdentity();  float ang=orientation.angle()*57.29578; // *180/PI  if ((ang*ang)>0.1) { // check the anle isn't zero    vnl_vector_fixed<float,3> axis(orientation.axis());    glRotatef(ang,axis[0],axis[1],axis[2]);  }  glTranslatef(position[0],	       position[1],	       position[2]);  glEnable(GL_TEXTURE_2D);  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);   glBindTexture(GL_TEXTURE_2D, *(display.GetTextureName()));  //AJD  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 		  image.GetWidth(), 		  image.GetHeight(), 		  GL_LUMINANCE,		  GL_UNSIGNED_BYTE, 		  image.GetRawBuffer());    //!AJD  float aspect=float(image.GetHeight())/float(image.GetWidth());  float useful_width=float(image.GetWidth())/float(display.GetTextureWidth());  float useful_height=float(image.GetHeight())/float(display.GetTextureHeight());  // Create a rectangle and map texture onto this  glBegin(GL_QUADS);  glTexCoord2f(0.0,          0.0);             glVertex3f(-DEF_IMAGE_HALF_HEIGHT/aspect,  DEF_IMAGE_HALF_HEIGHT, 0.0);  glTexCoord2f(0.0,          useful_height);   glVertex3f(-DEF_IMAGE_HALF_HEIGHT/aspect, -DEF_IMAGE_HALF_HEIGHT, 0.0);  glTexCoord2f(useful_width, useful_height);   glVertex3f( DEF_IMAGE_HALF_HEIGHT/aspect, -DEF_IMAGE_HALF_HEIGHT, 0.0);  glTexCoord2f(useful_width, 0.0);             glVertex3f( DEF_IMAGE_HALF_HEIGHT/aspect,  DEF_IMAGE_HALF_HEIGHT, 0.0);  glEnd();  glDisable(GL_TEXTURE_2D);  //glPopMatrix(); // Return to previous position  // That's all the GL stuff done}// }}}void DrawFunctions::Draw(DisplayGTKGLCommon& display,			 const Point3D& p) {  //cout << "Draw Point3D using GTK/GL optimised code\r";  // Make the GL widget 'current'  display.MakeCurrent();  glDisable(GL_TEXTURE_2D);  glDisable(GL_LIGHTING);  // So that colours work OK  glEnable(GL_COLOR_MATERIAL);  float delta=5,px=p.GetX(),py=p.GetY(),pz=p.GetZ();  glBegin(GL_LINE_STRIP); {    glVertex3d(  px,  py,  pz - delta);    glVertex3d(  px,  py,  pz + delta);  } glEnd();  glBegin(GL_LINE_STRIP); {    glVertex3d(  px,  py - delta,  pz);    glVertex3d(  px,  py + delta,  pz);  } glEnd();  glBegin(GL_LINE_STRIP); {    glVertex3d(  px - delta,  py,  pz);    glVertex3d(  px + delta,  py,  pz);  } glEnd();   glEnable(GL_LIGHTING);}
