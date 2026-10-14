/** 
 * @file   drawvl500.cc
 * @author BJ Tordoff
 * @date   06/02/2003
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
#include "drawvl500.h"

void DrawVL500Face(float width,
		   float depth,
		   float bevel)
{
  // Face
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(width-2*bevel,0,0);
    glVertex3f(0,0,-depth+2*bevel);
    glVertex3f(width-2*bevel,0,-depth+2*bevel);
  }; glEnd();
  // Front corner bevel
  glBegin(GL_TRIANGLES); {
    glNormal3f(-0.577,0.577,-0.577);
    glVertex3f(0,0,-depth+2*bevel);
    glVertex3f(-bevel,-bevel,-depth+2*bevel);
    glVertex3f(0,-bevel,-depth+bevel);
  }; glEnd();
  // Edge bevel
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-0.707,0.707,0);
    glVertex3f(0,0,0);
    glVertex3f(-bevel,-bevel,0);
    glVertex3f(0,0,-depth+2*bevel);
    glVertex3f(-bevel,-bevel,-depth+2*bevel);
  }; glEnd();
  // Rear corner bevel
  glBegin(GL_TRIANGLES); {
    glNormal3f(-0.577,0.577,0.577);
    glVertex3f(0,0,0);
    glVertex3f(-bevel,-bevel,0);
    glVertex3f(0,-bevel,bevel);
  }; glEnd();
  // Rear bevel
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0.707,0.707);
    glVertex3f(0,0,0);
    glVertex3f(0,-bevel,bevel);
    glVertex3f(width-2*bevel,0,0);
    glVertex3f(width-2*bevel,-bevel,bevel);
  }; glEnd();
}


// m sets the direction (up=1 or down=-1), n the normal (z=1 or z=-1)
void DrawVL500Curve(float width,float curveheight, float bevel, int m, int n)
{
  const float curve = m*curveheight;
  const float bevy = -n*bevel;
  const float bevz = m*bevel;
  static const float frac = 0.7;

  const float radius = ((0.25*width*width) + (curve*curve)) / (2*curve);
  const float theta = asin(width/(2*radius));

  const float ny=n*0.707;

  const float nx0 = 0.707*sin(theta);
  const float nz0 = 0.707*m*cos(theta);
  
  const float nx1 = 0.707*sin(0.5*theta);
  const float nz1 = 0.707*m*cos(0.5*theta);

  // Face
  glBegin(GL_TRIANGLES); {
    glNormal3f(0, n, 0);

    glVertex3f(0,          0,          0);
    glVertex3f(width*0.25, 0,          0);
    glVertex3f(width*0.25, 0, frac*curve);

    glVertex3f(width*0.25, 0, frac*curve);
    glVertex3f(width*0.5,  0,      curve);
    glVertex3f(width*0.5,  0,          0);

    glVertex3f(width*0.25, 0, frac*curve);
    glVertex3f(width*0.25, 0,          0);
    glVertex3f(width*0.5,  0,          0);

    glVertex3f(width*0.5,  0,          0);
    glVertex3f(width*0.75, 0,          0);
    glVertex3f(width*0.75, 0, frac*curve);

    glVertex3f(width*0.75, 0, frac*curve);
    glVertex3f(width*0.5,  0,      curve);
    glVertex3f(width*0.5,  0,          0);

    glVertex3f(width*0.75, 0, frac*curve);
    glVertex3f(width*0.75, 0,          0);
    glVertex3f(width,      0,          0);

  }; glEnd();
  
  // Bevel
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(nx0, ny, nz0);
    glVertex3f(0,             0,          0);
    glVertex3f(0,          bevy,       bevz);

    glNormal3f(nx1, ny, nz1);
    glVertex3f(width*0.25,    0, frac*curve);
    glVertex3f(width*0.25, bevy, frac*curve+bevz);

    glNormal3f(0, ny, -n*0.707);
    glVertex3f(width*0.5,     0,      curve);
    glVertex3f(width*0.5,  bevy,      curve+bevz);

    glNormal3f(-nx1, ny, nz1);
    glVertex3f(width*0.75,    0, frac*curve);
    glVertex3f(width*0.75, bevy, frac*curve+bevz);

    glNormal3f(-nx0, ny, nz0);
    glVertex3f(width,         0,          0);
    glVertex3f(width,      bevy,       bevz);
  }; glEnd();

 

}
 
void DrawVL500FrontQuadrant(int mx,int my,float width,float height,float frontcurve)
{
 const float frac = 0.7;
  const float radiusx = ((0.25*width*width) + (frontcurve*frontcurve)) / (2*frontcurve);
  const float radiusy = ((0.25*height*height) + (frontcurve*frontcurve)) / (2*frontcurve);
  const float thetax = asin(width/(2*radiusx));
  const float thetay = asin(height/(2*radiusy));
  const float cx = cos(thetax);
  const float sx = sin(thetax);
  const float cy = cos(thetay);
  const float sy = sin(thetay);
  const float chx = cos(0.5*thetax);
  const float shx = sin(0.5*thetax);
  const float chy = cos(0.5*thetay);
  const float shy = sin(0.5*thetay);

  const float dx = 0.5*width;
  const float dy = 0.5*height;
  const float dhx = 0.25*width;
  const float dhy = 0.25*height;

  // NB normals are only approx correct!
  glBegin(GL_TRIANGLE_FAN); {
    glNormal3f(mx*shx*chy,my*shy,-chx*chy); 
    glVertex3f(mx*dhx,my*dhy,-(2*frac)*frontcurve);

    // Points along bottom
    glNormal3f(mx*shx,0,-chx); 
    glVertex3f(mx*dhx,0,-(frac+1)*frontcurve);

    glNormal3f(mx*sx,0,-cx); 
    glVertex3f(mx*dx,0,-frontcurve);

     // Middle of left edge
     glNormal3f(mx*sx*chy,my*shy,-cx*chy); 
     glVertex3f(mx*dx,my*dhy,-frac*frontcurve);

     // Points along top
     glNormal3f(mx*sx*cy,my*sy,-cx*cy); 
     glVertex3f(mx*dx,my*dy,0);

     glNormal3f(mx*shx*cy,my*sy,-chx*cy); 
     glVertex3f(mx*dhx,my*dy,-frac*frontcurve);

     glNormal3f(0,my*sy,-cy); 
     glVertex3f(0,my*dy,-frontcurve);

     // Middle of right edge
     glNormal3f(0,my*shy,-chy); 
     glVertex3f(0,my*dhy,-(1+frac)*frontcurve);

 
  }; glEnd();
}

/**
 * @param display the 3D display on which the object will be drawn.
 * @param position the position (relative to the current origin) at which the
 *                 object will be drawn.
 * @param orientation the orientation of the object (default is that the ground
 *                    lives in the X-Z plane).
 * @param origin_is_optic_centre if true (the default) then the origin is
 *                               assumed to lie at the optic centre. If not
 *                               then the object origin is placed at the
 *                               screw-mount position.
 */
