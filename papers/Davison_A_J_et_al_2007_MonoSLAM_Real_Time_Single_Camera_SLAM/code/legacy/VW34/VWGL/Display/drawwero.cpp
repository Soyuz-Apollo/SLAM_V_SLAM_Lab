/** 
 * @file   drawwero.cpp
 * @author BJ Tordoff
 * @date   23/04/2003
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


#include "threedtoolglcommon.h"

#include "drawwero.h"


// Some constants (distances in metres)
const float motorwidth  = 0.0205;
const float motorlength1  = 0.014; // up to lip
const float motorlength2  = 0.005; // from lip to front
const float motorthick  = 0.0115;
const float motorbevel  = 0.0025;
const float motorlipwidth  = 0.0045;
const float motorlipthick  = 0.0008;
const float motorlength  = motorlength1 + motorlipthick + motorlength2;
const float motorfaciaextrawidth  = 0.0035;
const float motorfaciathick  = 0.002;
const float motorfaciaradius = 0.5 * motorthick;
const float motoraxleradius = 0.0015;
const float motoraxlelength = 0.002;

const float framethickness = 0.002;

const float motorspacing = 0.001;
const float motoroffset = framethickness + motorspacing + motorfaciathick + 0.5*motorlength;

const float frame1border = motorlipwidth;
const float frame1width = 0.008;
const float frame1depth = 0.020;
const float frame1length = motorlength2 + motorfaciathick + motorspacing + motorthick + 3*framethickness;
const float frame1ballpos = 0.005;
const float frame1ballcyllen = 0.001;
const float frame1ballcylrad1 = 0.003;
const float frame1ballcylrad2 = 0.0005;
const float frame1ballrad = 0.003;

const float frame2sidepanelwidth = 0.01;
const float frame2sidepaneldepth = 0.008;
const float frame2sidepaneldepthoffset = 0.001;

const float frame3sidepanelwidth = 0.008;
const float frame3sidepanelwidthoffset = 0.01;
const float frame3sidepaneldepth = 0.004;
const float frame3sidepaneldepthoffset = -0.003;

const float camerabasewidth = 0.0275;
const float camerabasedepth = 0.015;
const float camerabasecutout = 0.0055;
const float camerabasethick = 0.0035;
const float PCBwidth = 0.0275;
const float PCBdepth = 0.0225;
const float PCBthick = 0.001;
const float camerawidth = 0.0135;
const float cameradepth = 0.0145;
const float camerathick = 0.0045;
const float cameralugwidth = 0.0025;
const float cameralugdepth = 0.003;

const float lensbasecyllen = 0.0055;
const float lensbasecylrad = 0.0056;
const float lensfocuscyllen = 0.0055;
const float lensfocuscylrad = 0.0068;
const float lensinnercyl1len = 0.003;
const float lensinnercyl1rad1 = 0.0055;
const float lensinnercyl1rad2 = 0.004;
const float lensinnercyl2len = 0.002;
const float lensinnercyl2rad1 = 0.003;
const float lensinnercyl2rad2 = 0.0015;


const float motor2Xoffset = 0.5*motorwidth + frame1border - 0.5*frame2sidepanelwidth;
const float motor2Zoffset = -0.5*motorlength + motorlength1 + frame2sidepaneldepthoffset;

const float motor3Xoffset = 0.5*motorwidth + frame1border - frame3sidepanelwidthoffset - 0.5*frame3sidepanelwidth;
const float motor3Zoffset = -0.5*motorlength + motorlength1 + frame3sidepaneldepthoffset;



void DrawMotor(VW::ThreeDToolGLCommon& display)
{
  display.PushDrawingPosition();

  display.SetPenColour(10,10,255,0);

  display.TranslateDrawingPosition(VW::Vector3D(-0.5*motorwidth,
						-0.5*motorthick,
						0.5*motorlength));

    // Bottom face
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,-1,0);
    glVertex3f(0,                    0, -motorlength+motorbevel);
    glVertex3f(0,                    0,     0);
    glVertex3f(motorbevel,           0, -motorlength);
    glVertex3f(motorwidth-motorbevel,0, 0);
    glVertex3f(motorwidth,           0, -motorlength);
    glVertex3f(motorwidth,           0, -motorbevel);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,-1,0);
    glVertex3f(-motorlipwidth,           0,-motorlength1);
    glVertex3f(-motorlipwidth,           0,-motorlength1-motorlipthick);
    glVertex3f(motorwidth+motorlipwidth, 0,-motorlength1);
    glVertex3f(motorwidth+motorlipwidth, 0,-motorlength1-motorlipthick);
  }; glEnd();


    // Top face
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,1,0);
    glVertex3f(0,                     motorthick, -motorlength+motorbevel);
    glVertex3f(0,                     motorthick,     0);
    glVertex3f(motorbevel,            motorthick, -motorlength);
    glVertex3f(motorwidth-motorbevel, motorthick, 0);
    glVertex3f(motorwidth,            motorthick, -motorlength);
    glVertex3f(motorwidth,            motorthick, -motorbevel);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,1,0);
    glVertex3f(-motorlipwidth,           motorthick, -motorlength1);
    glVertex3f(-motorlipwidth,           motorthick, -motorlength1-motorlipthick);
    glVertex3f(motorwidth+motorlipwidth, motorthick, -motorlength1);
    glVertex3f(motorwidth+motorlipwidth, motorthick, -motorlength1-motorlipthick);
  }; glEnd();


    // Sides - left
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1,0,0);
    glVertex3f(0,         0,0);
    glVertex3f(0,motorthick,0);
    glVertex3f(0,         0,-motorlength1);
    glVertex3f(0,motorthick,-motorlength1);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0,1);
    glVertex3f(0,         0,-motorlength1);
    glVertex3f(0,motorthick,-motorlength1);
    glVertex3f(-motorlipwidth,         0,-motorlength1);
    glVertex3f(-motorlipwidth,motorthick,-motorlength1);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1,0,0);
    glVertex3f(-motorlipwidth,         0,-motorlength1);
    glVertex3f(-motorlipwidth,motorthick,-motorlength1);
    glVertex3f(-motorlipwidth,         0,-motorlength1-motorlipthick);
    glVertex3f(-motorlipwidth,motorthick,-motorlength1-motorlipthick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0,-1);
    glVertex3f(-motorlipwidth,         0,-motorlength1-motorlipthick);
    glVertex3f(-motorlipwidth,motorthick,-motorlength1-motorlipthick);
    glVertex3f(             0,         0,-motorlength1-motorlipthick);
    glVertex3f(             0,motorthick,-motorlength1-motorlipthick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1,0,0);
    glVertex3f(             0,         0,-motorlength1-motorlipthick);
    glVertex3f(             0,motorthick,-motorlength1-motorlipthick);
    glVertex3f(             0,         0,-motorlength+motorbevel);
    glVertex3f(             0,motorthick,-motorlength+motorbevel);
  }; glEnd();
  // Sides - front bevel & front
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-0.7071,0,-0.7071);
    glVertex3f(             0,         0,-motorlength+motorbevel);
    glVertex3f(             0,motorthick,-motorlength+motorbevel);
    glVertex3f(    motorbevel,         0,-motorlength);
    glVertex3f(    motorbevel,motorthick,-motorlength);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0,-1);
    glVertex3f(    motorbevel,         0,-motorlength);
    glVertex3f(    motorbevel,motorthick,-motorlength);
    glVertex3f(    motorwidth,         0,-motorlength);
    glVertex3f(    motorwidth,motorthick,-motorlength);
  }; glEnd();
  // Sides - right
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1,0,0);
    glVertex3f(    motorwidth,         0,-motorlength);
    glVertex3f(    motorwidth,motorthick,-motorlength);
    glVertex3f(    motorwidth,         0,-motorlength1-motorlipthick);
    glVertex3f(    motorwidth,motorthick,-motorlength1-motorlipthick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0,-1);
    glVertex3f(motorwidth,                       0,-motorlength1-motorlipthick);
    glVertex3f(motorwidth,              motorthick,-motorlength1-motorlipthick);
    glVertex3f(motorwidth+motorlipwidth,         0,-motorlength1-motorlipthick);
    glVertex3f(motorwidth+motorlipwidth,motorthick,-motorlength1-motorlipthick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1,0,0);
    glVertex3f(motorwidth+motorlipwidth,         0,-motorlength1-motorlipthick);
    glVertex3f(motorwidth+motorlipwidth,motorthick,-motorlength1-motorlipthick);
    glVertex3f(motorwidth+motorlipwidth,         0,-motorlength1);
    glVertex3f(motorwidth+motorlipwidth,motorthick,-motorlength1);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0,1);
    glVertex3f(motorwidth+motorlipwidth,         0,-motorlength1);
    glVertex3f(motorwidth+motorlipwidth,motorthick,-motorlength1);
    glVertex3f(motorwidth,                       0,-motorlength1);
    glVertex3f(motorwidth,              motorthick,-motorlength1);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1,0,0);
    glVertex3f(motorwidth,                       0,-motorlength1);
    glVertex3f(motorwidth,              motorthick,-motorlength1);
    glVertex3f(motorwidth,                       0,-motorbevel);
    glVertex3f(motorwidth,              motorthick,-motorbevel);
  }; glEnd();
  // Back
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0.7071,0,0.7071);
    glVertex3f(motorwidth,                     0,-motorbevel);
    glVertex3f(motorwidth,            motorthick,-motorbevel);
    glVertex3f(motorwidth-motorbevel,          0, 0);
    glVertex3f(motorwidth-motorbevel, motorthick, 0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0,1);
    glVertex3f(motorwidth-motorbevel,          0, 0);
    glVertex3f(motorwidth-motorbevel, motorthick, 0);
    glVertex3f(                    0,          0, 0);
    glVertex3f(                    0, motorthick, 0);
  }; glEnd();

 
  // Facia front
  display.TranslateDrawingPosition(VW::Vector3D(motorwidth-motorfaciaradius,
						0.5*motorthick,
						-motorlength-motorfaciathick));
  glBegin(GL_TRIANGLE_FAN); {
    glNormal3f(0,0,-1);
    glVertex3f(0,0,0);
    glVertex3f(0,motorfaciaradius,0);
    glVertex3f(0.7071*motorfaciaradius,0.7071*motorfaciaradius,0);
    glVertex3f(       motorfaciaradius,0,0);
    glVertex3f(0.7071*motorfaciaradius,-0.7071*motorfaciaradius,0);
    glVertex3f(0,-motorfaciaradius,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0,0,-1);
    glVertex3f(0,-motorfaciaradius,0);
    glVertex3f(0, motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth,-motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth, motorfaciaradius,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_FAN); {
    glNormal3f(0,0,-1);
    glVertex3f(-motorfaciaextrawidth,0,0);
    glVertex3f(-motorfaciaextrawidth,motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth-0.7071*motorfaciaradius,0.7071*motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth       -motorfaciaradius,0,0);
    glVertex3f(-motorfaciaextrawidth-0.7071*motorfaciaradius,-0.7071*motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth,-motorfaciaradius,0);
  }; glEnd();

  // Facia sides (left)
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-0.3827,-0.9239,0);
    glVertex3f(-motorfaciaextrawidth,-motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth,-motorfaciaradius,motorfaciathick);
    glVertex3f(-motorfaciaextrawidth-0.7071*motorfaciaradius,-0.7071*motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth-0.7071*motorfaciaradius,-0.7071*motorfaciaradius,motorfaciathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-0.9239,-0.3827,0);
    glVertex3f(-motorfaciaextrawidth-0.7071*motorfaciaradius,-0.7071*motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth-0.7071*motorfaciaradius,-0.7071*motorfaciaradius,motorfaciathick);
    glVertex3f(-motorfaciaextrawidth-motorfaciaradius,0,0);
    glVertex3f(-motorfaciaextrawidth-motorfaciaradius,0,motorfaciathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-0.9239, 0.3827,0);
    glVertex3f(-motorfaciaextrawidth-motorfaciaradius,0,0);
    glVertex3f(-motorfaciaextrawidth-motorfaciaradius,0,motorfaciathick);
    glVertex3f(-motorfaciaextrawidth-0.7071*motorfaciaradius, 0.7071*motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth-0.7071*motorfaciaradius, 0.7071*motorfaciaradius,motorfaciathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-0.3827, 0.9239, 0);
    glVertex3f(-motorfaciaextrawidth-0.7071*motorfaciaradius, 0.7071*motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth-0.7071*motorfaciaradius, 0.7071*motorfaciaradius,motorfaciathick);
    glVertex3f(-motorfaciaextrawidth,motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth,motorfaciaradius,motorfaciathick);
  }; glEnd();

  // Facia sides (right)
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0.3827,-0.9239,0);
    glVertex3f(0,-motorfaciaradius,0);
    glVertex3f(0,-motorfaciaradius,motorfaciathick);
    glVertex3f(0.7071*motorfaciaradius,-0.7071*motorfaciaradius,0);
    glVertex3f(0.7071*motorfaciaradius,-0.7071*motorfaciaradius,motorfaciathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0.9239,-0.3827,0);
    glVertex3f(0.7071*motorfaciaradius,-0.7071*motorfaciaradius,0);
    glVertex3f(0.7071*motorfaciaradius,-0.7071*motorfaciaradius,motorfaciathick);
    glVertex3f(motorfaciaradius,0,0);
    glVertex3f(motorfaciaradius,0,motorfaciathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0.9239, 0.3827,0);
    glVertex3f(motorfaciaradius,0,0);
    glVertex3f(motorfaciaradius,0,motorfaciathick);
    glVertex3f(0.7071*motorfaciaradius, 0.7071*motorfaciaradius,0);
    glVertex3f(0.7071*motorfaciaradius, 0.7071*motorfaciaradius,motorfaciathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0.3827, 0.9239, 0);
    glVertex3f(0.7071*motorfaciaradius, 0.7071*motorfaciaradius,0);
    glVertex3f(0.7071*motorfaciaradius, 0.7071*motorfaciaradius,motorfaciathick);
    glVertex3f(0,motorfaciaradius,0);
    glVertex3f(0,motorfaciaradius,motorfaciathick);
  }; glEnd();

  // Facia - top/bottom
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(0,motorfaciaradius,0);
    glVertex3f(0,motorfaciaradius,motorfaciathick);
    glVertex3f(-motorfaciaextrawidth,motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth,motorfaciaradius,motorfaciathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(                    0,-motorfaciaradius,0);
    glVertex3f(                    0,-motorfaciaradius,motorfaciathick);
    glVertex3f(-motorfaciaextrawidth,-motorfaciaradius,0);
    glVertex3f(-motorfaciaextrawidth,-motorfaciaradius,motorfaciathick);
  }; glEnd();

  display.SetPenColour(220,220,220,0);
  display.TranslateDrawingPosition(VW::Vector3D(0,0,-motoraxlelength));
  display.DrawCylinder(motoraxleradius,motoraxlelength);
    


  display.PopDrawingPosition();
}


void DrawMotor1Frame(VW::ThreeDToolGLCommon& display)
{
  display.PushDrawingPosition();

  display.SetPenColour(150,150,150,0);

  // Move to attachment point
  display.TranslateDrawingPosition(VW::Vector3D(-0.5*motorwidth-frame1border,
						-0.5*motorthick,
						0.5*motorlength-motorlength1-motorlipthick));
  
  // Back face
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(frame1border,motorthick,0);
    glVertex3f(0,motorthick,0);
    glVertex3f(frame1border,0,0);
    glVertex3f(0,0,0);
    glVertex3f(frame1border,-frame1border,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(frame1border+motorwidth,motorthick,0);
    glVertex3f(2*frame1border+motorwidth,motorthick,0);
    glVertex3f(frame1border+motorwidth,0,0);
    glVertex3f(2*frame1border+motorwidth,0,0);
    glVertex3f(frame1border+motorwidth,-frame1border,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(frame1border,0,0);
    glVertex3f(frame1border,-frame1border,0);
    glVertex3f(frame1border+motorwidth,0,0);
    glVertex3f(frame1border+motorwidth,-frame1border,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(frame1border+motorwidth-frame1width,-frame1border,0);
    glVertex3f(frame1border+motorwidth,-frame1border,0);
    glVertex3f(frame1border+motorwidth-frame1width,-frame1border-frame1depth,0);
    glVertex3f(frame1border+motorwidth,-frame1border-frame1depth,0);
  }; glEnd();

  // Front face
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(frame1border,motorthick,-framethickness);
    glVertex3f(0,motorthick,-framethickness);
    glVertex3f(frame1border,0,-framethickness);
    glVertex3f(0,0,-framethickness);
    glVertex3f(frame1border,-frame1border,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(frame1border+motorwidth,motorthick,-framethickness);
    glVertex3f(2*frame1border+motorwidth,motorthick,-framethickness);
    glVertex3f(frame1border+motorwidth,0,-framethickness);
    glVertex3f(2*frame1border+motorwidth,0,-framethickness);
    glVertex3f(frame1border+motorwidth,-frame1border,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(frame1border,0,-framethickness);
    glVertex3f(frame1border,-frame1border,-framethickness);
    glVertex3f(frame1border+motorwidth,0,-framethickness);
    glVertex3f(frame1border+motorwidth,-frame1border,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(frame1border+motorwidth-frame1width,-frame1border,-framethickness);
    glVertex3f(frame1border+motorwidth,-frame1border,-framethickness);
    glVertex3f(frame1border+motorwidth-frame1width,-frame1border-frame1depth+framethickness,-framethickness);
    glVertex3f(frame1border+motorwidth,-frame1border-frame1depth+framethickness,-framethickness);
  }; glEnd();

  // Sides
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(frame1border,motorthick,0);
    glVertex3f(frame1border,motorthick,-framethickness);
    glVertex3f(0,motorthick,0);
    glVertex3f(0,motorthick,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,motorthick,0);
    glVertex3f(0,motorthick,-framethickness);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-0.7071, -0.7071, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-framethickness);
    glVertex3f(frame1border,-frame1border,0);
    glVertex3f(frame1border,-frame1border,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(frame1border,-frame1border,0);
    glVertex3f(frame1border,-frame1border,-framethickness);
    glVertex3f(frame1border+motorwidth-frame1width,-frame1border,0);
    glVertex3f(frame1border+motorwidth-frame1width,-frame1border,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(frame1border+motorwidth-frame1width,-frame1border,0);
    glVertex3f(frame1border+motorwidth-frame1width,-frame1border,-framethickness);
    glVertex3f(frame1border+motorwidth-frame1width,-frame1border-frame1depth,0);
    glVertex3f(frame1border+motorwidth-frame1width,-frame1border-frame1depth+framethickness,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(frame1border+motorwidth,-frame1border,0);
    glVertex3f(frame1border+motorwidth,-frame1border,-framethickness);
    glVertex3f(frame1border+motorwidth,-frame1border-frame1depth,0);
    glVertex3f(frame1border+motorwidth,-frame1border-frame1depth+framethickness,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0.7071, -0.7071, 0);
    glVertex3f(frame1border+motorwidth,-frame1border,0);
    glVertex3f(frame1border+motorwidth,-frame1border,-framethickness);
    glVertex3f(2*frame1border+motorwidth,0,0);
    glVertex3f(2*frame1border+motorwidth,0,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(2*frame1border+motorwidth,0,0);
    glVertex3f(2*frame1border+motorwidth,0,-framethickness);
    glVertex3f(2*frame1border+motorwidth,motorthick,0);
    glVertex3f(2*frame1border+motorwidth,motorthick,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(2*frame1border+motorwidth,motorthick,0);
    glVertex3f(2*frame1border+motorwidth,motorthick,-framethickness);
    glVertex3f(frame1border+motorwidth,motorthick,0);
    glVertex3f(frame1border+motorwidth,motorthick,-framethickness);
  }; glEnd();

  // Ball joint
   display.PushDrawingPosition();
  display.TranslateDrawingPosition(VW::Vector3D(frame1border+motorwidth-0.5*frame1width,
						-frame1border-frame1depth+frame1ballpos,
						0));
  display.DrawCylinder(frame1ballcylrad1,frame1ballcyllen,true);
  display.TranslateDrawingPosition(VW::Vector3D(0,0,frame1ballcyllen));
  display.DrawCylinder(frame1ballcylrad2,frame1ballcyllen);
  display.TranslateDrawingPosition(VW::Vector3D(0,0,frame1ballcyllen+frame1ballrad));
  display.DrawSphere(frame1ballrad);
  display.PopDrawingPosition();
 


  // Bottom bit
  display.TranslateDrawingPosition(VW::Vector3D(frame1border+motorwidth-frame1width,
						-frame1border-frame1depth,
						0));

  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(0,0,0);
    glVertex3f(frame1width,0,0);
    glVertex3f(0,0,-frame1length);
    glVertex3f(frame1width,0,-frame1length);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(frame1width,0,0);
    glVertex3f(frame1width,framethickness,-framethickness);
    glVertex3f(frame1width,0,-frame1length);
    glVertex3f(frame1width,framethickness,-frame1length+framethickness);
  }; glEnd();
   glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(0,framethickness,-framethickness);
    glVertex3f(frame1width,framethickness,-framethickness);
    glVertex3f(0,framethickness,-frame1length+framethickness);
    glVertex3f(frame1width,framethickness,-frame1length+framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,framethickness,-framethickness);
    glVertex3f(0,0,-frame1length);
    glVertex3f(0,framethickness,-frame1length+framethickness);
  }; glEnd();

  // Far side bit
  display.TranslateDrawingPosition(VW::Vector3D(0,0,-frame1length));
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(0,0,0);
    glVertex3f(frame1width,0,0);
    glVertex3f(0,frame1depth+frame1border+motorthick,0);
    glVertex3f(frame1width,frame1depth+frame1border+motorthick,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(0,framethickness,framethickness);
    glVertex3f(frame1width,framethickness,framethickness);
    glVertex3f(0,frame1depth+frame1border+motorthick,framethickness);
    glVertex3f(frame1width,frame1depth+frame1border+motorthick,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(frame1width,0,0);
    glVertex3f(frame1width,frame1depth+frame1border+motorthick,0);
    glVertex3f(frame1width,framethickness,framethickness);
    glVertex3f(frame1width,frame1depth+frame1border+motorthick,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,frame1depth+frame1border+motorthick,0);
    glVertex3f(0,framethickness,framethickness);
    glVertex3f(0,frame1depth+frame1border+motorthick,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(          0,frame1depth+frame1border+motorthick,0);
    glVertex3f(          0,frame1depth+frame1border+motorthick,framethickness);
    glVertex3f(frame1width,frame1depth+frame1border+motorthick,0);
    glVertex3f(frame1width,frame1depth+frame1border+motorthick,framethickness);
  }; glEnd();


  display.PopDrawingPosition();
}



void DrawMotor2Frame(VW::ThreeDToolGLCommon& display)
{
  display.PushDrawingPosition();

  display.SetPenColour(150,150,150,0);

  // Move to attachment point
  display.TranslateDrawingPosition(VW::Vector3D(-0.5*motorwidth-frame1border,
						-0.5*motorthick-framethickness,
						0.5*motorlength-motorlength1));
  
  // Main section
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(0,0,0);
    glVertex3f(2*frame1border+motorwidth,0,0);
    glVertex3f(0,2*framethickness+motorthick,0);
    glVertex3f(2*frame1border+motorwidth,2*framethickness+motorthick,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(0,0,framethickness);
    glVertex3f(2*frame1border+motorwidth,0,framethickness);
    glVertex3f(0,2*framethickness+motorthick,framethickness);
    glVertex3f(2*frame1border+motorwidth,2*framethickness+motorthick,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,                          0,0);
    glVertex3f(0,                          0,framethickness);
    glVertex3f(0,2*framethickness+motorthick,0);
    glVertex3f(0,2*framethickness+motorthick,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(2*frame1border+motorwidth,                          0,0);
    glVertex3f(2*frame1border+motorwidth,                          0,framethickness);
    glVertex3f(2*frame1border+motorwidth,2*framethickness+motorthick,0);
    glVertex3f(2*frame1border+motorwidth,2*framethickness+motorthick,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,framethickness);
    glVertex3f(2*frame1border+motorwidth,0,0);
    glVertex3f(2*frame1border+motorwidth,0,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(0,2*framethickness+motorthick,0);
    glVertex3f(0,2*framethickness+motorthick,framethickness);
    glVertex3f(2*frame1border+motorwidth,2*framethickness+motorthick,0);
    glVertex3f(2*frame1border+motorwidth,2*framethickness+motorthick,framethickness);
  }; glEnd();

  // Side panel
  
  // Move to attachment point
  display.TranslateDrawingPosition(VW::Vector3D(0,
						motorthick+framethickness,
						-0.5*frame2sidepaneldepth-frame2sidepaneldepthoffset));

  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(0,0,0);
    glVertex3f(frame2sidepanelwidth,0,0);
    glVertex3f(0,0,frame2sidepaneldepth);
    glVertex3f(frame2sidepanelwidth,0,frame2sidepaneldepth);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(0,framethickness,0);
    glVertex3f(frame2sidepanelwidth,framethickness,0);
    glVertex3f(0,framethickness,frame2sidepaneldepth);
    glVertex3f(frame2sidepanelwidth,framethickness,frame2sidepaneldepth);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,framethickness,0);
    glVertex3f(0,0,frame2sidepaneldepth);
    glVertex3f(0,framethickness,frame2sidepaneldepth);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(frame2sidepanelwidth,0,0);
    glVertex3f(frame2sidepanelwidth,framethickness,0);
    glVertex3f(frame2sidepanelwidth,0,frame2sidepaneldepth);
    glVertex3f(frame2sidepanelwidth,framethickness,frame2sidepaneldepth);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(0,0,0);
    glVertex3f(frame2sidepanelwidth,0,0);
    glVertex3f(0,framethickness,0);
    glVertex3f(frame2sidepanelwidth,framethickness,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(0,0,frame2sidepaneldepth);
    glVertex3f(frame2sidepanelwidth,0,frame2sidepaneldepth);
    glVertex3f(0,framethickness,frame2sidepaneldepth);
    glVertex3f(frame2sidepanelwidth,framethickness,frame2sidepaneldepth);
  }; glEnd();

  display.PopDrawingPosition();
}


void DrawMotor3Frame(VW::ThreeDToolGLCommon& display)
{
  display.PushDrawingPosition();

  display.SetPenColour(150,150,150,0);

  // Move to attachment point
  display.TranslateDrawingPosition(VW::Vector3D(-0.5*motorwidth-frame1border,
						-0.5*motorthick-framethickness,
						0.5*motorlength-motorlength1));
  
  // Main section
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(0,0,0);
    glVertex3f(2*frame1border+motorwidth,0,0);
    glVertex3f(0,framethickness+motorthick,0);
    glVertex3f(2*frame1border+motorwidth,framethickness+motorthick,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(0,0,framethickness);
    glVertex3f(2*frame1border+motorwidth,0,framethickness);
    glVertex3f(0,framethickness+motorthick,framethickness);
    glVertex3f(2*frame1border+motorwidth,framethickness+motorthick,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,                        0,0);
    glVertex3f(0,                        0,framethickness);
    glVertex3f(0,framethickness+motorthick,0);
    glVertex3f(0,framethickness+motorthick,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(2*frame1border+motorwidth,                        0,0);
    glVertex3f(2*frame1border+motorwidth,                        0,framethickness);
    glVertex3f(2*frame1border+motorwidth,framethickness+motorthick,0);
    glVertex3f(2*frame1border+motorwidth,framethickness+motorthick,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,framethickness);
    glVertex3f(2*frame1border+motorwidth,0,0);
    glVertex3f(2*frame1border+motorwidth,0,framethickness);
  }; glEnd();
  // Top comes in two parts to not overlap camera
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(0,framethickness+motorthick,0);
    glVertex3f(0,framethickness+motorthick,framethickness);
    glVertex3f(frame1border,framethickness+motorthick,0);
    glVertex3f(frame1border,framethickness+motorthick,framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(frame1border+motorwidth,framethickness+motorthick,0);
    glVertex3f(frame1border+motorwidth,framethickness+motorthick,framethickness);
    glVertex3f(2*frame1border+motorwidth,framethickness+motorthick,0);
    glVertex3f(2*frame1border+motorwidth,framethickness+motorthick,framethickness);
  }; glEnd();

  // Side panel
  
  // Move to attachment point
  display.TranslateDrawingPosition(VW::Vector3D(frame3sidepanelwidthoffset,
						0,
						-0.5*frame3sidepaneldepth-frame3sidepaneldepthoffset));

  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(0,0,0);
    glVertex3f(frame3sidepanelwidth,0,0);
    glVertex3f(0,0,frame3sidepaneldepth);
    glVertex3f(frame3sidepanelwidth,0,frame3sidepaneldepth);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(0,framethickness,0);
    glVertex3f(frame3sidepanelwidth,framethickness,0);
    glVertex3f(0,framethickness,frame3sidepaneldepth);
    glVertex3f(frame3sidepanelwidth,framethickness,frame3sidepaneldepth);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,framethickness,0);
    glVertex3f(0,0,frame3sidepaneldepth);
    glVertex3f(0,framethickness,frame3sidepaneldepth);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(frame3sidepanelwidth,0,0);
    glVertex3f(frame3sidepanelwidth,framethickness,0);
    glVertex3f(frame3sidepanelwidth,0,frame3sidepaneldepth);
    glVertex3f(frame3sidepanelwidth,framethickness,frame3sidepaneldepth);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(0,0,0);
    glVertex3f(frame3sidepanelwidth,0,0);
    glVertex3f(0,framethickness,0);
    glVertex3f(frame3sidepanelwidth,framethickness,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(0,0,frame3sidepaneldepth);
    glVertex3f(frame3sidepanelwidth,0,frame3sidepaneldepth);
    glVertex3f(0,framethickness,frame3sidepaneldepth);
    glVertex3f(frame3sidepanelwidth,framethickness,frame3sidepaneldepth);
  }; glEnd();

  display.PopDrawingPosition();
}



void DrawCamera(VW::ThreeDToolGLCommon& display)
{
  display.PushDrawingPosition();

  // Move to attachment point
  display.TranslateDrawingPosition(VW::Vector3D(-0.5*camerabasewidth,
						-0.5*camerabasedepth,
						framethickness));

  // First the steel
  display.SetPenColour(150,150,150,0);
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(0,0,0);
    glVertex3f(camerabasewidth,0,0);
    glVertex3f(0,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasecutout,camerabasedepth,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(0,0,-framethickness);
    glVertex3f(camerabasewidth,0,-framethickness);
    glVertex3f(0,camerabasedepth-camerabasecutout,-framethickness);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,-framethickness);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,-framethickness);
    glVertex3f(camerabasecutout,camerabasedepth,-framethickness);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-framethickness);
    glVertex3f(0,camerabasedepth-camerabasecutout,0);
    glVertex3f(0,camerabasedepth-camerabasecutout,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
   glVertex3f(0,camerabasedepth-camerabasecutout,0);
    glVertex3f(0,camerabasedepth-camerabasecutout,-framethickness);
   glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,-framethickness);
    glVertex3f(camerabasecutout,camerabasedepth,0);
    glVertex3f(camerabasecutout,camerabasedepth,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(camerabasecutout,camerabasedepth,0);
    glVertex3f(camerabasecutout,camerabasedepth,-framethickness);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,-framethickness);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,-framethickness);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,-framethickness);
    glVertex3f(camerabasewidth,0,0);
    glVertex3f(camerabasewidth,0,-framethickness);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(camerabasewidth,0,0);
    glVertex3f(camerabasewidth,0,-framethickness);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-framethickness);
  }; glEnd();


  // Then the plastic
  display.SetPenColour(50,50,50,0);
  display.TranslateDrawingPosition(VW::Vector3D(0,0,-framethickness));
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(0,0,0);
    glVertex3f(camerabasewidth,0,0);
    glVertex3f(0,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasecutout,camerabasedepth,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(0,0,-camerabasethick);
    glVertex3f(camerabasewidth,0,-camerabasethick);
    glVertex3f(0,camerabasedepth-camerabasecutout,-camerabasethick);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,-camerabasethick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,-camerabasethick);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,-camerabasethick);
    glVertex3f(camerabasecutout,camerabasedepth,-camerabasethick);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,-camerabasethick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-camerabasethick);
    glVertex3f(0,camerabasedepth-camerabasecutout,0);
    glVertex3f(0,camerabasedepth-camerabasecutout,-camerabasethick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
   glVertex3f(0,camerabasedepth-camerabasecutout,0);
    glVertex3f(0,camerabasedepth-camerabasecutout,-camerabasethick);
   glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,-camerabasethick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasecutout,camerabasedepth-camerabasecutout,-camerabasethick);
    glVertex3f(camerabasecutout,camerabasedepth,0);
    glVertex3f(camerabasecutout,camerabasedepth,-camerabasethick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(camerabasecutout,camerabasedepth,0);
    glVertex3f(camerabasecutout,camerabasedepth,-camerabasethick);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,-camerabasethick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth,-camerabasethick);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,-camerabasethick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth-camerabasecutout,camerabasedepth-camerabasecutout,-camerabasethick);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,-camerabasethick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,0);
    glVertex3f(camerabasewidth,camerabasedepth-camerabasecutout,-camerabasethick);
    glVertex3f(camerabasewidth,0,0);
    glVertex3f(camerabasewidth,0,-camerabasethick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(camerabasewidth,0,0);
    glVertex3f(camerabasewidth,0,-camerabasethick);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-camerabasethick);
  }; glEnd();

  // Then the PCB
    display.SetPenColour(85,255,10,0);
  display.TranslateDrawingPosition(VW::Vector3D(0.5*(camerabasewidth-PCBwidth),
						0.5*(camerabasedepth-PCBdepth),
						-camerabasethick));
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, 1);
    glVertex3f(0,0,0);
    glVertex3f(PCBwidth,0,0);
    glVertex3f(0,PCBdepth,0);
    glVertex3f(PCBwidth,PCBdepth,0);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(0,0,-PCBthick);
    glVertex3f(PCBwidth,0,-PCBthick);
    glVertex3f(0,PCBdepth,-PCBthick);
    glVertex3f(PCBwidth,PCBdepth,-PCBthick);
  }; glEnd();
    display.SetPenColour(155,255,10,0);
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-PCBthick);
    glVertex3f(0,PCBdepth,0);
    glVertex3f(0,PCBdepth,-PCBthick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(0,PCBdepth,0);
    glVertex3f(0,PCBdepth,-PCBthick);
    glVertex3f(PCBwidth,PCBdepth,0);
    glVertex3f(PCBwidth,PCBdepth,-PCBthick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(PCBwidth,PCBdepth,0);
    glVertex3f(PCBwidth,PCBdepth,-PCBthick);
    glVertex3f(PCBwidth,0,0);
    glVertex3f(PCBwidth,0,-PCBthick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(PCBwidth,0,0);
    glVertex3f(PCBwidth,0,-PCBthick);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-PCBthick);
  }; glEnd();


  // Now the camera itself
  display.SetPenColour(50,50,50,0);
  display.TranslateDrawingPosition(VW::Vector3D(0.5*(PCBwidth - camerawidth),
						0.5*(PCBdepth - cameradepth),
						-PCBthick));
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(0,0,-camerathick);
    glVertex3f(camerawidth,0,-camerathick);
    glVertex3f(0,cameradepth,-camerathick);
    glVertex3f(camerawidth,cameradepth,-camerathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,          0,0);
    glVertex3f(0,          0,-camerathick);
    glVertex3f(0,cameradepth,0);
    glVertex3f(0,cameradepth,-camerathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(0,cameradepth,0);
    glVertex3f(0,cameradepth,-camerathick);
    glVertex3f(camerawidth,cameradepth,0);
    glVertex3f(camerawidth,cameradepth,-camerathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(camerawidth,cameradepth,0);
    glVertex3f(camerawidth,cameradepth,-camerathick);
    glVertex3f(camerawidth,0,0);
    glVertex3f(camerawidth,0,-camerathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(camerawidth,0,0);
    glVertex3f(camerawidth,0,-camerathick);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-camerathick);
  }; glEnd();

  // lug
  display.TranslateDrawingPosition(VW::Vector3D(0.5*(camerawidth-cameralugwidth),
						  -cameralugdepth,
						  0));
    
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 0, -1);
    glVertex3f(0,0,-camerathick);
    glVertex3f(cameralugwidth,0,-camerathick);
    glVertex3f(0,             cameradepth+2*cameralugdepth,-camerathick);
    glVertex3f(cameralugwidth,cameradepth+2*cameralugdepth,-camerathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(-1, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-camerathick);
    glVertex3f(0,cameradepth+2*cameralugdepth,0);
    glVertex3f(0,cameradepth+2*cameralugdepth,-camerathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, 1, 0);
    glVertex3f(0,cameradepth+2*cameralugdepth,0);
    glVertex3f(0,cameradepth+2*cameralugdepth,-camerathick);
    glVertex3f(cameralugwidth,cameradepth+2*cameralugdepth,0);
    glVertex3f(cameralugwidth,cameradepth+2*cameralugdepth,-camerathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(1, 0, 0);
    glVertex3f(cameralugwidth,cameradepth+2*cameralugdepth,0);
    glVertex3f(cameralugwidth,cameradepth+2*cameralugdepth,-camerathick);
    glVertex3f(cameralugwidth,0,0);
    glVertex3f(cameralugwidth,0,-camerathick);
  }; glEnd();
  glBegin(GL_TRIANGLE_STRIP); {
    glNormal3f(0, -1, 0);
    glVertex3f(cameralugwidth,0,0);
    glVertex3f(cameralugwidth,0,-camerathick);
    glVertex3f(0,0,0);
    glVertex3f(0,0,-camerathick);
  }; glEnd();


  // Now the lens assembly
  display.TranslateDrawingPosition(VW::Vector3D(0.5*cameralugwidth,
						  cameralugdepth+0.5*cameradepth,
						-camerathick-lensbasecyllen));

  display.DrawCylinder(lensbasecylrad,lensbasecyllen,false);
  display.DrawDisc(lensbasecylrad,lensfocuscylrad);

  display.TranslateDrawingPosition(VW::Vector3D(0,0,-lensfocuscyllen));
  display.DrawCylinder(lensfocuscylrad,lensfocuscyllen,false);
  display.DrawDisc(lensinnercyl1rad1,lensfocuscylrad);
  display.DrawCone(lensinnercyl1rad1,lensinnercyl1rad2,lensinnercyl1len);

  display.TranslateDrawingPosition(VW::Vector3D(0,0,lensinnercyl1len));
  display.DrawDisc(lensinnercyl2rad1,lensinnercyl1rad2);
  display.DrawCone(lensinnercyl2rad1,lensinnercyl2rad2,lensinnercyl2len);
  display.TranslateDrawingPosition(VW::Vector3D(0,0,lensinnercyl2len));
  display.DrawDisc(0,lensinnercyl2rad2);


  display.PopDrawingPosition();
}













/**
 * @param display     the ThreeDToolGLCommon (or derivative) on which to draw
 * @param position    3D position at which to begin drawing
 * @param orientation the direction in which Yorick is facing when zeroed
 * @param tilt        tilt angle (in degrees) -60<tilt<60
 * @param pan         pan angle (in degrees) -90<pan<90
 * @param cyclo       cyclo-rotation angle (in degrees) -90<cyclo<90
 * @param origin_at_camera_centre if true the robot is drawn start with the camera at the origin, then following the kinematic chain backwards, but otherwise it is drawn from the ball-joint upwards.
 */
