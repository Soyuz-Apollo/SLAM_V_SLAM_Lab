/** 
 * @file   drawthreequarterdesk.cc
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

#include "drawthreequarterdesk.h"

// Data (all units in metres)
static const double thickness     = 0.025;
static const double size          = VW::ThreeQuarterDeskSideLength;
static const double cutout_size   = 1.050;
static const double curve_radius  = 0.300;
static const double leg_height    = VW::ThreeQuarterDeskHeight - thickness;
static const double leg_thickness = 0.025;
static const double leg_width     = 0.060;



// {{{ void DrawTop(double height,int norm)

void DrawThreeQuarterDeskTop(double height,int norm)
{
  // Top (square bits)
  glBegin(GL_TRIANGLE_STRIP);{
    glNormal3f(0,0,norm);
    glVertex3f(size,size-cutout_size,height);
    glVertex3f(size,0,height);
    glVertex3f(size-cutout_size,size-cutout_size,height);
    glVertex3f(size-cutout_size,0,height);
    glVertex3f(0,0,height);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP);{
    glNormal3f(0,0,norm);
    glVertex3f(size-cutout_size,size,height);
    glVertex3f(0,size,height);
    glVertex3f(size-cutout_size,size-cutout_size,height);
    glVertex3f(0,size-cutout_size,height);
    glVertex3f(0,0,height);
  }; glEnd();

  // Top (curve)
  double cc = size-cutout_size+curve_radius;
  double cr = curve_radius;
  glBegin(GL_TRIANGLE_STRIP);{
    glNormal3f(0,0,norm);
    glVertex3f(cc - cr, cc, height);
    glVertex3f(cc-0.966*cr,cc-0.259*cr,height);
    glVertex3f(cc-cr,cc-cr,height);
    glVertex3f(cc-0.866*cr,cc-0.500*cr,height);
    glVertex3f(cc-0.707*cr,cc-0.707*cr,height);
    glVertex3f(cc-cr,cc-cr,height);
    glVertex3f(cc-0.500*cr,cc-0.866*cr,height);
    glVertex3f(cc-0.259*cr,cc-0.966*cr,height);
    glVertex3f(cc-cr,cc-cr,height);
    glVertex3f(cc,cc-cr,height);
  }; glEnd();
}

// }}}

// {{{ void GLThreeQuarterDesk::DrawBorder()

void DrawThreeQuarterDeskBorder()
{
  // Draw border
  glBegin(GL_TRIANGLE_STRIP);{
    glNormal3f(0,1,0);
    glVertex3f(0,size,leg_height);
    glVertex3f(0,size,leg_height+thickness);
    glVertex3f(size-cutout_size,size,leg_height);
    glVertex3f(size-cutout_size,size,leg_height+thickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP);{
    glNormal3f(-1,0,0);
    glVertex3f(0,size,leg_height);
    glVertex3f(0,size,leg_height+thickness);
    glVertex3f(0,0,leg_height);
    glVertex3f(0,0,leg_height+thickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP);{
     glNormal3f(0,-1,0);
   glVertex3f(0,0,leg_height);
    glVertex3f(0,0,leg_height+thickness);
    glVertex3f(size,0,leg_height);
    glVertex3f(size,0,leg_height+thickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP);{
    glNormal3f(1,0,0);
    glVertex3f(size,0,leg_height);
    glVertex3f(size,0,leg_height+thickness);
    glVertex3f(size,size-cutout_size,leg_height);
    glVertex3f(size,size-cutout_size,leg_height+thickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP);{
     glNormal3f(1,0,0);
   glVertex3f(size-cutout_size,size-cutout_size,leg_height);
    glVertex3f(size-cutout_size,size-cutout_size,leg_height+thickness);
    glVertex3f(size-cutout_size,size,leg_height);
    glVertex3f(size-cutout_size,size,leg_height+thickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP);{
    glNormal3f(0,1,0);
    glVertex3f(size-cutout_size,size-cutout_size,leg_height);
    glVertex3f(size-cutout_size,size-cutout_size,leg_height+thickness);
    glVertex3f(size,size-cutout_size,leg_height);
    glVertex3f(size,size-cutout_size,leg_height+thickness);
  }; glEnd();
  // Curved bit
  double cc = size-cutout_size+curve_radius;
  double cr = curve_radius;
  glBegin(GL_TRIANGLE_STRIP);{
    glNormal3f(0,1,0);
    glVertex3f(cc,cc-cr,leg_height);
    glVertex3f(cc,cc-cr,leg_height+thickness);
    glVertex3f(cc-0.259*cr,cc-0.966*cr,leg_height);
    glVertex3f(cc-0.259*cr,cc-0.966*cr,leg_height+thickness);
    glVertex3f(cc-0.500*cr,cc-0.866*cr,leg_height);
    glVertex3f(cc-0.500*cr,cc-0.866*cr,leg_height+thickness);
    glVertex3f(cc-0.707*cr,cc-0.707*cr,leg_height);
    glVertex3f(cc-0.707*cr,cc-0.707*cr,leg_height+thickness);
    glVertex3f(cc-0.866*cr,cc-0.500*cr,leg_height);
    glVertex3f(cc-0.866*cr,cc-0.500*cr,leg_height+thickness);
    glVertex3f(cc-0.966*cr,cc-0.259*cr,leg_height);
    glVertex3f(cc-0.966*cr,cc-0.259*cr,leg_height+thickness);
    glVertex3f(cc-cr,cc,leg_height);
    glVertex3f(cc-cr,cc,leg_height+thickness);
  }; glEnd();
}

// }}}

// {{{ void GLThreeQuarterDesk::DrawLeg()

void DrawThreeQuarterDeskLeg(double x,double y, int dx, int dy)
{
  double offset=leg_width/2;
  double x00=0.707*(offset-0.5*leg_thickness);
  double x01=0.707*(offset+0.5*leg_thickness);
  double x10=0.707*(offset+leg_width-0.5*leg_thickness);
  double x11=0.707*(offset+leg_width+0.5*leg_thickness);
  double y00 = y + dy*x00;
  double y01 = y + dy*x01;
  double y10 = y + dy*x10;
  double y11 = y + dy*x11;
  x00 = x + dx*x00;
  x01 = x + dx*x01;
  x10 = x + dx*x10;
  x11 = x + dx*x11;
  glBegin(GL_QUADS);{
    glNormal3f(-0.707,-0.707,0);
    glVertex3f(x00,y01,0);
    glVertex3f(x00,y01,leg_height);
    glVertex3f(x01,y00,leg_height);
    glVertex3f(x01,y00,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0.707,0.707,0);
    glVertex3f(x10,y11,0);
    glVertex3f(x10,y11,leg_height);
    glVertex3f(x11,y10,leg_height);
    glVertex3f(x11,y10,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(-0.707,0.707,0);
    glVertex3f(x00,y01,0);
    glVertex3f(x00,y01,leg_height);
    glVertex3f(x10,y11,leg_height);
    glVertex3f(x10,y11,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0.707,-0.707,0);
    glVertex3f(x01,y00,0);
    glVertex3f(x01,y00,leg_height);
    glVertex3f(x11,y10,leg_height);
    glVertex3f(x11,y10,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,0,-1);
    glVertex3f(x00,y01,0);
    glVertex3f(x01,y00,0);
    glVertex3f(x11,y10,0);
    glVertex3f(x10,y11,0);
  }; glEnd();
}

// }}}



/**
 * @param display the 3D display on which the object will be drawn.
 * @param position the position (relative to the current origin) at which the
 *                 object will be drawn.
 * @param orientation the orientation of the object (default is that the ground
 *                    lives in the X-Z plane).
 * @param origin_on_desk whether to position the desk relative to
 *                       the centre of the desktop (true) or the corner 
 *                       leg (false).
 */