void VW::DrawVL500(VW::ThreeDToolGLCommon& display,
		 const VW::Vector3D& position,
		 const VW::Quaternion& orientation,
		 bool origin_is_optic_centre)
{
  // All distances in metres
  static const float height =     0.060;
  static const float width =      0.058;
  static const float depth =      0.099;
  static const float bevel =      0.004;
  static const float frontcurve = 0.005;
  static const float lensradius = 0.0222;
  static const float lensinnerradius = 0.0177;
  static const float lensheight =  0.002;
  static const float portposx =    0.037;
  static const float portposy =    0.001;
  static const float portheight =  0.024;
  static const float portwidth =   0.015;
  static const float portdepth =   0.004;
  static const float portthickness = 0.0015;
  


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
  
  if (!origin_is_optic_centre) {
    // Origin is screw-mount position (directly below optic centre?)
    glTranslatef(0,0.5*height,0);
  }

  display.PushDrawingPosition();

   // OK time to draw
  display.SetPenColour(175,175,180,0);

  // Move to top-rear corner
  glTranslatef(-0.5*width+bevel,0.5*height,0.5*depth-bevel);
  DrawVL500Face(width,depth,bevel);
  glTranslatef(width-bevel,-bevel,0);
  glRotatef(-90,0,0,1);
  DrawVL500Face(height,depth,bevel);
  glTranslatef(height-bevel,-bevel,0);
  glRotatef(-90,0,0,1);
  DrawVL500Face(width,depth,bevel);
  glTranslatef(width-bevel,-bevel,0);
  glRotatef(-90,0,0,1);
  DrawVL500Face(height,depth,bevel);
  glTranslatef(height-bevel,-bevel,0);
  glRotatef(-90,0,0,1);
 
  // Draw back
  glTranslatef(0,-bevel,bevel);
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(width-2*bevel,0,0);
    glVertex3f(0,-height+2*bevel,0);
    glVertex3f(width-2*bevel,-height+2*bevel,0);
  }; glEnd();

  // Draw IEEE1394 port
  glPushMatrix();
  glTranslatef(portposx,-height+2*bevel+portposy,0);
  glBegin(GL_QUADS); {

    // Outer walls
    glNormal3f(0,-1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,portdepth);
    glVertex3f(portwidth,0,portdepth);
    glVertex3f(portwidth,0,0);

    glNormal3f(0,1,0);
    glVertex3f(0,portheight,0);
    glVertex3f(0,portheight,portdepth);
    glVertex3f(portwidth,portheight,portdepth);
    glVertex3f(portwidth,portheight,0);

    glNormal3f(-1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,portdepth);
    glVertex3f(0,portheight,portdepth);
    glVertex3f(0,portheight,0);

    glNormal3f(1,0,0);
    glVertex3f(portwidth,0,0);
    glVertex3f(portwidth,0,portdepth);
    glVertex3f(portwidth,portheight,portdepth);
    glVertex3f(portwidth,portheight,0);

    // Inner walls
    glNormal3f(0,1,0);
    glVertex3f(0,portthickness,0);
    glVertex3f(0,portthickness,portdepth);
    glVertex3f(portwidth,portthickness,portdepth);
    glVertex3f(portwidth,portthickness,0);

    glNormal3f(0,-1,0);
    glVertex3f(0,portheight-portthickness,0);
    glVertex3f(0,portheight-portthickness,portdepth);
    glVertex3f(portwidth,portheight-portthickness,portdepth);
    glVertex3f(portwidth,portheight-portthickness,0);

    glNormal3f(1,0,0);
    glVertex3f(portthickness,0,0);
    glVertex3f(portthickness,0,portdepth);
    glVertex3f(portthickness,portheight,portdepth);
    glVertex3f(portthickness,portheight,0);

    glNormal3f(-1,0,0);
    glVertex3f(portwidth-portthickness,0,0);
    glVertex3f(portwidth-portthickness,0,portdepth);
    glVertex3f(portwidth-portthickness,portheight,portdepth);
    glVertex3f(portwidth-portthickness,portheight,0);

    // Top
    glNormal3f(0,0,1);
    glVertex3f(0,0,portdepth);
    glVertex3f(portwidth,0,portdepth);
    glVertex3f(portwidth,portthickness,portdepth);
    glVertex3f(0,portthickness,portdepth);

    glVertex3f(0,portheight,portdepth);
    glVertex3f(portwidth,portheight,portdepth);
    glVertex3f(portwidth,portheight-portthickness,portdepth);
    glVertex3f(0,portheight-portthickness,portdepth);

    glVertex3f(0,portthickness,portdepth);
    glVertex3f(portthickness,portthickness,portdepth);
    glVertex3f(portthickness,portheight-portthickness,portdepth);
    glVertex3f(0,portheight-portthickness,portdepth);

    glVertex3f(portwidth,portthickness,portdepth);
    glVertex3f(portwidth-portthickness,portthickness,portdepth);
    glVertex3f(portwidth-portthickness,portheight-portthickness,portdepth);
    glVertex3f(portwidth,portheight-portthickness,portdepth);


   }; glEnd();
  display.SetPenColour(105,105,110,0);
  glBegin(GL_QUADS); {
    glNormal3f(0,0,1);
    glVertex3f(portthickness,portthickness,portdepth/2);
    glVertex3f(portthickness,portheight-portthickness,portdepth/2);
    glVertex3f(portwidth-portthickness,portheight-portthickness,portdepth/2);
    glVertex3f(portwidth-portthickness,portthickness,portdepth/2);
    
   }; glEnd();
  display.SetPenColour(175,175,180,0);
  glPopMatrix();

  // Move to front corner
  glTranslatef(0,bevel,-depth+bevel);
  DrawVL500Curve(width-2*bevel,frontcurve,bevel,-1,1);
  glTranslatef(width-bevel,-bevel,0);
  glRotatef(-90,0,0,1);
  DrawVL500Curve(height-2*bevel,frontcurve,bevel,-1,1);
  glTranslatef(height-bevel,-bevel,0);
  glRotatef(-90,0,0,1);
  DrawVL500Curve(width-2*bevel,frontcurve,bevel,-1,1);
  glTranslatef(width-bevel,-bevel,0);
  glRotatef(-90,0,0,1);
  DrawVL500Curve(height-2*bevel,frontcurve,bevel,-1,1);
  glTranslatef(height-bevel,-bevel,0);
  glRotatef(-90,0,0,1);
  
  // Front face
  // (move to middle of each quadrant, and do a fan)
  display.PopDrawingPosition();
  glTranslatef(0,0,-0.5*depth);

  DrawVL500FrontQuadrant(-1,-1,width-2*bevel,height-2*bevel,frontcurve);
  DrawVL500FrontQuadrant( 1,-1,width-2*bevel,height-2*bevel,frontcurve);
  DrawVL500FrontQuadrant( 1, 1,width-2*bevel,height-2*bevel,frontcurve);
  DrawVL500FrontQuadrant(-1, 1,width-2*bevel,height-2*bevel,frontcurve);

  glRotatef(180,0,1,0);
  display.SetPenColour(80,80,80,0);
  display.DrawCylinder(lensradius,2*frontcurve+lensheight);
  display.DrawCylinder(lensinnerradius,2*frontcurve+lensheight);
  glTranslatef(0,0,2*frontcurve+lensheight);
  display.DrawDisc(lensinnerradius,lensradius);
  glTranslatef(0,0,-lensheight);
  display.SetPenColour(10,10,10,0);
  display.DrawDisc(0,lensinnerradius);

  display.PopDrawingPosition();
}
