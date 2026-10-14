/** 
 * @file   drawyorick811.cc
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
#include "drawyorick811.h"


// Some constants (distances in metres)
const float pan_axis_radius1  = 0.060; // Lower cylinder
const float pan_axis_height1  = 0.130;
const float pan_axis_radius2  = 0.041; // Upper cylinder
const float pan_axis_height2  = 0.050;
const float pan_motor_radius  = 0.0185; // Pan motor protudes from top
const float pan_motor_height  = 0.075;
const float tilt_axis_radius1 = 0.050;
const float tilt_axis_length1 = 0.0475;
const float tilt_axis_radius2 = 0.044;
const float tilt_axis_length2 = 0.085;
const float tilt_axis_radius3 = 0.0118;
const float tilt_axis_length3 = 0.005;
const float verge_radius      = 0.03; 
const float verge_length      = 0.0875; 
const float verge_motor_radius = 0.016; 
const float verge_motor_length = 0.03; 
const float verge_offset       = 0.0025; 
const float vergestem_radius   = 0.01; 
const float platform_halfwidth = 0.05;
const float platform_halfdepth = 0.035;
const float platform_thickness = 0.017;
const float nameplatewidth     = 0.065;
const float nameplateheight    = 0.038;
const float nameplaterelief    = 0.002; // A fudge to make GL get it on top
const float tiltspotposx       = 0.092;
const float tiltspotradius     = 0.015;
const float tiltspotrelief     = 0.002;

/**
 * @param display     the ThreeDToolGLWidget (or derivative) on which to draw
 * @param position    3D position at which to begin drawing
 * @param orientation the direction in which Yorick is facing when zeroed
 * @param pan         pan angle (in degrees) -90<pan<90
 * @param tilt        tilt angle (in degrees) -60<tilt<60
 * @param vergeleft   left vergence angle (in degrees) -90<vergeleft<90
 * @param pan         right vergence angle (in degrees) -90<vergeright<90
 * @param cameratype  the name of the camera to draw on the platforms, possible *                    values are
 *                          - VL500
 */