void VW::DrawWeRo(VW::ThreeDToolGLCommon& display,
		  const VW::Vector3D& position,
		  const VW::Quaternion& orientation,
		  const double tilt, 
		  const double pan, 
		  const double cyclo,
		  const bool origin_at_camera_centre)
{
  display.PushDrawingPosition();

  // Move to the right place
  display.TranslateDrawingPosition(position);
  display.RotateDrawingPosition(orientation);

  if (origin_at_camera_centre) {
    // Move so that approx optic centre is at origin
    static const float opx = 0;
    static const float opy = 0;
    static const float opz = framethickness + camerabasethick + PCBthick + camerathick;
  glTranslatef(opx,opy,opz);

    DrawCamera(display);

    glRotatef(-cyclo,0,0,1);

    glTranslatef(-0.5*motorwidth+motorfaciaradius,0,motoroffset);

    DrawMotor(display);
    DrawMotor3Frame(display);

  glTranslatef(-motor3Xoffset,0,-motor3Zoffset);
  glRotatef(90,1,0,0);
  glRotatef(-pan-90,0,0,1);
  glTranslatef(-0.5*motorwidth+motorfaciaradius,0,motoroffset+0.5*motorthick);


  DrawMotor(display);
  DrawMotor2Frame(display);

  glTranslatef(-motor2Xoffset,0,-motor2Zoffset);

  glRotatef(-90,1,0,0);
  glRotatef(-tilt,0,0,1);

  glTranslatef(-0.5*motorwidth+motorfaciaradius,0,motoroffset+0.5*motorthick);

  DrawMotor(display);
  DrawMotor1Frame(display);

  } else {

  // Move so that ball is at origin
  static const float ballx = -0.5*motorwidth + 0.5*frame1width;
  static const float bally = 0.5*motorthick + frame1border + frame1depth - frame1ballpos;
  static const float ballz = -0.5*motorlength + motorlength1 + motorlipthick - 2*frame1ballcyllen - frame1ballrad;

  display.TranslateDrawingPosition(VW::Vector3D(ballx,bally,ballz));


  display.IncludeInRotationCentre(VW::Vector3D(0,0,0));


  DrawMotor(display);
  DrawMotor1Frame(display);

  glTranslatef(0.5*motorwidth-motorfaciaradius,0,-motoroffset-0.5*motorthick);

  glRotatef(tilt,0,0,1);
  glRotatef(90,1,0,0);

  glTranslatef(motor2Xoffset,0,motor2Zoffset);
  DrawMotor(display);
  DrawMotor2Frame(display);

  glTranslatef(0.5*motorwidth-motorfaciaradius,0,-motoroffset-0.5*motorthick);

  glRotatef(pan+90,0,0,1);
  glRotatef(-90,1,0,0);

  glTranslatef(motor3Xoffset,0,motor3Zoffset);
  DrawMotor(display);
  DrawMotor3Frame(display);

  glTranslatef(0.5*motorwidth-motorfaciaradius,0,-motoroffset);

  glRotatef(cyclo,0,0,1);

  DrawCamera(display);

  }

  display.PopDrawingPosition();

}
