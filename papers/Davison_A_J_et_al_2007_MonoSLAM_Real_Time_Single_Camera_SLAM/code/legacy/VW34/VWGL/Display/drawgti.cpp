/** 
 * @file   drawgti.cc
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

#include "drawgti.h"

#include "threedtoolglcommon.h"
#include "drawyorick811.h"


// Some constants (distances in metres)
const float block_width     = float(0.190);
const float block_depth     = float(0.190);
const float block_height    = float(0.190);
const float bar_size        = float(0.025);
const float wheel_height    = float(0.220);
const float wheel_radius    = float(0.080);
const float wheel_thickness = float(0.030);
const float wheel_spacing   = float(0.005);

void DrawGTIBlock(VW::ThreeDToolGLCommon& display,
		  const VW::Vector3D& position,
		  const VW::Quaternion& orientation,
		  bool battery=false)
{
  glEnable(GL_COLOR_MATERIAL);

  glMatrixMode(GL_MODELVIEW);

  display.PushDrawingPosition();

  // Transform into Object frame
  display.TranslateDrawingPosition(position);
  display.RotateDrawingPosition(orientation);
   
  display.SetPenColour(225,225,225,0);
  // Left
  glBegin(GL_QUADS);{
    glNormal3f(-1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,block_height);
    glVertex3f(0,bar_size,block_height);
    glVertex3f(0,bar_size,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(-1,0,0);
    glVertex3f(0,block_depth-bar_size,0);
    glVertex3f(0,block_depth-bar_size,block_height);
    glVertex3f(0,block_depth,block_height);
    glVertex3f(0,block_depth,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(-1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,bar_size);
    glVertex3f(0,block_depth,bar_size);
    glVertex3f(0,block_depth,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(-1,0,0);
    glVertex3f(0,0,block_height-bar_size);
    glVertex3f(0,0,block_height);
    glVertex3f(0,block_depth,block_height);
    glVertex3f(0,block_depth,block_height-bar_size);
  }; glEnd();
  // Right
  glBegin(GL_QUADS);{
    glNormal3f(1,0,0);
    glVertex3f(block_width,0,0);
    glVertex3f(block_width,0,block_height);
    glVertex3f(block_width,bar_size,block_height);
    glVertex3f(block_width,bar_size,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(1,0,0);
    glVertex3f(block_width,block_depth-bar_size,0);
    glVertex3f(block_width,block_depth-bar_size,block_height);
    glVertex3f(block_width,block_depth,block_height);
    glVertex3f(block_width,block_depth,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(1,0,0);
    glVertex3f(block_width,0,0);
    glVertex3f(block_width,0,bar_size);
    glVertex3f(block_width,block_depth,bar_size);
    glVertex3f(block_width,block_depth,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(1,0,0);
    glVertex3f(block_width,0,block_height-bar_size);
    glVertex3f(block_width,0,block_height);
    glVertex3f(block_width,block_depth,block_height);
    glVertex3f(block_width,block_depth,block_height-bar_size);
  }; glEnd();
  // Front
  glBegin(GL_QUADS);{
    glNormal3f(0,-1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,block_height);
    glVertex3f(bar_size,0,block_height);
    glVertex3f(bar_size,0,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,-1,0);
    glVertex3f(block_width-bar_size,0,0);
    glVertex3f(block_width-bar_size,0,block_height);
    glVertex3f(block_width,0,block_height);
    glVertex3f(block_width,0,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,-1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,bar_size);
    glVertex3f(block_width,0,bar_size);
    glVertex3f(block_width,0,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,-1,0);
    glVertex3f(0,0,block_height-bar_size);
    glVertex3f(0,0,block_height);
    glVertex3f(block_width,0,block_height);
    glVertex3f(block_width,0,block_height-bar_size);
  }; glEnd();
  // Back
  glBegin(GL_QUADS);{
    glNormal3f(0,1,0);
    glVertex3f(0,block_depth,0);
    glVertex3f(0,block_depth,block_height);
    glVertex3f(bar_size,block_depth,block_height);
    glVertex3f(bar_size,block_depth,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,1,0);
    glVertex3f(block_width-bar_size,block_depth,0);
    glVertex3f(block_width-bar_size,block_depth,block_height);
    glVertex3f(block_width,block_depth,block_height);
    glVertex3f(block_width,block_depth,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,1,0);
    glVertex3f(0,block_depth,0);
    glVertex3f(0,block_depth,bar_size);
    glVertex3f(block_width,block_depth,bar_size);
    glVertex3f(block_width,block_depth,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,1,0);
    glVertex3f(0,block_depth,block_height-bar_size);
    glVertex3f(0,block_depth,block_height);
    glVertex3f(block_width,block_depth,block_height);
    glVertex3f(block_width,block_depth,block_height-bar_size);
  }; glEnd();
  // Bottom
  glBegin(GL_QUADS);{
    glNormal3f(0,0,-1);
    glVertex3f(0,0,0);
    glVertex3f(0,block_depth,0);
    glVertex3f(bar_size,block_depth,0);
    glVertex3f(bar_size,0,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,0,-1);
    glVertex3f(block_width-bar_size,0,0);
    glVertex3f(block_width-bar_size,block_depth,0);
    glVertex3f(block_width,block_depth,0);
    glVertex3f(block_width,0,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,0,-1);
    glVertex3f(0,0,0);
    glVertex3f(0,bar_size,0);
    glVertex3f(block_width,bar_size,0);
    glVertex3f(block_width,0,0);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,0,-1);
    glVertex3f(0,block_depth-bar_size,0);
    glVertex3f(0,block_depth,0);
    glVertex3f(block_width,block_depth,0);
    glVertex3f(block_width,block_depth-bar_size,0);
  }; glEnd();
  // Top
  glBegin(GL_QUADS);{
    glNormal3f(0,0,1);
    glVertex3f(0,0,block_height);
    glVertex3f(0,block_depth,block_height);
    glVertex3f(bar_size,block_depth,block_height);
    glVertex3f(bar_size,0,block_height);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,0,1);
    glVertex3f(block_width-bar_size,0,block_height);
    glVertex3f(block_width-bar_size,block_depth,block_height);
    glVertex3f(block_width,block_depth,block_height);
    glVertex3f(block_width,0,block_height);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,0,1);
    glVertex3f(0,0,block_height);
    glVertex3f(0,bar_size,block_height);
    glVertex3f(block_width,bar_size,block_height);
    glVertex3f(block_width,0,block_height);
  }; glEnd();
  glBegin(GL_QUADS);{
    glNormal3f(0,0,1);
    glVertex3f(0,block_depth-bar_size,block_height);
    glVertex3f(0,block_depth,block_height);
    glVertex3f(block_width,block_depth,block_height);
    glVertex3f(block_width,block_depth-bar_size,block_height);
  }; glEnd();

  if (battery) {
    // Also draw battery
    
    for (int x=0;x<2;x++) {
      float x0 = (x*block_width*0.5) + (block_width*0.025);
      float x1 = (x*block_width*0.5) + (block_width*0.475);
      float y0 = 0.025 * block_depth;
      float y1 = 0.975 * block_depth;
      float z0 = 0.025 * block_height;
      float z1 = 0.9 * block_height;
      float z2 = 0.975 * block_height;

      // First the gray bits
      display.SetPenColour(127,127,127,0);
      glBegin(GL_QUADS);{
	// Bottom
	glNormal3f(0,0,-1);
	glVertex3f(x0,y0,z0);
	glVertex3f(x0,y1,z0);
	glVertex3f(x1,y1,z0);
	glVertex3f(x1,y0,z0);
	// Left
	glNormal3f(-1,0,0);
	glVertex3f(x0,y0,z0);
	glVertex3f(x0,y0,z1);
	glVertex3f(x0,y1,z1);
	glVertex3f(x0,y1,z0);
 	// Right
	glNormal3f(1,0,0);
	glVertex3f(x1,y0,z0);
	glVertex3f(x1,y0,z1);
	glVertex3f(x1,y1,z1);
	glVertex3f(x1,y1,z0);
 	// Front
	glNormal3f(0,-1,0);
	glVertex3f(x0,y0,z0);
	glVertex3f(x0,y0,z1);
	glVertex3f(x1,y0,z1);
	glVertex3f(x1,y0,z0);
 	// Back
	glNormal3f(0,-1,0);
	glVertex3f(x0,y1,z0);
	glVertex3f(x0,y1,z1);
	glVertex3f(x1,y1,z1);
	glVertex3f(x1,y1,z0);
      }; glEnd();

      // Now the black bits
      display.SetPenColour(0,0,0,0);
      glBegin(GL_QUADS);{
	// Top
	glNormal3f(0,0,1);
	glVertex3f(x0,y0,z2);
	glVertex3f(x0,y1,z2);
	glVertex3f(x1,y1,z2);
	glVertex3f(x1,y0,z2);
	// Left
	glNormal3f(-1,0,0);
	glVertex3f(x0,y0,z1);
	glVertex3f(x0,y0,z2);
	glVertex3f(x0,y1,z2);
	glVertex3f(x0,y1,z1);
 	// Right
	glNormal3f(1,0,0);
	glVertex3f(x1,y0,z1);
	glVertex3f(x1,y0,z2);
	glVertex3f(x1,y1,z2);
	glVertex3f(x1,y1,z1);
 	// Front
	glNormal3f(0,-1,0);
	glVertex3f(x0,y0,z1);
	glVertex3f(x0,y0,z2);
	glVertex3f(x1,y0,z2);
	glVertex3f(x1,y0,z1);
 	// Back
	glNormal3f(0,1,0);
	glVertex3f(x0,y1,z1);
	glVertex3f(x0,y1,z2);
	glVertex3f(x1,y1,z2);
	glVertex3f(x1,y1,z1);
      }; glEnd();
    }
  }

 display.PopDrawingPosition();
}


void DrawGTIWheel(VW::ThreeDToolGLCommon& display,
		  const VW::Vector3D& position,
		  const VW::Quaternion& orientation)
{

  display.SetPenColour(100,100,100,0);
  glEnable(GL_COLOR_MATERIAL);

  glMatrixMode(GL_MODELVIEW);
  display.PushDrawingPosition();

  // Transform into Object frame
  display.TranslateDrawingPosition(position);
  display.RotateDrawingPosition(orientation);

  // First draw the plate
  glBegin(GL_QUADS); {
    glNormal3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(block_width,0,0);
    glVertex3f(block_width,block_depth,0);
    glVertex3f(0,block_depth,0);
  }; glEnd();

  // Wheel is held by triangular plates
  float x1=(block_width-wheel_thickness)*0.5-wheel_spacing;
  float x2=(block_width+wheel_thickness)*0.5+wheel_spacing;
  float y1=block_depth*0.25;
  float y2=block_depth*0.75;
  float y3=block_depth*0.55;
  float y4=block_depth*0.45;
  float z=-(wheel_height-wheel_radius)*1.1;
  glBegin(GL_QUADS); {
    glNormal3f(-1,0,0);
    glVertex3f(x1,y1,0);
    glVertex3f(x1,y2,0);
    glVertex3f(x1,y3,z);
    glVertex3f(x1,y4,z);
  }; glEnd();
  glBegin(GL_QUADS); {
    glNormal3f(1,0,0);
    glVertex3f(x2,y1,0);
    glVertex3f(x2,y2,0);
    glVertex3f(x2,y3,z);
    glVertex3f(x2,y4,z);
  }; glEnd();
  

  display.SetPenColour(200,200,200,0);
  glTranslatef((block_width-wheel_thickness)*0.5,
	       block_depth*0.5,
	       wheel_radius-wheel_height);
  glRotatef(90,0,1,0);
  display.DrawCylinder(wheel_radius, wheel_thickness);

  display.DrawDisc(0, wheel_radius);
  glTranslatef(0,0,wheel_thickness);
  display.DrawDisc(0, wheel_radius);


  display.PopDrawingPosition();
}

/**
 * @param display the 3D display on which the object will be drawn.
 * @param position the position (relative to the current origin) at which the
 *                 object will be drawn.
 * @param orientation the orientation of the object (default is that the ground
 *                    lives in the X-Z plane).
 */