void VW::DrawYorick811(VW::ThreeDToolGLCommon& display,
		       const VW::Vector3D& position,
		       const VW::Quaternion& orientation,
		       const double pan, const double tilt, 
		       const double vergeleft, const double vergeright, 
		       const std::string& cameratype)
{
  display.PushDrawingPosition();


  glEnable(GL_COLOR_MATERIAL);

  display.TranslateDrawingPosition(position);
  display.RotateDrawingPosition(orientation);

  display.IncludeInRotationCentre(VW::Vector3D(0,0,0));

  // Align vertically to make drawing easier
  glRotatef(-90,1,0,0);

  // Stuff
  display.SetPenColour(200,200,200,0);

  // Pan axis is vertical
  // Main cylinder
  display.DrawCylinder(pan_axis_radius1, pan_axis_height1);
  display.DrawDisc(0, pan_axis_radius1);
  glTranslatef(0,0,pan_axis_height1);
  display.DrawDisc(pan_axis_radius2, pan_axis_radius1);
  // Second pan cylinder
  display.DrawCylinder(pan_axis_radius2, pan_axis_height2);
  glTranslatef(0,0,pan_axis_height2);
  display.DrawDisc(pan_motor_radius, pan_axis_radius2);
  // Pan motor
  display.SetPenColour(10,10,10,0);
  display.DrawCylinder(pan_motor_radius,pan_motor_height);
  glTranslatef(0,0,pan_motor_height);
  display.DrawDisc(0,pan_motor_radius);

  // Tilt axis is centered on the leading edge of the 2nd Pan cylinder
  // so move back to top of 2nd cylinder and rotate by the pan angle
  glRotatef(pan,0,0,1);
  glTranslatef(0,pan_axis_radius2,-pan_motor_height);


  // First draw nameplate
  {
    const float dx = 0.5*nameplatewidth;
    const float dy = tilt_axis_radius1 + nameplaterelief;
    const float dz = 0.5*nameplateheight;

    display.SetPenColour(200,150,10,0);
    glBegin(GL_QUADS); {
      // Front
      glNormal3f(  0,   1,   0);
      glVertex3f(-dx,  dy,  dz);
      glVertex3f( dx,  dy,  dz);
      glVertex3f( dx,  dy, -dz);
      glVertex3f(-dx,  dy, -dz);
      // Top
      glNormal3f(  0,   0,   1);
      glVertex3f(-dx,  dy,  dz);
      glVertex3f( dx,  dy,  dz);
      glVertex3f( dx,   0,  dz);
      glVertex3f(-dx,   0,  dz);
      // Bottom
      glNormal3f(  0,   0,  -1);
      glVertex3f(-dx,  dy, -dz);
      glVertex3f( dx,  dy, -dz);
      glVertex3f( dx,   0, -dz);
      glVertex3f(-dx,   0, -dz);
      // Left
      glNormal3f( -1,   0,  0);
      glVertex3f(-dx,  dy, -dz);
      glVertex3f(-dx,  dy,  dz);
      glVertex3f(-dx,   0,  dz);
      glVertex3f(-dx,   0, -dz);
      // Right
      glNormal3f(  1,   0,  0);
      glVertex3f( dx,  dy, -dz);
      glVertex3f( dx,  dy,  dz);
      glVertex3f( dx,   0,  dz);
      glVertex3f( dx,   0, -dz);
    }; glEnd();
  } // End nameplate

  // Also draw cylindrical markings
  glPushMatrix();
  display.SetPenColour(200,200,200,0);
  glRotatef(-90,1,0,0);
  glTranslatef(-tiltspotposx,0,0);
  display.DrawCylinder(tiltspotradius,tiltspotrelief+tilt_axis_radius2,true);
  glTranslatef(2*tiltspotposx,0,0);
  display.DrawCylinder(tiltspotradius,tiltspotrelief+tilt_axis_radius2,true);
  glPopMatrix();


  // Tilt axis is in three parts and is horizontal
 
  glTranslatef(-tilt_axis_length1, 0,0);
  glRotatef(90,0,1,0);
  display.DrawCylinder(tilt_axis_radius1, 2*tilt_axis_length1);
  display.DrawDisc(tilt_axis_radius2, tilt_axis_radius1);
  glTranslatef(0, 0, tilt_axis_length1*2);
  display.DrawDisc(tilt_axis_radius2, tilt_axis_radius1);

  // Now cylinder 2
  glTranslatef(0, 0, -tilt_axis_length1*2-tilt_axis_length2);
  display.DrawCylinder(tilt_axis_radius2,
		       2*(tilt_axis_length1+tilt_axis_length2));
  display.DrawDisc(0, tilt_axis_radius2);
  glTranslatef(0, 0, 2*(tilt_axis_length1+tilt_axis_length2));
  display.DrawDisc(0, tilt_axis_radius2);

  // Now cylinder 3
  glTranslatef(0, 0, -tilt_axis_length3
	             -2*(tilt_axis_length1+tilt_axis_length2));
  display.DrawCylinder(tilt_axis_radius3,
		       2*(tilt_axis_length1+tilt_axis_length2+tilt_axis_length3));
  display.DrawDisc(0, tilt_axis_radius3);
  glTranslatef(0, 0, 2*(tilt_axis_length1+tilt_axis_length2+tilt_axis_length3));
  display.DrawDisc(0, tilt_axis_radius2);
  glTranslatef(0, 0, -2*(tilt_axis_length1+tilt_axis_length2+tilt_axis_length3));

  // Rotate by the tilt angle
  glRotatef(tilt,0,0,1);

  // Verg axes are vertical
  const float verge_sep=2*(verge_radius+tilt_axis_length1+tilt_axis_length2+tilt_axis_length3);
   glRotatef(-90,0,1,0);
   glTranslatef(-verge_radius, 0, tilt_axis_radius2-verge_offset-verge_length);
   glPushMatrix();
   display.DrawCylinder(verge_radius, verge_length);
   display.DrawDisc(verge_motor_radius, verge_radius);
   glTranslatef(verge_sep, 0, 0);
   display.DrawCylinder(verge_radius, verge_length);
   display.DrawDisc(verge_motor_radius, verge_radius);
   // Draw motors
   display.SetPenColour(10,10,10,0);
   glTranslatef(0, 0, -verge_motor_length);
   display.DrawCylinder(verge_motor_radius, verge_motor_length);
   display.DrawDisc(0, verge_motor_radius);
   glTranslatef(-verge_sep, 0, 0);
   display.DrawCylinder(verge_motor_radius, verge_motor_length);
   display.DrawDisc(0, verge_motor_radius);
   display.SetPenColour(200,200,200,0);
   glPopMatrix();

   // Vergeleft stuff
   glTranslatef(0, 0, verge_length+verge_offset);
   glPushMatrix();
  // Rotate by the vergl angle
   glRotatef(vergeleft,0,0,1);
   // Draw the platform
   //canvas->SetPenColour(200,0,0,0);
   glBegin(GL_QUADS); {
     // Bottom
     glNormal3f(0,0,-1);
     glVertex3f(-platform_halfwidth,-platform_halfdepth,0);
     glVertex3f(-platform_halfwidth, platform_halfdepth,0);
     glVertex3f( platform_halfwidth, platform_halfdepth,0);
     glVertex3f( platform_halfwidth,-platform_halfdepth,0);
     // Top
     glNormal3f(0,0,1);
     glVertex3f(-platform_halfwidth,-platform_halfdepth,platform_thickness);
     glVertex3f(-platform_halfwidth, platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth, platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth,-platform_halfdepth,platform_thickness);
     // Left
     glNormal3f(-1,0,0);
     glVertex3f(-platform_halfwidth,-platform_halfdepth,0);
     glVertex3f(-platform_halfwidth,-platform_halfdepth,platform_thickness);
     glVertex3f(-platform_halfwidth, platform_halfdepth,platform_thickness);
     glVertex3f(-platform_halfwidth, platform_halfdepth,0);
     // Right
     glNormal3f(1,0,0);
     glVertex3f(platform_halfwidth,-platform_halfdepth,0);
     glVertex3f(platform_halfwidth,-platform_halfdepth,platform_thickness);
     glVertex3f(platform_halfwidth, platform_halfdepth,platform_thickness);
     glVertex3f(platform_halfwidth, platform_halfdepth,0);
      // Front
     glNormal3f(0,-1,0);
     glVertex3f(-platform_halfwidth,-platform_halfdepth,0);
     glVertex3f(-platform_halfwidth,-platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth,-platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth,-platform_halfdepth,0);
      // Back
     glNormal3f(0,1,0);
     glVertex3f(-platform_halfwidth,platform_halfdepth,0);
     glVertex3f(-platform_halfwidth,platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth,platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth,platform_halfdepth,0);
   }; glEnd();

   // Draw the camera
   if (cameratype==std::string("VL500")) {
     VW::DrawVL500(display,
		   VW::Vector3D(0,0,platform_thickness),
		   VW::Quaternion(0.7071,0,0,0.7071),
		   false); // Make the base the origin
   }

   glTranslatef(0,0,-verge_offset);
   display.DrawCylinder(vergestem_radius,verge_offset);


   // Now move to the vergr stem
  glPopMatrix();
  glTranslatef(verge_sep, 0, 0);
  // Rotate by the vergr angle
  glRotatef(vergeright,0,0,1);

  display.SetPenColour(200,200,200,0);
   glBegin(GL_QUADS); {
     // Bottom
     glNormal3f(0,0,-1);
     glVertex3f(-platform_halfwidth,-platform_halfdepth,0);
     glVertex3f(-platform_halfwidth, platform_halfdepth,0);
     glVertex3f( platform_halfwidth, platform_halfdepth,0);
     glVertex3f( platform_halfwidth,-platform_halfdepth,0);
     // Top
     glNormal3f(0,0,1);
      glVertex3f(-platform_halfwidth,-platform_halfdepth,platform_thickness);
    glVertex3f(-platform_halfwidth, platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth, platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth,-platform_halfdepth,platform_thickness);
     // Left
    glNormal3f(-1,0,0);
      glVertex3f(-platform_halfwidth,-platform_halfdepth,0);
     glVertex3f(-platform_halfwidth,-platform_halfdepth,platform_thickness);
     glVertex3f(-platform_halfwidth, platform_halfdepth,platform_thickness);
     glVertex3f(-platform_halfwidth, platform_halfdepth,0);
     // Right
     glNormal3f(1,0,0);
     glVertex3f(platform_halfwidth,-platform_halfdepth,0);
     glVertex3f(platform_halfwidth,-platform_halfdepth,platform_thickness);
     glVertex3f(platform_halfwidth, platform_halfdepth,platform_thickness);
     glVertex3f(platform_halfwidth, platform_halfdepth,0);
      // Front
     glNormal3f(0,-1,0);
     glVertex3f(-platform_halfwidth,-platform_halfdepth,0);
     glVertex3f(-platform_halfwidth,-platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth,-platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth,-platform_halfdepth,0);
      // Back
     glNormal3f(0,1,0);
     glVertex3f(-platform_halfwidth,platform_halfdepth,0);
     glVertex3f(-platform_halfwidth,platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth,platform_halfdepth,platform_thickness);
     glVertex3f( platform_halfwidth,platform_halfdepth,0);
   }; glEnd();

   // Draw the camera
   if (cameratype==std::string("VL500")) {
     VW::DrawVL500(display,
		   VW::Vector3D(0,0,platform_thickness),
		   VW::Quaternion(0.7071,0,0,0.7071),
		   false); // Make the base the origin
   }

   glTranslatef(0,0,-verge_offset);
   display.DrawCylinder(vergestem_radius,verge_offset);


  display.PopDrawingPosition();
}
