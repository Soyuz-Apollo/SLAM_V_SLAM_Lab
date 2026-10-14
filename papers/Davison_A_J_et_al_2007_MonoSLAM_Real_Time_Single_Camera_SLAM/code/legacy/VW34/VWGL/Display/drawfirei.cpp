/** 
 * @file   drawfirei.cc
 * @author BJ Tordoff
 * @date   02/02/2003
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


#include "drawfirei.h"

// m sets the direction (up=1 or down=-1), n the normal (z=1 or z=-1)
void DrawFireICurve(float width,float curveheight, int m, int n)
{
  const float curve = m*curveheight;
  static const float frac = 0.7;

  glBegin(GL_TRIANGLES); {
    glNormal3f(0, 0, n);

    glVertex3f(0,                   0,  0);
    glVertex3f(width*0.25,          0,  0);
    glVertex3f(width*0.25, frac*curve,  0);

    glVertex3f(width*0.25, frac*curve, 0);
    glVertex3f(width*0.5,       curve, 0);
    glVertex3f(width*0.5,           0, 0);

    glVertex3f(width*0.25, frac*curve, 0);
    glVertex3f(width*0.25,          0, 0);
    glVertex3f(width*0.5,           0, 0);

    glVertex3f(width*0.5,           0,  0);
    glVertex3f(width*0.75,          0,  0);
    glVertex3f(width*0.75, frac*curve,  0);

    glVertex3f(width*0.75, frac*curve, 0);
    glVertex3f(width*0.5,       curve, 0);
    glVertex3f(width*0.5,           0, 0);

    glVertex3f(width*0.75, frac*curve, 0);
    glVertex3f(width*0.75,          0, 0);
    glVertex3f(width,               0, 0);

  }; glEnd();
}
void DrawFireICurveLid(float width,float curveheight,float depth, int m)
{
  const float curve = m*curveheight;
  static const float frac = 0.7;

  const float radius = ((0.25*width*width) + (curve*curve)) / (2*curve);
  const float theta = asin(width/(2*radius));

  const float nx0 = sin(theta);
  const float ny0 = m*cos(theta);
  const float nx1 = sin(0.5*theta);
  const float ny1 = m*cos(0.5*theta);

  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(nx0, ny0, 0);
    glVertex3f(0,                   0, 0);
    glVertex3f(0,                   0, depth);

    glNormal3f(nx1, ny1, 0);
    glVertex3f(width*0.25, frac*curve, 0);
    glVertex3f(width*0.25, frac*curve, depth);

    glNormal3f(0, m, 0);
    glVertex3f(width*0.5,       curve, 0);
    glVertex3f(width*0.5,       curve, depth);

    glNormal3f(-nx1, ny1, 0);
    glVertex3f(width*0.75, frac*curve, 0);
    glVertex3f(width*0.75, frac*curve, depth);

     glNormal3f(-nx0, ny0, 0);
   glVertex3f(width,               0, 0);
    glVertex3f(width,               0, depth);

  }; glEnd();
}

/**
 * @param display the 3D display on which the object will be drawn.
 * @param position the position (relative to the current origin) at which the
 *                 object will be drawn.
 * @param orientation the orientation of the object (default is that the ground
 *                    lives in the X-Z plane).
 */
void VW::DrawFireI(VW::ThreeDToolGLCommon& display,
		 const VW::Vector3D& position,
		 const VW::Quaternion& orientation)
{
  // All distances in metres
  static const float height =     0.054;
  static const float width =      0.058;
  static const float depth =      0.024;
  static const float curve =      0.003;
  static const float lensposx =   0.032;
  static const float lensposy =   0.034;
  static const float lensradius = 0.012;
  static const float lensheight = 0.005;
  


 // Make sure that it'll draw nice and smooth
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);
  glMatrixMode(GL_MODELVIEW);

  // Get to the right location
  display.PushDrawingPosition();

  display.TranslateDrawingPosition(position);
  display.RotateDrawingPosition(orientation);
  display.PushDrawingPosition();

   // OK time to draw
  display.SetPenColour(150,150,150,0);

 
  glTranslatef(0,0,-depth/2);
  glRotatef(180,0,1,0);
  // Draw the lens area
  display.DrawCylinder(lensradius,lensheight);
  display.DrawCone(0.5*lensradius,lensradius,lensheight);
  display.SetPenColour(0,0,0,0);

  display.DrawCone(0,0.55*lensradius,0.1*lensheight);


  // Go back to original position
  display.PopDrawingPosition();

  // Move to bottom left
  glTranslatef(-lensposx,-lensposy,-depth/2);

  display.SetPenColour(150,150,150,0);

  // Top bevel
  DrawFireICurve(width,curve,-1,-1);
  DrawFireICurveLid(width,curve,depth,-1);
  glTranslatef(0,0,depth);
  DrawFireICurve(width,curve,-1,1);

  glTranslatef(0,height,-depth);
  DrawFireICurve(width,curve,1,-1);
  DrawFireICurveLid(width,curve,depth,1);
  glTranslatef(0,0,depth);
  DrawFireICurve(width,curve,1,1);
  glTranslatef(0,-height,-depth);

  // Draw front, back and sides
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0,-1);
    glVertex3f(0,0,0);
    glVertex3f(0,height,0);
    glVertex3f(width,0,0);
    glVertex3f(width,height,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0,1);
    glVertex3f(0,0,depth);
    glVertex3f(0,height,depth);
    glVertex3f(width,0,depth);
    glVertex3f(width,height,depth);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(0,height,0);
    glVertex3f(0,0,depth);
    glVertex3f(0,height,depth);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1,0,0);
    glVertex3f(width,0,0);
    glVertex3f(width,height,0);
    glVertex3f(width,0,depth);
    glVertex3f(width,height,depth);
  }; glEnd();

  display.PopDrawingPosition();


}