void VW::DrawGTI(VW::ThreeDToolGLCommon& display,
		 const VW::Vector3D& position,
		 const VW::Quaternion& orientation)
{
  display.PushDrawingPosition();


  glEnable(GL_COLOR_MATERIAL);

  display.TranslateDrawingPosition(position);
  display.RotateDrawingPosition(orientation);
  display.TranslateDrawingPosition(VW::Vector3D(0,wheel_height,0));

  display.IncludeInRotationCentre(VW::Vector3D(0,0,0));

  // Align vertically to make drawing easier
  glRotatef(-90,1,0,0);

  // Stuff
  display.SetPenColour(200,200,200,0);

  // Blocks
  {
    VW::Vector3D pos(-block_width*1.5,
		     -block_depth*2.5,
		     0);
    DrawGTIBlock(display,pos,Quaternion(0,0,0,1));
  }

  {
    VW::Vector3D pos(-block_width*0.5,
		     -block_depth*2.5,
		     0);
    DrawGTIBlock(display,pos,Quaternion(0,0,0,1));
  }

  {
    VW::Vector3D pos( block_width*0.5,
		     -block_depth*2.5,
		     0);
    DrawGTIBlock(display,pos,Quaternion(0,0,0,1));
  }

  {
    VW::Vector3D pos(-block_width*1.5,
		     -block_depth*1.5,
		     0);
    DrawGTIBlock(display,pos,Quaternion(0,0,0,1),true);
  }

  {
    VW::Vector3D pos(-block_width*0.5,
		     -block_depth*1.5,
		     0);
    DrawGTIBlock(display,pos,Quaternion(0,0,0,1));
  }

  {
    VW::Vector3D pos( block_width*0.5,
		     -block_depth*1.5,
		     0);
    DrawGTIBlock(display,pos,Quaternion(0,0,0,1),true);
  }

  {
    VW::Vector3D pos(-block_width*1.5,
		     -block_depth*0.5,
		     0);
    DrawGTIBlock(display,pos,Quaternion(0,0,0,1));
  }

  {
    VW::Vector3D pos(-block_width*0.5,
		     -block_depth*0.5,
		     0);
    DrawGTIBlock(display,pos,Quaternion(0,0,0,1));
  }

  {
    VW::Vector3D pos( block_width*0.5,
		     -block_depth*0.5,
		     0);
    DrawGTIBlock(display,pos,Quaternion(0,0,0,1));
  }

  // Wheels
  {
    VW::Vector3D pos(-block_width*1.5,
		 -block_depth*0.5,
		 0);
    DrawGTIWheel(display,pos,VW::Quaternion(0,0,0,1));
  }

  {
    VW::Vector3D pos( block_width*0.5,
		 -block_depth*0.5,
		 0);
    DrawGTIWheel(display,pos,VW::Quaternion(0,0,0,1));
  }

  {
    VW::Vector3D pos( -block_width*0.5,
		 -block_depth*2.5,
		 0);
    DrawGTIWheel(display,pos,VW::Quaternion(0,0,0,1));
  }

  display.PopDrawingPosition();
}