void VW::DrawThreeQuarterDesk(VW::ThreeDToolGLCommon& display,
			      const VW::Vector3D& position,
			      const VW::Quaternion& orientation,
			      bool origin_on_desk)
{
  display.PushDrawingPosition();


  glEnable(GL_COLOR_MATERIAL);

  display.TranslateDrawingPosition(position);
  display.RotateDrawingPosition(orientation);

  display.IncludeInRotationCentre(VW::Vector3D(0,0,0));

  glRotatef(-90,1,0,0);
  
  // Origin is centre of desk curve
  if (origin_on_desk)
    glTranslatef(cutout_size-size,cutout_size-size,-leg_height-thickness);

  display.SetPenColour(225,225,225,0);
  DrawThreeQuarterDeskTop(leg_height,-1);
  DrawThreeQuarterDeskTop(leg_height+thickness,1);
  DrawThreeQuarterDeskBorder();
  display.SetPenColour(150,150,150,0);
  DrawThreeQuarterDeskLeg(0,0,1,1);
  DrawThreeQuarterDeskLeg(size,0,-1,1);
  DrawThreeQuarterDeskLeg(0,size,1,-1);
  DrawThreeQuarterDeskLeg(size,size-cutout_size,-1,-1);
  DrawThreeQuarterDeskLeg(size-cutout_size,size,-1,-1);

  display.PopDrawingPosition();
}