/**
 * @param display the 3D display on which the object will be drawn.
 * @param position the position (relative to the current origin) at which the
 *                 object will be drawn.
 * @param orientation the orientation of the object (default is that the ground
 *                    lives in the X-Z plane).
 * @param pan        Yorick pan angle (in degrees) -90<pan<90
 * @param tilt       Yorick tilt angle (in degrees) -60<tilt<60
 * @param vergeleft  Yorick left vergence angle (in degrees) -90<vergeleft<90
 * @param pan        Yorick right vergence angle (in degrees) -90<vergeright<90
 * @param cameratype the name of the camera to draw on the Yorick platforms, 
 *                   possible values are
 *                         - VL500
 */
void VW::DrawGTIWithYorick(VW::ThreeDToolGLCommon& display,
			 const VW::Vector3D& position,
			 const VW::Quaternion& orientation,
			 const double yorickpan, 
			 const double yoricktilt, 
			 const double yorickvergeleft, 
			 const double yorickvergeright, 
			 const std::string& yorickcameratype)
{
  display.PushDrawingPosition();


  glEnable(GL_COLOR_MATERIAL);

  display.TranslateDrawingPosition(position);
  display.RotateDrawingPosition(orientation);

  // First draw GTI
  VW::DrawGTI(display,
	      VW::Vector3D(0,0,0),
	      VW::Quaternion(0,0,0,1));

  // Now draw Yorick at the correct base position
  VW::DrawYorick811(display,
		    VW::Vector3D(0,block_height+wheel_height,0),
		    VW::Quaternion(0,0,0,1),
		    yorickpan,yoricktilt,
		    yorickvergeleft,yorickvergeright,
		    yorickcameratype);

  display.PopDrawingPosition();
}
