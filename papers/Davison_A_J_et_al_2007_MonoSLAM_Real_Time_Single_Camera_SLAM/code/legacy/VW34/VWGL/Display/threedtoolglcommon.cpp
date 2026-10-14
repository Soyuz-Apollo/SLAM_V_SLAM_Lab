/** 
 * @file   threedtoolglcommon.cpp
 * @author AJ Davison, BJ Tordoff
 * @date   09/09/2002
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


// System includes
#ifdef _WIN32
#include <windows.h>
#endif

#include <math.h>
#include <VNL/matrixfixed.h>
#include <VNL/Algo/symmetriceigensystem.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#ifdef _WIN32
	#define M_PI 3.141592653
	#define GL_BGRA GL_RGBA 
#endif


// VW includes
#include <VW/Image/imagemono.h>
#include <VW/Image/imagergb.h>
#include <VW/Image/imagecopy.h>

#include <VW/GeomObjects/quaternion.h>
#include <VW/GeomObjects/rotationmatrix.h>
#include <VW/GeomObjects/calibrationmatrix.h>
#include <VW/GeomCompute/radialdistortion.h>

// Local includes
#include "threedtoolglcommon.h"


#define VERBOSE false

using namespace std;
using namespace VW;

const unsigned int VW::ThreeDToolGLCommon::DEFAULT_CAMERA_WIDTH = 640;
const unsigned int VW::ThreeDToolGLCommon::DEFAULT_CAMERA_HEIGHT = 480;
const double VW::ThreeDToolGLCommon::DEFAULT_WIDTH_OVER_FKU = 0.75;
const double VW::ThreeDToolGLCommon::TRACKBALLSIZE = 0.8;
const double VW::ThreeDToolGLCommon::move_clipping_plane_factor = 0.999999;
const double VW::ThreeDToolGLCommon::zN = 0.01;
const double VW::ThreeDToolGLCommon::zF = 100000;

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





DrawingPosition::DrawingPosition()
{
  Zero();
}

void DrawingPosition::Zero()
{
  rW.Set(0.0 , 0.0 , 0.0);
  qWO.Set(0.0 , 0.0 , 0.0 , 1.0);
}


// {{{ RotationCentre::RotationCentre()

RotationCentre::RotationCentre(DrawingPosition &current_drawing_position)
  : drawing_position(current_drawing_position)
{
  Reset();
}

// }}}

// {{{ void RotationCentre::Reset()

void RotationCentre::Reset()
{
  initialised = false;
 
  centre.Set(0.0, 0.0, 0.0);

  xmin = xmax = ymin = ymax = zmin = zmax = 0.0;
}

// }}}

// {{{ void RotationCentre::Include(const Vector3D& vO)

void RotationCentre::Include(const Vector3D& vO)
{
  // vO is in current drawing frame; translate back into world frame
  Vector3D vW = drawing_position.rW 
    + drawing_position.qWO.RotationMatrix() * vO;

  if (initialised == false) {
    centre = vW;
    xmin = centre.GetX();
    xmax = centre.GetX();
    ymin = centre.GetY();
    ymax = centre.GetY();
    zmin = centre.GetZ();
    xmax = centre.GetZ();
    initialised = true;
  }
  else {
    if (vW.GetX() < xmin) xmin = vW.GetX();
    if (vW.GetX() > xmax) xmax = vW.GetX();
    if (vW.GetY() < ymin) ymin = vW.GetY();
    if (vW.GetY() > ymax) ymax = vW.GetY();
    if (vW.GetZ() < zmin) zmin = vW.GetZ();
    if (vW.GetZ() > zmax) zmax = vW.GetZ();

    centre.SetX((xmin + xmax) / 2.0); 
    centre.SetY((ymin + ymax) / 2.0); 
    centre.SetZ((zmin + zmax) / 2.0); 
  }

  if (VERBOSE) {
    cerr << "Rotation centre: including " << vW << " New centre " << centre
	 << endl;
  }
}

// }}}

// {{{ void RotationCentre::IncludeCurrent()

void RotationCentre::IncludeCurrent()
{
  Include(Vector3D(0,0,0));
}

// }}}

// {{{ ThreeDToolGLCommon::ThreeDToolGLCommon

ThreeDToolGLCommon::ThreeDToolGLCommon () 
  : texture_interpolation(GL_NEAREST), 
    camera_width(DEFAULT_CAMERA_WIDTH), camera_height(DEFAULT_CAMERA_HEIGHT),
    gl_initialised(false),
    _R_background(0.0), 
    _G_background(0.0), 
    _B_background(0.0), 
    _alpha_background(0.0)
{
  Vector3D x(0.0, 0.0, 10.0);
  Quaternion q(0.0, 0.0, 0.0, 
             1.0);

  camera_position = x;
  camera_orientation = q;

  rotation_centre = new RotationCentre(current_drawing_position);

  // Set default camera parameters (perspective)
  SetCameraSize(camera_width, camera_height);
}

ThreeDToolGLCommon::ThreeDToolGLCommon (const unsigned int w, 
              const unsigned int h) 
  : texture_interpolation(GL_NEAREST), 
    camera_width(w), camera_height(h), 
    gl_initialised(false),
    _R_background(0.0), 
    _G_background(0.0), 
    _B_background(0.0), 
    _alpha_background(0.0)
{
  Vector3D x(0.0, 0.0, 10.0);
  Quaternion q(0.0, 0.0, 0.0, 
             1.0);

  camera_position = x;
  camera_orientation = q;

  rotation_centre = new RotationCentre(current_drawing_position);

  // Set default camera parameters (perspective)
  SetCameraSize(camera_width, camera_height);
}

// }}}

// {{{ ThreeDToolGLCommon::~ThreeDToolGLCommon()

ThreeDToolGLCommon::~ThreeDToolGLCommon()
{
    if (gl_initialised) {
	gluDeleteQuadric(sphereptr);
	gluDeleteQuadric(cylinderptr);
	gluDeleteQuadric(circleptr);
    }
}

// }}}


void ThreeDToolGLCommon::InitGL()
{
  // Initialise some GL stuff
  MakeCurrent();

  // Set up light and model surface properties
  GLfloat mat_ambient[] = {0.5, 0.5, 0.5, 1.0 };
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat mat_shininess[] = { 50.0 };
  GLfloat light_position[] = { 1.0, 100.0, 10.0, 0.0 };
  GLfloat model_ambient[] = {0.5, 0.5, 0.5, 1.0};

  glClearColor(_R_background, _G_background, _B_background, _alpha_background);
   
  // The material that the scene objects are made out of
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  // The light model
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

  // Activate some GL stuff
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);

  currently_drawing_in_selection_mode_flag = false;
  click_x0 = -1; click_y0 = -1;

  // Allocate sphere object which we will hold onto until the end
  sphereptr = gluNewQuadric();
  gluQuadricDrawStyle(sphereptr, GLU_FILL);
  gluQuadricNormals(sphereptr, GLU_SMOOTH);

  // Allocate cylinder object which we will hold onto until the end
  cylinderptr = gluNewQuadric();
  gluQuadricDrawStyle(cylinderptr, GLU_FILL);
  gluQuadricNormals(cylinderptr, GLU_SMOOTH);

  // Allocate circle object which we will hold onto until the end
  circleptr = gluNewQuadric();
  gluQuadricDrawStyle(circleptr, GLU_FILL);
  gluQuadricNormals(circleptr, GLU_NONE);

  // Reset the texture stuff
  texName = 0;
  texWidth = 0;
  texHeight = 0;

  gl_initialised = true;
}



void ThreeDToolGLCommon::Clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
}


void ThreeDToolGLCommon::DoDrawing()
{
  if (!gl_initialised) InitGL();
  MakeCurrent();

  glClearColor(_R_background, _G_background, _B_background, _alpha_background);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (orthographic_projection_flag == false) {
    // Perspective projection
    glFrustum(move_clipping_plane_factor * xL, 
	      move_clipping_plane_factor * xR, 
	      move_clipping_plane_factor * yB, 
	      move_clipping_plane_factor * yT, 
	      move_clipping_plane_factor * zN, 
	      zF);
  } 
  else {
    // Orthographic projection
    glOrtho(xL, xR, yB, yT, move_clipping_plane_factor * zN, zF);
  } 

  // Make sure that we show a window of the right aspect ratio (defined
  // by camera_width and camera_height)
  // Hopefully the widget is within an AspectBox which ensures this is
  // always true
  // Otherwise use glViewport to only draw only in central part with 
  // the correct aspect ratio of the window we have
  double current_window_aspect_ratio = double(WindowWidth()) / 
    double(WindowHeight());
  double camera_aspect_ratio = double(camera_width) / double(camera_height);
  if (  current_window_aspect_ratio > camera_aspect_ratio) {
    // We are in a wide window: set the width we display depending on 
    // the height of the window
    int local_width = int (camera_aspect_ratio * WindowHeight() + 0.5);
    int difference = WindowWidth() - local_width; 
    glViewport(difference / 2, 0, local_width, WindowHeight());
    if (VERBOSE) cerr << "Setting viewport size " << local_width << ", " 
		      << WindowHeight() << endl;
  }
  else {
    // Tall (or equal) window: set height we display depending on window width
    int local_height = int (WindowWidth() / camera_aspect_ratio + 0.5);
    int difference = WindowHeight() - local_height; 
    glViewport(0, difference / 2, WindowWidth(), local_height);
    if (VERBOSE) cerr << "Setting viewport size " << WindowWidth() << ", " 
		      << local_height << endl;
  }

  // Take care of the position of our virtual camera
  // camera_position is vector cW
  // camera_orientation is quaternion qWC
  // Apply them negatively to transform from camera frame to world frame
  Quaternion qCW = camera_orientation.Inverse();
  Vector3D tCW = -camera_position;
  if (VERBOSE) {
    cerr << "In Draw(): rCC " 
	 << qCW.RotationMatrix() *
      (camera_position) << endl;
  }
  

  
  double ang = qCW.Angle();
  if (ang != 0.0) {
    Vector3D axis(qCW.Axis());
    if (VERBOSE) cerr << "Angle: " << ang << " axis " << axis << endl;
    glRotatef(ang * 180.0 / M_PI, axis.GetX(), axis.GetY(), axis.GetZ());
  }

  glTranslatef(tCW.GetX(), tCW.GetY(), tCW.GetZ());

  if (VERBOSE) {
    cerr << "Camera position: " << camera_position << endl
	 << "Camera orientation: " << camera_orientation << endl
	 << "qCW: " << qCW << endl;
  }

  // Now on to the ModelView matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();			


  // Reset drawing position
  current_drawing_position.Zero();
  drawing_position_stack.clear();


  rotation_centre->Reset();


  // Now call the specific drawing function of user program
  _CallDrawEvent(false);

  if(dragging == true) {
    SetPenColour(255, 255, 255, 0);
    DrawRotationCentre();
  }


  SwapBuffers();


  if (VERBOSE) cerr << "GL error status: " 
		    << gluErrorString(glGetError()) << endl;

}


void ThreeDToolGLCommon::SetCameraSize(unsigned int w, unsigned int h)
{
  camera_width = w;
  camera_height = h;
  SetCameraParameters(camera_width / DEFAULT_WIDTH_OVER_FKU, 
          camera_width / DEFAULT_WIDTH_OVER_FKU,
          camera_width / 2.0, camera_height / 2.0); 
}

void ThreeDToolGLCommon::
SetCameraPositionOrientation(const Vector3D& new_position,
			     const Quaternion& new_orientation)
{
  camera_position = new_position;
  camera_orientation = new_orientation;
}


void ThreeDToolGLCommon::LookAt(const Point3D& some_point)
{
  // If C = Camera position
  //    P = Desired point to look at
  //    U = Desired "up" direction
  //
  // R = [ S V -L ]
  //
  //   where L = P - C (normalised)
  //         S = L x U (normalised)
  //         V = S x L


  /**** Should calculate up direction from current up direction, but seems to be unstable ***/
  // const VW::Vector3D U = camera_orientation.Conjugate().Rotate(VW::Vector3D(0,1,0));
  const VW::Vector3D U = VW::Vector3D(0,1,0);

  VW::Vector3D L = (some_point - camera_position); L.Normalise();
  const VW::Vector3D S = VW::VectorProduct(L,U);
  const VW::Vector3D V = VW::VectorProduct(S,L);

  VW::RotationMatrix R;
  R[0][0] = S.GetX();   R[0][1] = V.GetX();   R[0][2] = -L.GetX(); 
  R[1][0] = S.GetY();   R[1][1] = V.GetY();   R[1][2] = -L.GetY(); 
  R[2][0] = S.GetZ();   R[2][1] = V.GetZ();   R[2][2] = -L.GetZ(); 

  camera_orientation.Set(R);

}


void ThreeDToolGLCommon::
GetCameraPositionOrientation(Vector3D& current_position,
			     Quaternion& current_orientation)
{
  current_position = camera_position;
  current_orientation = camera_orientation;
}



void ThreeDToolGLCommon::SetCameraParameters(const double fku_, 
					     const double fkv_,
					     const double u0_, 
					     const double v0_)
{
  orthographic_projection_flag = false;
  
  fku = fku_;
  fkv = fkv_;
  u0 = u0_;
  v0 = v0_;

  // Calculate GL versions of these (see explanation in header file)
  xW = camera_width * zN / fku;
  yH = camera_height * zN / fkv;
  xM = (zN / fku) * (camera_width / 2.0 - u0);
  yM = - (zN / fkv) * (camera_height / 2.0 - v0);
  
  xL = xM - 0.5 * xW;
  xR = xM + 0.5 * xW;
  yB = yM - 0.5 * yH;
  yT = yM + 0.5 * yH;

  if (VERBOSE) {
    cerr << "xW yH xM yM " 
	 << xW << " " << yH << " " << xM << " " << yM << endl;
    cerr << "xL xR yB yT " 
	 << xL << " " << xR << " " << yB << " " << yT << endl;
  }
}


void ThreeDToolGLCommon::SetCameraParameters(const VW::CalibrationMatrix& normK)
{
  const double scale = 0.5*camera_width;
  const double xoffset = 0.5*camera_width;
  const double yoffset = 0.5*camera_height;
  double f = scale * normK.GetFocalLength();
  SetCameraParameters(f, normK.GetAspectRatio()*f,
		      xoffset + scale * normK.GetPrincipalPointX(),
		      yoffset + scale * normK.GetPrincipalPointY());
}

void ThreeDToolGLCommon::SetOrthographicCameraParameters(const double xWidth, 
							 const double yHeight,
							 const double u0_, 
							 const double v0_)
{
  orthographic_projection_flag = true;

  u0 = u0_;
  v0 = v0_;

  // Calculate GL versions of these (see explanation in header file)
  xW = xWidth;
  yH = yHeight;
  xM = (xW / camera_width) * (camera_width / 2.0 - u0);
  yM = - (yH / camera_height) * (camera_height / 2.0 - v0);
  
  xL = xM - 0.5 * xW;
  xR = xM + 0.5 * xW;
  yB = yM - 0.5 * yH;
  yT = yM + 0.5 * yH;

  if (VERBOSE) {
    cerr << "xW yH xM yM " 
	 << xW << " " << yH << " " << xM << " " << yM << endl;
    cerr << "xL xR yB yT " 
	 << xL << " " << xR << " " << yB << " " << yT << endl;
  }
}



// {{{ void ThreeDToolGLCommon::Dragged(int x0, int y0, int x1, int y1, int button)

void ThreeDToolGLCommon::Dragged(const int x0, const int y0, 
				    const int x1, const int y1, 
				    const int button)
{
  if (VERBOSE) cerr << "Dragged from " << x0 << ", " << y0
		    << " to " << x1 << ", " << y1 << endl;
  double this_x=2.0*double(x1-x0)/camera_width;
  double this_y=2.0*double(y0-y1)/camera_height;
  if (VERBOSE) cerr << "this_x = " << this_x << ", this_y = " << this_y 
		    << endl;

  dragging=true;

  switch(button) {
  case 1:
    // Left button: trackball rotation
    {
      // For current frame drag: beginx, beginy are the start point in image
      // coordinates not GL 0,0 in top left, x, y are end point
      // area.width, area.height are area size in pixels
      // So args are all in range -1 to 1 image position 0,0 is centre
      // -1,1 is bottom left, 1, 1 is top right
      Quaternion qCprimeC = TrackBall(
			     	 (2.0 * x0 - WindowWidth()) / WindowWidth(),
			      	 (WindowHeight() - 2.0 * y0) / WindowHeight(),
			       	 (2.0 * x1 - WindowWidth()) / WindowWidth(),
			       	 (WindowHeight() - 2.0 * y1) / WindowHeight()
					 );

      // Check:
      if (VERBOSE) {
	cerr << "Before transform: rCC " 
	     << camera_orientation.Inverse().RotationMatrix() *
	  (-camera_position) << endl;
      }
      
      if (VERBOSE) cerr << "qCprimeC " << qCprimeC << endl;
    
      // Need to change camera_position first
      Quaternion qCCprime = qCprimeC.Inverse();
      
      if (VERBOSE) cerr << "qCCprime " << qCCprime << endl;
      
      Quaternion qWCprime = camera_orientation * qCCprime;
      
      Quaternion qCW = camera_orientation.Inverse();
      Quaternion qtrans = qWCprime * qCW;
      
      Vector3D bminuscW = 
	rotation_centre->Centre() - camera_position;
      
      Vector3D new_camera_position = 
	rotation_centre->Centre() -
	qtrans.RotationMatrix() * 
	(rotation_centre->Centre() - camera_position);
      
      // And now camera orientation
      Quaternion new_camera_orientation = 
	camera_orientation * qCCprime;
      
      camera_position = new_camera_position;
      camera_orientation = new_camera_orientation;
      
      if (VERBOSE) {
	cerr << "After transform: rCC " 
	     << camera_orientation.Inverse().RotationMatrix() *
	  (-camera_position) << endl;
      }

    }
    break;
  case 2:
    // Middle button: xy drag
    {
      // Set "gain": amount we translate should be such that image drag
      // equates to equivalent motion
      // Motion proportional to distance from centre of rotation
      
      // In coordinates of camera image 
      double udrag = double(x0 - x1) * double(camera_width) / 
	double(WindowWidth());
      double vdrag = double(y1 - y0) * double(camera_height) / 
	double(WindowHeight());

      double txC, tyC;
      if (orthographic_projection_flag == false) {
	// Perspective projection

	// Vector from camera to rotation centre in camera frame
	// RCW * (-cW + rotation_centreW)
	Vector3D cbC = 
	    camera_orientation.Inverse().RotationMatrix() 
	    * (rotation_centre->Centre() - camera_position);

     
	// This translation in camera frame
	txC = (udrag / fku) * cbC.GetZ();
	tyC = (vdrag / fkv) * cbC.GetZ();
      }
      else {
	// Orthographic projection
	
	// Simpler... 
	txC = -udrag * xW / camera_width;
	tyC = -vdrag * yH / camera_height;
      }

      Vector3D tC(txC, tyC, 0.0);
      Vector3D new_camera_position =
	camera_position -
	camera_orientation.RotationMatrix() * tC;
      camera_position = new_camera_position;
    }
    break;
  case 3:
    // Right button: z drag
    {
      // In coordinates of camera image 
      double vdrag = double(y1 - y0) * double(camera_height) / 
	double(WindowHeight());

      // Here we do something very different in the orthographic and 
      // perspective cases: for perspective we move the camera (zdrag)
      // But for orthographic we have to change the projection
      if (orthographic_projection_flag == false) {
	// Perspective projection

	// Set "gain": amount we translate should be such that image drag
	// equates to equivalent motion in y direction...?
	// Motion proportional to distance from centre of rotation
      
	// Vector from camera to rotation centre in camera frame
	// RCW * (-cW + rotation_centreW)
	Vector3D cbC = 
	  camera_orientation.Inverse().RotationMatrix() 
	  * (rotation_centre->Centre() - camera_position);

     
	// This translation in camera frame
	double tzC = (vdrag / fkv) * cbC.GetZ();

	Vector3D tC(0.0, 0.0, tzC);
	Vector3D new_camera_position =
	  camera_position -
	  camera_orientation.RotationMatrix() * tC;
	camera_position = new_camera_position;
      }
      else {
	// Orthographic Projection

	// Use the drag to change the size of the viewing volume
	double metredrag = vdrag * xW / camera_width;

	xW += metredrag;
	yH += metredrag * camera_height / camera_width;

	SetOrthographicCameraParameters(xW, yH, u0, v0);
      }
    }
    break;
  default:
    if (VERBOSE) cerr << "Button > 3?" << endl;
  }

  RequestDraw();
}

// }}}

// {{{ void ThreeDToolGLCommon::DragRelease()

void ThreeDToolGLCommon::DragRelease() {
  dragging = false;
  RequestDraw();
}

// }}}

// {{{ void ThreeDToolGLCommon::Clicked()

void ThreeDToolGLCommon::Clicked(const int x0, const int y0, 
				    const int button) {
  if (VERBOSE) cerr << "Click at " << x0 << ", " << y0 << ", button " 
		    << button << endl;

  // Save click position: needed by Set2DDrawingPosition if 2D objects
  // are drawn
  click_x0 = x0; 
  click_y0 = y0;

  const int SELECTBUFSIZE = 512;

  // Work out which object was clicked on
  GLuint select_buffer[SELECTBUFSIZE];
  GLint hits;
  glSelectBuffer(SELECTBUFSIZE, select_buffer);
  glRenderMode(GL_SELECT);

  glInitNames();
  glPushName(0);

  glMatrixMode(GL_PROJECTION);
  //  glPushMatrix();
  glLoadIdentity();
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  gluPickMatrix((GLdouble) x0, (GLdouble) (WindowHeight() - y0),
		7.0, 7.0, viewport);

  // Set up same viewing conditions as normal
  if (orthographic_projection_flag == false) {
    // Perspective projection
    glFrustum(move_clipping_plane_factor * xL, 
	      move_clipping_plane_factor * xR, 
	      move_clipping_plane_factor * yB, 
	      move_clipping_plane_factor * yT, 
	      move_clipping_plane_factor * zN, 
	      zF);
  } 
  else {
    // Orthographic projection
    glOrtho(xL, xR, yB, yT, move_clipping_plane_factor * zN, zF);
  } 

  // Take care of the position of our virtual camera
  // camera_position is vector cW
  // camera_orientation is quaternion qWC
  // Apply them negatively to transform from camera frame to world frame
  Quaternion qCW = camera_orientation.Inverse();
  Vector3D tCW = -camera_position;
  if (VERBOSE) {
    cerr << "In Draw(): rCC " 
	 << qCW.RotationMatrix() *
      (camera_position) << endl;
  }
  
  double ang = qCW.Angle();
  if (ang != 0.0) {
    Vector3D axis(qCW.Axis());
    if (VERBOSE) cerr << "Angle: " << ang << " axis " << axis << endl;
    glRotatef(ang * 180.0 / M_PI, axis.GetX(), axis.GetY(), axis.GetZ());
  }

  glTranslatef(tCW.GetX(), tCW.GetY(), tCW.GetZ());

  // Now on to the ModelView matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();			


  // Reset drawing position
  current_drawing_position.Zero();
  drawing_position_stack.clear();


  rotation_centre->Reset();


  currently_drawing_in_selection_mode_flag = true;

  // Call the external drawing function in selection mode
  DrawEvent(true);

  currently_drawing_in_selection_mode_flag = false;

  // Process any hits we've got
  hits = glRenderMode(GL_RENDER);

  if (VERBOSE) cerr << "Hits = " << hits  << endl;
  GLuint *ptr = select_buffer;

  // Find if we have selected a real item; if more than one then choose
  // the closest
  int selected_item = 0;
  double closest_distance = 10000000000.0;
    // Structure of select_buffer:
    // For each hit this ints:
    // 1. number of names (always 1?)
    // 2. z1
    // 3. z2 (? what is the difference?)
    // 4. (or more) the names
  for (int i = 0; i < hits; i++) {	/*  for each hit  */
    GLuint number_of_names = *ptr++;
    // Skip past z1...
    ptr++;
    // double z1 = double(*ptr++ / 0x7fffffff);
    double z2 = double(*ptr++ / 0x7fffffff);
    GLuint name = 0;
    for (int j = 0; j < int(number_of_names); j++) {	/*  for each name */
      // Don't know why there would be more than one name? 
      // Just take the last one
      name = *ptr++;
    }

    if (name != 0) {
      // zero name isn't a real object
      if (z2 < closest_distance) {
	selected_item = name;
	closest_distance = z2;
      }
    }
  }

  if (selected_item != 0) {
    if (VERBOSE) cerr << "Selected item with name " << selected_item << endl;
  }
  else {
    if (VERBOSE) cerr << "No valid item selected." << endl;
  }

  // Also want to export the position of the click in camera coordinates
  // (useful if this display is being used for augmented reality with 
  // a CinemaScreen image)
  double camera_x, camera_y;

  double current_window_aspect_ratio = double(WindowWidth()) / 
    double(WindowHeight());
  double camera_aspect_ratio = double(camera_width) / double(camera_height);
  if (  current_window_aspect_ratio > camera_aspect_ratio) {
    // We are in a wide window: set the width we display depending on 
    // the height of the window
    double local_width = camera_aspect_ratio * WindowHeight();
    double difference = WindowWidth() - local_width; 

    camera_x = (x0 - difference / 2.0) * 
		    double(camera_width) / local_width;
    camera_y = (y0) * 
		    double(camera_height) / double(WindowHeight());
  }
  else {
    // Tall (or equal) window: set height we display depending on window width
    double local_height = WindowWidth() / camera_aspect_ratio;
    double difference = WindowHeight() - local_height; 

    camera_x = int ((x0) * 
		    double(camera_width) / double(WindowWidth()) + 0.5);
    camera_y = int ((y0 - difference / 2.0) * 
		    double(camera_height) / local_height + 0.5);
  }

  // And call our external function to do something with this:
  // but only if the click is within the proper camera window
  if (camera_x >= 0.0 && camera_x < camera_width &&
      camera_y >= 0.0 && camera_y < camera_height)
    _CallProcessHitsEvent((int) selected_item, camera_x, camera_y);
}

// }}}

// {{{ void ThreeDToolGLCommon::SetPenColour()

void ThreeDToolGLCommon::SetPenColour(const unsigned char red, 
           const unsigned char green, 
      const unsigned char blue, const unsigned char alpha)
{
  glColor4f(double(red)/255, double(green)/255, 
      double(blue)/255, double(alpha)/255);
}

void ThreeDToolGLCommon::GetPenColour(
  unsigned char& red, unsigned char& green,
  unsigned char& blue, unsigned char& alpha)
{
  GLfloat fcol[4];
  glGetFloatv(GL_CURRENT_COLOR, fcol);
  red = static_cast<unsigned char>(255 * fcol[0]);
  green = static_cast<unsigned char>(255 * fcol[1]);
  blue = static_cast<unsigned char>(255 * fcol[2]);
  alpha = static_cast<unsigned char>(255 * fcol[3]);
}
// }}}


// {{{ void ThreeDToolGLCommon::DrawAxes()

void ThreeDToolGLCommon::DrawAxes(bool number_axes)
{
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);
  //  SetPenColour(255,255,255,0);

  Vector3D r0,r1;

  // Where to draw the "origin" of our axes? At 0, 0, 0 if our model
  // data spans that point
  // But otherwise we're prepared to shift it
  double origin_x = 0.0;
  double origin_y = 0.0;
  double origin_z = 0.0;
#if 0 
  //  Actually I think it's better not to shift it
  if (rotation_centre->XMin() > 0.0 || rotation_centre->XMax() < 0.0)
    origin_x = rotation_centre->Centre().GetX();
  if (rotation_centre->YMin() > 0.0 || rotation_centre->YMax() < 0.0)
    origin_y = rotation_centre->Centre().GetY();
  if (rotation_centre->ZMin() > 0.0 || rotation_centre->ZMax() < 0.0)
    origin_z = rotation_centre->Centre().GetZ();
#endif  

  // Move to origin
  glLoadIdentity();

  // X axis
  r0.Set(rotation_centre->XMin() , origin_y , origin_z);
  r1.Set(rotation_centre->XMax() , origin_y , origin_z);
  DrawLine(r0, r1);
  
  // Y axis
  r0.Set(origin_x , rotation_centre->YMin() , origin_z);
  r1.Set(origin_x , rotation_centre->YMax() , origin_z);
  DrawLine(r0, r1);
  
  // Z axis
  r0.Set(origin_x , origin_y , rotation_centre->ZMin());
  r1.Set(origin_x , origin_y , rotation_centre->ZMax());
  DrawLine(r0, r1);
  
  // Draw axes labels
  glRasterPos3f(rotation_centre->XMax(), origin_y, origin_z);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'X');

  glRasterPos3f(origin_x, rotation_centre->YMax(), origin_z);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Y');

  glRasterPos3f(origin_x, origin_y, rotation_centre->ZMax());
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Z');


  // Draw axes scale
  // Work out a suitable scale: we want to use the same scale for 
  // all the axes so add them all up to start with
  double axes_range = rotation_centre->XMax() - rotation_centre->XMin()
    + rotation_centre->YMax() - rotation_centre->YMin()
    + rotation_centre->ZMax() - rotation_centre->ZMin();

  double log_axes_range = log10(axes_range);

  int power = int (log_axes_range);
  double unit = pow(10.0, power - 1);

  if(number_axes)
  {  
    // Origin
    glRasterPos3f(origin_x, origin_y, origin_z);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '0');
  
    // X axis
    // Positive
    for (double x = origin_x + unit; x < rotation_centre->XMax(); x += unit) {
      glRasterPos3f(x, origin_y, origin_z);
      DrawNumberHere(x);
    }
    // Negative
    {
    for (double x = origin_x - unit; x > rotation_centre->XMin(); x -= unit) {
      glRasterPos3f(x, origin_y, origin_z);
      DrawNumberHere(x);
    }
    }
    // Y axis
    // Positive
    {
      for (double y = origin_y + unit; y < rotation_centre->YMax(); y += unit) {
      glRasterPos3f(origin_x, y, origin_z);
      DrawNumberHere(y);
    }
    }
    // Negative
    for (double y = origin_y - unit; y > rotation_centre->YMin(); y -= unit) {
      glRasterPos3f(origin_x, y, origin_z);
      DrawNumberHere(y);
    }
    // Z axis
    // Positive
    for (double z = origin_z + unit; z < rotation_centre->ZMax(); z += unit) {
      glRasterPos3f(origin_x, origin_y, z);
      DrawNumberHere(z);
    }
    // Negative
    {
    for (double z = origin_z - unit; z > rotation_centre->ZMin(); z -= unit) {
      glRasterPos3f(origin_x, origin_y, z);
      DrawNumberHere(z);
    }
    }
  }

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

}

// }}}


// Little utility function
// Draw a number at current raster position
// {{{ void ThreeDToolGLCommon::DrawNumberHere(double number)

void ThreeDToolGLCommon::DrawNumberHere(const double number)
{
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);

  char number_label[100];
  char *digit_pointer;

  sprintf(number_label, "%g", number);
  digit_pointer = number_label;
  for (unsigned int i = 0; i < strlen(number_label); i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *(digit_pointer++));  
  }
}

// }}}

// Point at r0
// {{{ void ThreeDToolGLCommon::DrawPoint()

void ThreeDToolGLCommon::DrawPoint(const Vector3D& r0)
{
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);

  glPointSize(4);

  glBegin(GL_POINTS); {
    glVertex3d(r0.GetX(), r0.GetY(), r0.GetZ());
  };glEnd();

  rotation_centre->Include(r0);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

}

// }}}

// Selection version of DrawPoint
// {{{ void ThreeDToolGLCommon::DrawPoint()

void ThreeDToolGLCommon::DrawPoint(const Vector3D& r0,
				      const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  DrawPoint(r0);
  glLoadName(0);    
}

// }}}



// Straight Line between r0 and r1
// {{{ void ThreeDToolGLCommon::DrawLine()

void ThreeDToolGLCommon::DrawLine(const Vector3D& r0, 
				     const Vector3D& r1)
{
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);

  glBegin(GL_LINE_STRIP); {
    glVertex3d(r0.GetX(), r0.GetY(), r0.GetZ());
    glVertex3d(r1.GetX(), r1.GetY(), r1.GetZ());
  }; glEnd();

  rotation_centre->Include(r0);
  rotation_centre->Include(r1);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

// }}}

// {{{ void ThreeDToolGLCommon::DrawLine()

void ThreeDToolGLCommon::DrawLine(const Vector3D& r0, 
				     const Vector3D& r1, 
				     const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  DrawLine(r0, r1);
  glLoadName(0);
}

// }}}

void ThreeDToolGLCommon::DrawTriangle(
  const Vector3D& p1, const Vector3D& p2, const Vector3D& p3) const
{
  glBegin(GL_TRIANGLES);
  glVertex3f(p1[0], p1[1], p1[2]);
  glVertex3f(p2[0], p2[1], p2[2]);
  glVertex3f(p3[0], p3[1], p3[2]);
  glEnd();

  rotation_centre->Include(p1);
  rotation_centre->Include(p2);
  rotation_centre->Include(p3);
}

void ThreeDToolGLCommon::DrawTriangle(
  const Vector3D& p1, const Vector3D& p2, const Vector3D& p3,
  int name) const
{
  if (currently_drawing_in_selection_mode_flag)
  {
    glLoadName(name);
  }
  DrawTriangle(p1, p2, p3);
  glLoadName(0);
}


// Draw a rectangle
// We specify the location vector rW of the centre of the rectangle in
// the world frame
// And the quaternion qWO which specifies the transformation between the
// world frame and the object frame in which the rectangle is aligned:
// In the object frame, the height of the rectangle is aligned with the 
// y axis; the width with the left axis; and the rectangle is perpendicular
// to the z axis
// {{{ void ThreeDToolGLCommon::DrawRectangle()

void ThreeDToolGLCommon::DrawRectangle(const Vector3D& rW, 
					  const Quaternion& qWO,
					  const double rectangle_width,
					  const double rectangle_height)
{
  // Save the current drawing matrix 
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // Transform into Object frame
  glTranslatef(rW.GetX(), rW.GetY(), rW.GetZ());
  double ang = qWO.Angle();
  if (ang != 0.0) {
    Vector3D axis(qWO.Axis());
    glRotatef(ang * 180.0 / M_PI, axis.GetX(), axis.GetY(), axis.GetZ());
  }

  // And draw the rectangle
  glBegin(GL_QUADS); {
    glVertex3f(-rectangle_width / 2.0, -rectangle_height / 2.0, 0.0);
    glVertex3f(-rectangle_width / 2.0,  rectangle_height / 2.0, 0.0);
    glVertex3f( rectangle_width / 2.0,  rectangle_height / 2.0, 0.0);
    glVertex3f( rectangle_width / 2.0, -rectangle_height / 2.0, 0.0);
  }; glEnd();


  // Slightly crude 
  rotation_centre->Include(rW);

  // Pop old drawing matrix back up
  glPopMatrix();
}

// }}}

// {{{ void ThreeDToolGLCommon::DrawRectangle()

void ThreeDToolGLCommon::DrawRectangle(const Vector3D& rW, 
					  const Quaternion& qWO,
					  const double rectangle_width,
					  const double rectangle_height,
					  const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  DrawRectangle(rW, qWO, rectangle_width, rectangle_height);
  glLoadName(0);    
}

// }}}

// A protected function for internal use only
void ThreeDToolGLCommon::
ActuallyDrawTexturedRectangle(const Vector3D rW, 
		      const Quaternion qWO,
		      const double rectangle_width,
		      const double rectangle_height,
		      const VW::ImageMono<unsigned char> &imageref)
{
  glDisable(GL_COLOR_MATERIAL);
  glShadeModel(GL_FLAT);

  if ((texWidth<imageref.GetWidth())
      || (texHeight<imageref.GetHeight())) {
      // Current texture is too small
      texWidth = GetNextPowerOf2(imageref.GetWidth());
      texHeight = GetNextPowerOf2(imageref.GetHeight());
      if (texName > 0) {
	  // Texture already exists, so reallocate
	  glDeleteTextures(1, &texName);
      }
      glGenTextures(1, &texName);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      glBindTexture(GL_TEXTURE_2D, texName);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_interpolation);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_interpolation);
      
      GLubyte *dummy_texture = new GLubyte [texWidth * texHeight];
  
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 
		   0, GL_LUMINANCE, GL_UNSIGNED_BYTE, dummy_texture);
      delete dummy_texture;

  } else {
      // Texture size is OK, so just bind to it
      glBindTexture(GL_TEXTURE_2D, texName);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_interpolation);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_interpolation);
  }
  if (VERBOSE) cerr << "Texture width " << texWidth << " height " << 
		 texHeight << endl;


  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		  imageref.GetWidth(), 
		  imageref.GetHeight(), 
		  GL_LUMINANCE,
		  GL_UNSIGNED_BYTE, 
		  imageref.GetRawBuffer());

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  // Save the current drawing matrix 
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
   

  // Transform into Object frame
  glTranslatef(rW.GetX(), rW.GetY(), rW.GetZ());
  double ang = qWO.Angle();
  if (ang != 0.0) {
    Vector3D axis(qWO.Axis());
    glRotatef(ang * 180.0 / M_PI, axis.GetX(), axis.GetY(), axis.GetZ());
  }

  double useful_width = double(imageref.GetWidth()) /
                       double(texWidth);
  double useful_height = double(imageref.GetHeight()) /
                        double(texHeight);
  if (VERBOSE) cerr << "useful_width " << useful_width << " useful_height "
		    << useful_height << endl;

  glRotatef(180.0, 1.0, 0.0, 0.0);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);   
  glVertex3f(-rectangle_width / 2.0,  -rectangle_height / 2.0, 0.0);
  glTexCoord2f(0.0,          useful_height); 
  glVertex3f(-rectangle_width / 2.0, rectangle_height / 2.0, 0.0);
  glTexCoord2f(useful_width, useful_height); 
  glVertex3f( rectangle_width / 2.0, rectangle_height / 2.0, 0.0);
  glTexCoord2f(useful_width, 0.0);           
  glVertex3f( rectangle_width / 2.0,  -rectangle_height / 2.0, 0.0);
  glEnd();

  glPopMatrix();

  glDisable(GL_TEXTURE_2D);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
}


// A protected function for internal use only
void ThreeDToolGLCommon::
ActuallyDrawWarpedTexturedRectangle(const Vector3D rW, 
		      const Quaternion qWO,
		      double corner00[2], double corner01[2], 
		      double corner10[2], double corner11[2], 
		      const VW::ImageMono<unsigned char> &imageref)
{
  glDisable(GL_COLOR_MATERIAL);
  glShadeModel(GL_FLAT);

  if ((texWidth<imageref.GetWidth())
      || (texHeight<imageref.GetHeight())) {
      // Current texture is too small
      texWidth = GetNextPowerOf2(imageref.GetWidth());
      texHeight = GetNextPowerOf2(imageref.GetHeight());
      if (texName > 0) {
	  // Texture already exists, so reallocate
	  glDeleteTextures(1, &texName);
      }
      glGenTextures(1, &texName);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      glBindTexture(GL_TEXTURE_2D, texName);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_interpolation);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_interpolation);
      
      GLubyte *dummy_texture = new GLubyte [texWidth * texHeight];
  
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 
		   0, GL_LUMINANCE, GL_UNSIGNED_BYTE, dummy_texture);
      delete dummy_texture;

  } else {
      // Texture size is OK, so just bind to it
      glBindTexture(GL_TEXTURE_2D, texName);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_interpolation);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_interpolation);
  }
  if (VERBOSE) cerr << "Texture width " << texWidth << " height " << 
		 texHeight << endl;


  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		  imageref.GetWidth(), 
		  imageref.GetHeight(), 
		  GL_LUMINANCE,
		  GL_UNSIGNED_BYTE, 
		  imageref.GetRawBuffer());

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  // Save the current drawing matrix 
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
   

  // Transform into Object frame
  glTranslatef(rW.GetX(), rW.GetY(), rW.GetZ());
  double ang = qWO.Angle();
  if (ang != 0.0) {
    Vector3D axis(qWO.Axis());
    glRotatef(ang * 180.0 / M_PI, axis.GetX(), axis.GetY(), axis.GetZ());
  }

  double useful_width = double(imageref.GetWidth()) /
                       double(texWidth);
  double useful_height = double(imageref.GetHeight()) /
                        double(texHeight);
  if (VERBOSE) cerr << "useful_width " << useful_width << " useful_height "
		    << useful_height << endl;

  glRotatef(180.0, 1.0, 0.0, 0.0);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);   
  glVertex3f(corner00[0], corner00[1], 0.0);
  glTexCoord2f(0.0,          useful_height); 
  glVertex3f(corner10[0], corner10[1], 0.0);
  glTexCoord2f(useful_width, useful_height); 
  glVertex3f(corner11[0], corner11[1], 0.0);
  glTexCoord2f(useful_width, 0.0);           
  glVertex3f(corner01[0], corner01[1], 0.0);
  glEnd();

  glPopMatrix();

  glDisable(GL_TEXTURE_2D);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);

}


void ThreeDToolGLCommon::
ActuallyDrawDistortedTexturedRectangle(const Vector3D rW, 
				       const Quaternion qWO,
				       const double rectangle_width,
				       const double rectangle_height,
				       const VW::ImageMono<unsigned char> &imageref,
				       double kappa,
				       const VW::Point2D& distortion_centre)
{
  static const unsigned int num_divisions = 10;

  glDisable(GL_COLOR_MATERIAL);
  glShadeModel(GL_FLAT);

  if ((texWidth<imageref.GetWidth())
      || (texHeight<imageref.GetHeight())) {
      // Current texture is too small
      texWidth = GetNextPowerOf2(imageref.GetWidth());
      texHeight = GetNextPowerOf2(imageref.GetHeight());
      if (texName > 0) {
	  // Texture already exists, so reallocate
	  glDeleteTextures(1, &texName);
      }
      glGenTextures(1, &texName);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      glBindTexture(GL_TEXTURE_2D, texName);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_interpolation);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_interpolation);
      
      GLubyte *dummy_texture = new GLubyte [texWidth * texHeight];
  
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 
		   0, GL_LUMINANCE, GL_UNSIGNED_BYTE, dummy_texture);
      delete dummy_texture;

  } else {
      // Texture size is OK, so just bind to it
      glBindTexture(GL_TEXTURE_2D, texName);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_interpolation);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_interpolation);
  }
  if (VERBOSE) cerr << "Texture width " << texWidth << " height " << 
		 texHeight << endl;


  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		  imageref.GetWidth(), 
		  imageref.GetHeight(), 
		  GL_LUMINANCE,
		  GL_UNSIGNED_BYTE, 
		  imageref.GetRawBuffer());

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  // Save the current drawing matrix 
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
   

  // Transform into Object frame
  glTranslatef(rW.GetX(), rW.GetY(), rW.GetZ());
  double ang = qWO.Angle();
  if (ang != 0.0) {
    Vector3D axis(qWO.Axis());
    glRotatef(ang * 180.0 / M_PI, axis.GetX(), axis.GetY(), axis.GetZ());
  }

  float useful_width = float(imageref.GetWidth()) /
                       float(texWidth);
  float useful_height = float(imageref.GetHeight()) /
                        float(texHeight);
  if (VERBOSE) cerr << "useful_width " << useful_width << " useful_height "
		    << useful_height << endl;

  // Constants for stepping through texture
  const float tex_step_x = useful_width/float(num_divisions);
  const float tex_step_y = useful_height/float(num_divisions);
  const float im_step_x = imageref.GetWidth()/float(num_divisions);
  const float im_step_y = imageref.GetHeight()/float(num_divisions);

  // Build an array of new locations
  VW::Point2D new_pos[num_divisions+1][num_divisions+1];

  for (unsigned int r=0;r<(num_divisions+1);r++) {
    const float y = r*im_step_y;
    for (unsigned int c=0;c<(num_divisions+1);c++) {
      const float x = c*im_step_x;
      new_pos[r][c] = VW::RadialDistortion::UndistortPoint(kappa,
							   VW::Point2D(x,y),
							   distortion_centre);
      double norm_x = rectangle_width*((new_pos[r][c].GetX() / imageref.GetWidth()) - 0.5);
      double norm_y = rectangle_height*((new_pos[r][c].GetY() / imageref.GetHeight()) - 0.5);
      new_pos[r][c].Set(norm_x,norm_y);
    }
  }


  glRotatef(180.0, 1.0, 0.0, 0.0);
  for (unsigned int r=0;r<num_divisions;r++) {
    for (unsigned int c=0;c<num_divisions;c++) {
      // Work out source coords
      const float tex_x0 = tex_step_x*c;
      const float tex_y0 = tex_step_y*r;
      const float tex_x1 = tex_step_x*(c+1);
      const float tex_y1 = tex_step_y*(r+1);

      glBegin(GL_QUADS); {
	glTexCoord2f(tex_x0, tex_y0);   
	glVertex3f(new_pos[r][c].GetX(), new_pos[r][c].GetY(), 0.0);

	glTexCoord2f(tex_x0, tex_y1); 
	glVertex3f(new_pos[r+1][c].GetX(), new_pos[r+1][c].GetY(), 0.0);

	glTexCoord2f(tex_x1, tex_y1); 
	glVertex3f(new_pos[r+1][c+1].GetX(), new_pos[r+1][c+1].GetY(), 0.0);

	glTexCoord2f(tex_x1, tex_y0);           
	glVertex3f(new_pos[r][c+1].GetX(), new_pos[r][c+1].GetY(), 0.0);
      }; glEnd();
    }
  }

  glPopMatrix();

  glDisable(GL_TEXTURE_2D);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
}

// {{{ void ThreeDToolGLCommon::DrawTexturedRectangle()

void ThreeDToolGLCommon::
DrawTexturedRectangle(const Vector3D& rW, 
		      const Quaternion& qWO,
		      const double rectangle_width,
		      const double rectangle_height,
		      const VW::ImageMono<unsigned char> &imageref)
{
  ActuallyDrawTexturedRectangle(rW, qWO, rectangle_width, rectangle_height,
				imageref);

  // Just use the centre of the rectangle
  rotation_centre->Include(rW);
}

// }}}


// {{{ void ThreeDToolGLCommon::DrawWarpedTexturedRectangle()
void ThreeDToolGLCommon::DrawWarpedTexturedRectangle(const Vector3D& rW, 
		      const Quaternion& qWO,
		      double corner00[2], double corner01[2], 
		      double corner10[2], double corner11[2],
                      const VW::ImageMono<unsigned char> &imageref,
                      const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  ActuallyDrawWarpedTexturedRectangle(rW, qWO, corner00, corner01, corner10, corner11, imageref);
  glLoadName(0);  
  // Just use the centre of the rectangle
  rotation_centre->Include(rW);
}
// }}}


// {{{ void ThreeDToolGLCommon::DrawTexturedRectangle()

void ThreeDToolGLCommon::
DrawTexturedRectangle(const Vector3D& rW, 
		      const Quaternion& qWO,
		      const double rectangle_width,
		      const double rectangle_height,
		      const VW::ImageMono<unsigned char> &imageref,
		      const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  DrawTexturedRectangle(rW, qWO, rectangle_width, rectangle_height, imageref);
  glLoadName(0);    
}

// }}}

// Draw a cinema screen image: texture at correct position, depth zC
// to fill the virtual camera image

void ThreeDToolGLCommon::DrawCinemaScreen(const double zC, 
					  const VW::ImageMono<unsigned char> &imageref)
{
  // Orientation of screen is same as camera orientation
  // Position of screen in world frame: rW = cW + rcW

  // Vector from camera to screen in camera frame
  Vector3D rcC(
			 (double(camera_width) / 2.0 - double(u0)) * zC / fku,
			-(double(camera_height) / 2.0 - double(v0)) * zC / fkv,
			 -zC);

  if (VERBOSE) cerr << "rcC" << rcC << endl;

  Vector3D rW = camera_position + 
    camera_orientation.RotationMatrix() * rcC;

  if (VERBOSE) cerr << "rW" << rW << endl;

  if (VERBOSE) cerr << "Cinema screen size " << double(camera_width) * zC / fku
		    << " * " << double(camera_height) * zC / fkv << endl;

  ActuallyDrawTexturedRectangle(rW, camera_orientation, 
				double(camera_width) * zC / fku,
				double(camera_height) * zC / fkv, imageref);
}
void ThreeDToolGLCommon::DrawCinemaScreen(const double zC, 
					  const VW::ImageMono<unsigned char> &imageref,
					  double kappa,
					  const VW::Point2D& distortion_centre)
{
  // Orientation of screen is same as camera orientation
  // Position of screen in world frame: rW = cW + rcW

  // Vector from camera to screen in camera frame
  Vector3D rcC(
			 (double(camera_width) / 2.0 - double(u0)) * zC / fku,
			-(double(camera_height) / 2.0 - double(v0)) * zC / fkv,
			 -zC);

  if (VERBOSE) cerr << "rcC" << rcC << endl;

  Vector3D rW = camera_position + 
    camera_orientation.RotationMatrix() * rcC;

  if (VERBOSE) cerr << "rW" << rW << endl;

  if (VERBOSE) cerr << "Cinema screen size " << double(camera_width) * zC / fku
		    << " * " << double(camera_height) * zC / fkv << endl;

  if (kappa == 0.0) {
  ActuallyDrawTexturedRectangle(rW, camera_orientation, 
				double(camera_width) * zC / fku,
				double(camera_height) * zC / fkv, imageref);
  } else {
  ActuallyDrawDistortedTexturedRectangle(rW, camera_orientation, 
					 double(camera_width) * zC / fku,
					 double(camera_height) * zC / fkv, 
					 imageref,
					 kappa,
					 distortion_centre);
  }
}



// Sphere at centre r0, radius R
// {{{ void ThreeDToolGLCommon::DrawSphere()

void ThreeDToolGLCommon::DrawSphere(const double R)
{
  if (VERBOSE) cerr << "Sphere at current origin" << endl;

  gluSphere(sphereptr, R, 20, 20);

  // Slightly crude 
  rotation_centre->IncludeCurrent();
}

// }}}

// {{{ void ThreeDToolGLCommon::DrawSphere()

void ThreeDToolGLCommon::DrawSphere(const Vector3D& r0, 
				       const double R)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  if (VERBOSE) cerr << "Sphere centre " << r0 << endl;

  glTranslatef(r0.GetX(), r0.GetY(), r0.GetZ());

  gluSphere(sphereptr, R, 20, 20);

  glPopMatrix();

  // Slightly crude 
  rotation_centre->Include(r0);
}

// }}}

// {{{ void ThreeDToolGLCommon::DrawSphere()

void ThreeDToolGLCommon::DrawSphere(const Vector3D& r0, 
				       const double R, const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  DrawSphere(r0, R);
  glLoadName(0);    
}

// }}}

// Cylinder
// {{{ void ThreeDToolGLCommon::DrawCylinder()

void ThreeDToolGLCommon::
DrawCylinder(const double radius,
	     const double height,
		    const bool drawends)
{
  if (VERBOSE) cerr << "Cylinder at current origin" << endl;

  gluCylinder(cylinderptr, radius, radius, height, 20, 4);

  // Slightly crude 
  rotation_centre->IncludeCurrent();

  if (drawends) {
    glPushMatrix();
    gluDisk(circleptr, 0, radius, 20, 2);
    glTranslatef(0,0,height);
    glRotatef(180,1,0,0); // So that normals point right way!
    gluDisk(circleptr, 0, radius, 20, 2);
    glPopMatrix();
  }
}

// }}}

// {{{ void ThreeDToolGLCommon::DrawCylinder()

void ThreeDToolGLCommon::
DrawCylinder(const Vector3D& base_centre,
	     const Quaternion& orientation,
	     const double radius,
	     const double height,
	     const bool drawends)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  if (VERBOSE) cerr << "Cylinder at " << base_centre << endl;

  // Move to object frame
  glTranslatef(base_centre.GetX(), base_centre.GetY(), base_centre.GetZ());
  double ang = orientation.Angle();
  if (ang != 0.0) {
    Vector3D axis(orientation.Axis());
    glRotatef(ang * 180.0 / M_PI, axis.GetX(), axis.GetY(), axis.GetZ());
  }

  gluCylinder(cylinderptr, radius, radius, height, 20, 4);
  if (drawends) {
    gluDisk(circleptr, 0, radius, 20, 2);
    glTranslatef(0,0,height);
    glRotatef(180,1,0,0); // So that normals point right way!
    gluDisk(circleptr, 0, radius, 20, 2);
  }
  glPopMatrix();

  // Slightly crude 
  rotation_centre->Include(base_centre);
}

// }}}

// {{{ void ThreeDToolGLCommon::DrawCylinder()

void ThreeDToolGLCommon::
DrawCylinder(const Vector3D& base_centre, 
	     const Quaternion& orientation,
	     const double radius,
	     const double height, const int name,
		    const bool drawends)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  DrawCylinder(base_centre, orientation, radius, height,drawends);
  glLoadName(0);    
}

// }}}

// Cone
void ThreeDToolGLCommon::DrawCone(const double bottomradius,
				  const double topradius,
				  const double height)
{
   if (VERBOSE) cerr << "Cone at current origin" << endl;

  gluCylinder(cylinderptr, bottomradius, topradius, height, 20, 4);

  // Slightly crude 
  rotation_centre->IncludeCurrent();
 
}

// Disc
void ThreeDToolGLCommon::DrawDisc(const double innerradius,
				  const double outerradius)
{
   if (VERBOSE) cerr << "Disc at current origin" << endl;

  gluDisk(circleptr, innerradius, outerradius, 20, 2);

  // Slightly crude 
  rotation_centre->IncludeCurrent();
 
}


// Draw covariance matrix
// {{{ void ThreeDToolGLCommon::DrawCovariance()

void ThreeDToolGLCommon::
DrawCovariance(const Vector3D& r0, 
	       const VNL::MatrixFixed<3,3,double>& P,
	       const double number_of_sigma)
{
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  glTranslatef(r0.GetX(), r0.GetY(), r0.GetZ());

  // Calculate eigenvalues, vectors of P
  VNL::SymmetricEigensystem<double> eigen(P);

  if (VERBOSE) cerr << "Eigenvectors " << eigen.V << " eigenvalues "
		    << eigen.GetEigenvalue(0) << " " 
		    << eigen.GetEigenvalue(1) << " "
		    << eigen.GetEigenvalue(2) << endl;

  // Eigenvectors become rotation matrix
  // Turn into 4*4 transformation
  GLfloat Varray[16] = {eigen.V(0, 0), eigen.V(1, 0), eigen.V(2, 0), 0.0,
		       eigen.V(0, 1), eigen.V(1, 1), eigen.V(2, 1), 0.0,
		       eigen.V(0, 2), eigen.V(1, 2), eigen.V(2, 2), 0.0,
		       0.0, 0.0, 0.0, 1.0};
  glMultMatrixf(Varray);

  glScalef(sqrt(eigen.GetEigenvalue(0)) * number_of_sigma, 
	   sqrt(eigen.GetEigenvalue(1)) * number_of_sigma, 
	   sqrt(eigen.GetEigenvalue(2)) * number_of_sigma);

  gluSphere(sphereptr, 1, 20, 20);

  glPopMatrix();

  // Slightly crude 
  rotation_centre->Include(r0);
}

// }}}

// {{{ void ThreeDToolGLCommon::DrawCovariance()

void ThreeDToolGLCommon::
DrawCovariance(const Vector3D& r0, 
	       const VNL::MatrixFixed<3,3,double>& P,
	       const double number_of_sigma, const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  DrawCovariance(r0, P, number_of_sigma);
  glLoadName(0);    

}

// }}}

 
// Draw a camera graphic
// We specify the location vector rW of the centre of the rectangle in
// the world frame
// And the quaternion qWO which specifies the transformation between the
// world frame and the object frame in which the rectangle is aligned
void ThreeDToolGLCommon::DrawCameraGraphic(const Vector3D& rW, 
					   const Quaternion& qWO,
					   const double width)
{
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glShadeModel(GL_SMOOTH);

  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  // Camera consists of a cubiod and a cylinder

  // Transform into Object frame
  glTranslatef(rW.GetX(), rW.GetY(), rW.GetZ());
  double ang = qWO.Angle();
  if (ang != 0.0) {
    Vector3D axis(qWO.Axis());
    glRotatef(ang * 180.0 / M_PI, axis.GetX(), axis.GetY(), axis.GetZ());
  }
  
  glutSolidCube(width);


  // Now draw cylinder
  GLUquadricObj *cylinderptr;
  cylinderptr = gluNewQuadric();
  gluQuadricDrawStyle(cylinderptr, GLU_FILL);
  gluQuadricNormals(cylinderptr, GLU_SMOOTH);

  glTranslatef(0.0, 0.0, -width / 2.0);

  // So we can draw our cylinder along -z direction
  glRotatef(180.0, 1.0, 0.0, 0.0);
  gluCylinder(cylinderptr, 0.7 * width / 2.0, 
	      0.8 * width / 2.0, 0.4 * width, 20, 20);

  glPopMatrix();

  // Slightly crude 
  rotation_centre->Include(rW);
}
 
// Selection version of DrawCameraGraphic
void ThreeDToolGLCommon::DrawCameraGraphic(
		       const Vector3D& rW, 
		       const Quaternion& qWO,
		       const double width, const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  DrawCameraGraphic(rW, qWO, width);
  glLoadName(0);    
}

void ThreeDToolGLCommon::Set2DDrawingPosition(const double u, const double v)
{
  // Set a drawing position and scale on the near clipping plane so that
  // things can be drawn in camera coordinates.

  // Do it all in projection matrix (which should be saved by any function
  // calling this one)

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Set projection
  if (currently_drawing_in_selection_mode_flag) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluPickMatrix((GLdouble) click_x0, (GLdouble) (WindowHeight() - click_y0),
		  7.0, 7.0, viewport);
  }

  if (orthographic_projection_flag == false) {
    // Perspective projection
    glFrustum(move_clipping_plane_factor * xL, 
	      move_clipping_plane_factor * xR, 
	      move_clipping_plane_factor * yB, 
	      move_clipping_plane_factor * yT, 
	      move_clipping_plane_factor * zN, 
	      zF);
  } 
  else {
    // Orthographic projection
    glOrtho(xL, xR, yB, yT, move_clipping_plane_factor * zN, zF);
  } 

  // Now make transformation to clipping plane
  if (orthographic_projection_flag == false) {
    // Perspective projection

    glTranslatef((u - u0) * zN / fku, (v0 - v) * zN / fkv, -zN);
    glScalef(zN / fku, -zN / fkv, 1.0);

    // Also set the raster position for drawing text
    glRasterPos3f((u - u0) * zN / fku, (v0 - v) * zN / fkv, 0.0);
  }
  else {
    // Orthographic projection

    glTranslatef((u - u0) * xW / camera_width, (v0 - v) * yH / camera_height, 
		 0.0);
    glScalef(xW / camera_width, -yH / camera_height, 1.0);    

    // Also set the raster position for drawing text
    glRasterPos3f((u - u0) * xW / camera_width, (v0 - v) * yH / camera_height, 
		 -zN);
  }

}

void ThreeDToolGLCommon::Draw2DBegin(double u, double v)
{
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);

  glShadeModel(GL_FLAT);

  // Save the current drawing and projection matrices
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  Set2DDrawingPosition(u, v);
}

void ThreeDToolGLCommon::Draw2DEnd()
{
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

void ThreeDToolGLCommon::Draw2DLine(const double u1, const double v1, 
                    const double u2, const double v2)
{
  Draw2DBegin(u1, v1);

  glBegin(GL_LINES); {
    glVertex3f(0.0, 0.0, 0.0);    
    glVertex3f(u2-u1, v2-v1, 0.0);
  }; glEnd();

  Draw2DEnd();
}

void ThreeDToolGLCommon::Draw2DArrow(
  const double u1, const double v1, double size1,
  const double u2, const double v2, double size2)
{
  Draw2DBegin(u1, v1);

  VNL::VectorFixed<2> line_to;
  line_to[0] = u2-u1;
  line_to[1] = v2-v1;
  glBegin(GL_LINES);
  {
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(line_to[0], line_to[1], 0.0);
  };
  glEnd();

  VNL::VectorFixed<2> line_dir = line_to;
  VNL::VectorFixed<2> cross_dir;
  cross_dir[0] = line_dir[1];
  cross_dir[1] = -line_dir[0];
  
  line_dir.Normalize();
  cross_dir.Normalize();
  cross_dir /= 2.0;
  
  glBegin(GL_TRIANGLES);
  {
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f((line_dir[0] + cross_dir[0]) * size1,
               (line_dir[1] + cross_dir[1]) * size1,
               0.0);
    glVertex3f((line_dir[0] - cross_dir[0]) * size1,
               (line_dir[1] - cross_dir[1]) * size1,
               0.0);
    
    glVertex3f(line_to[0], line_to[1], 0.0);
    glVertex3f(line_to[0] + (-line_dir[0] + cross_dir[0]) * size2,
               line_to[1] + (-line_dir[1] + cross_dir[1]) * size2,
               0.0);
    glVertex3f(line_to[0] + (-line_dir[0] - cross_dir[0]) * size2,
               line_to[1] + (-line_dir[1] - cross_dir[1]) * size2,
               0.0);
  };
  glEnd();

  Draw2DEnd();
}

void ThreeDToolGLCommon::Draw2DLine(const double u1, const double v1,
            const double u2, const double v2, const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  Draw2DLine(u1, v1, u2, v2);
  glLoadName(0);    
}

void ThreeDToolGLCommon::Draw2DArrow(
  const double u1, const double v1, double size1,
  const double u2, const double v2, double size2, const int name)
{
  if (currently_drawing_in_selection_mode_flag) 
    glLoadName(name);
  
  Draw2DArrow(u1, v1, size1, u2, v2, size2);

  glLoadName(0);    
}

void ThreeDToolGLCommon::Draw2DDashedLine(const double u1,      const double v1,
				          const double u2,      const double v2,
                                          const double ndashes, const double dash_gap_size_ratio)
{
  Draw2DBegin(u1, v1);

  double dash_fraction = dash_gap_size_ratio/((double)ndashes * dash_gap_size_ratio + (double)ndashes - 1.0);
  double dash_u = dash_fraction * (u2 - u1);
  double dash_v = dash_fraction * (v2 - v1);
  double dashgap_u = (1.0 + (1.0/dash_gap_size_ratio)) * dash_u;
  double dashgap_v = (1.0 + (1.0/dash_gap_size_ratio)) * dash_v;
  double u = 0.0;
  double v = 0.0;

  glBegin(GL_LINES); {
    for (int i=0; i<ndashes; ++i) {
      glVertex3f(u, v, 0.0);    
      glVertex3f(u+dash_u, v+dash_v, 0.0);
      u += dashgap_u;
      v += dashgap_v;
    }
  }; glEnd();

  Draw2DEnd();
}

void ThreeDToolGLCommon::Draw2DDashedLine(const double u1,      const double v1, 
				          const double u2,      const double v2, 
                                          const double ndashes, const double dash_gap_size_ratio,
                                          const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  Draw2DDashedLine(u1, v1, u2, v2, ndashes, dash_gap_size_ratio);
  glLoadName(0);    
}

void ThreeDToolGLCommon::Draw2DRectangle(const double u, const double v, 
				      const double width, const double height)
{

  Draw2DBegin(u, v);

  glBegin(GL_LINES); {
    glVertex3f(-0.5 * width, -0.5 * height, 0.0);    
    glVertex3f(-0.5 * width,  0.5 * height, 0.0);
    glVertex3f(-0.5 * width,  0.5 * height, 0.0);    
    glVertex3f( 0.5 * width,  0.5 * height, 0.0);
    glVertex3f( 0.5 * width,  0.5 * height, 0.0);    
    glVertex3f( 0.5 * width, -0.5 * height, 0.0);
    glVertex3f( 0.5 * width, -0.5 * height, 0.0);    
    glVertex3f(-0.5 * width, -0.5 * height, 0.0);
  }; glEnd();

  Draw2DEnd();
}

void ThreeDToolGLCommon::Draw2DRectangle(const double u, const double v, 
				      const double width, const double height,
					 const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  Draw2DRectangle(u, v, width, height);
  glLoadName(0);    
}

void ThreeDToolGLCommon::Draw2DTexturedRectangle(
                               const double u, const double v, 
			       const double width, const double height, 
			       const VW::ImageMono<unsigned char> &imageref)
{
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);

  glShadeModel(GL_FLAT);

  int texture_width = int (pow(2.0, 
		 int((log(double(imageref.GetWidth())) / log(2.0)) + 1.0)));
  int texture_height = int (pow(2.0, 
	       	 int((log(double(imageref.GetHeight())) / log(2.0)) + 1.0)));
  if (VERBOSE) cerr << "2D Texture width " << texture_width << " height " << 
		 texture_height << endl;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  GLuint texName;
  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_interpolation);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_interpolation);

  GLubyte *dummy_texture = new GLubyte [texture_width * texture_height];
  for (int i = 0; i < texture_width * texture_height; i++)
    dummy_texture[i] = (unsigned char) 
      (double(texture_width * texture_height) / double(i)
       * double(255));
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 
	       0, GL_LUMINANCE, GL_UNSIGNED_BYTE, dummy_texture);
  delete dummy_texture;

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		  imageref.GetWidth(), 
		  imageref.GetHeight(), 
		  GL_LUMINANCE,
		  GL_UNSIGNED_BYTE, 
		  imageref.GetRawBuffer());

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  // Save the current drawing and projection matrices 
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  Set2DDrawingPosition(u, v);

  double useful_width = double(imageref.GetWidth()) /
                       double(texture_width);
  double useful_height = double(imageref.GetHeight()) /
                        double(texture_height);
  if (VERBOSE) cerr << "useful_width " << useful_width << " useful_height "
		    << useful_height << endl;

  glBegin(GL_QUADS);
  glTexCoord2f(0.0, useful_height);   
  glVertex3f(-0.5 * width, 0.5 * height, 0.0);
  glTexCoord2f(0.0,          0.0); 
  glVertex3f(-0.5 * width, -0.5 * height, 0.0);
  glTexCoord2f(useful_width, 0.0); 
  glVertex3f( 0.5 * width, -0.5 * height, 0.0);
  glTexCoord2f(useful_width, useful_height);           
  glVertex3f( 0.5 * width, 0.5 * height, 0.0);
  glEnd();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glDeleteTextures(1, &texName);
  glDisable(GL_TEXTURE_2D);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);

}

void ThreeDToolGLCommon::Draw2DTexturedRectangle(
                               const double u, const double v, 
			       const double width, const double height, 
			       const VW::ImageMono<unsigned char> &imageref,
			       const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  Draw2DTexturedRectangle(u, v, width, height, imageref);
  glLoadName(0);    
}

void ThreeDToolGLCommon::Draw2DCircle(const double u, const double v, 
		  const double inner_radius, const double outer_radius)
{

  Draw2DBegin(u, v);
  gluDisk(circleptr, inner_radius, outer_radius,
	  20, 2);

  Draw2DEnd();
}

void ThreeDToolGLCommon::Draw2DCircle(const double u, const double v, 
		  const double inner_radius, const double outer_radius,
		  const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  Draw2DCircle(u, v, inner_radius, outer_radius);
  glLoadName(0);    
}


void ThreeDToolGLCommon::Draw2DCovariance(const double u, const double v, 
														const VNL::MatrixFixed<2,2,double>& P,
														const double number_of_sigma,
														const double ring_thickness)
{
  Draw2DBegin(u, v);

  // Calculate eigenvalues, vectors of P
  VNL::SymmetricEigensystem<double> eigen(P);

  if (VERBOSE) cerr << "Eigenvectors " << eigen.V << " eigenvalues "
		    << eigen.GetEigenvalue(0) << " " 
		    << eigen.GetEigenvalue(1) << endl;

  // Eigenvectors become rotation matrix
  // Turn into 4*4 transformation
  GLfloat Varray[16] = {eigen.V(0, 0), eigen.V(1, 0), 0.0, 0.0,
		       eigen.V(0, 1), eigen.V(1, 1), 0.0, 0.0,
		       0.0, 0.0, 0.0, 0.0,
		       0.0, 0.0, 0.0, 1.0};
  glMultMatrixf(Varray);

  glScalef(sqrt(eigen.GetEigenvalue(0)) * number_of_sigma, 
	   sqrt(eigen.GetEigenvalue(1)) * number_of_sigma, 
	   1.0); 

  double inner_size = (sqrt(eigen.GetEigenvalue(0)) * number_of_sigma 
		      - ring_thickness) /
   (sqrt(eigen.GetEigenvalue(0)) * number_of_sigma);
  if (inner_size < 0.0)
    inner_size = 0.0;

  gluDisk(circleptr, inner_size, 1.0,
	  20, 20);

  Draw2DEnd();
}

void ThreeDToolGLCommon::Draw2DCovariance(const double u, const double v, 
					  const VNL::MatrixFixed<2,2,double>& P,
					  const double number_of_sigma,
			const double ring_thickness, const int name)
{
  if (currently_drawing_in_selection_mode_flag) {
    glLoadName(name);
  }
  Draw2DCovariance(u, v, P, number_of_sigma, ring_thickness);
  glLoadName(0);    
}


void ThreeDToolGLCommon::Draw2DText(
  const double u, const double v, const std::string& text)
{
  // Save the current drawing and projection matrices 
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  Set2DDrawingPosition(u, v);
  
  for (unsigned int i = 0; i < text.size(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,text[i] );  
  }

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}


// {{{ void ThreeDToolGLCommon::DrawRotationCentre()

void ThreeDToolGLCommon::DrawRotationCentre()
{
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);

  glPointSize(4);

  glBegin(GL_POINTS); {
    glVertex3d(rotation_centre->Centre().GetX(), 
	       rotation_centre->Centre().GetY(), 
	       rotation_centre->Centre().GetZ());
  }; glEnd();

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

}

// }}}


// Utility function to work out projection of planar coordinates onto
// distorted sphere
// Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet (??)
// if we are away from the center of the sphere.
// {{{ double ThreeDToolGLCommon::ProjectOntoSphere()

double ThreeDToolGLCommon::ProjectOntoSphere(const double px, 
						const double py)
{
  double z;

  double d = sqrt(px * px + py * py);
  if (d < TRACKBALLSIZE * 0.70710678118654752440) {    /* Inside sphere */
    z = sqrt(TRACKBALLSIZE*TRACKBALLSIZE - d*d);
  } else {           /* On hyperbola */
    double t = TRACKBALLSIZE / 1.41421356237309504880;
    z = t*t / d;
  }
    
  return z;
}

// }}}

// TrackBall function for internal use: takes input of normalised 
// image drag coordinates and produces quaternion rotation
// {{{ Quaternion ThreeDToolGLCommon::TrackBall()

Quaternion 
ThreeDToolGLCommon::TrackBall(const double p1x, const double p1y, 
				 const double p2x, const double p2y)
{
  // Input coordinates p1 and p2 are in the range -1 -> 1 representing
  // mouse image position before and after drag (in GL style,
  // -1,-1 is image bottom-left, 0,0 is centre and 1,1 is top-right)

  if (p1x == p2x && p1y == p2y) {
    Quaternion q(0,0,0,1); 
    return q;
  }

  // Work out z coordinates of projection of p1 and p2 onto sphere
  Vector3D p1(p1x, p1y, ProjectOntoSphere(p1x, p1y));
  Vector3D p2(p2x, p2y, ProjectOntoSphere(p2x, p2y));

  if (VERBOSE) cerr << "p1 " << p1 << " p2 " << p2 << endl;

  // Find rotation axis
  Vector3D cross = VectorProduct(p1, p2);

  if (VERBOSE) cerr << "cross " << cross << endl;

  // And angle of rotation
  Vector3D d = p1 - p2;
  double t = d.Norm() / (2.0 * TRACKBALLSIZE);

  // Avoid problems with out-of-control values
  if (t > 1.0) t = 1.0;
  if (t < -1.0) t = -1.0;
  double phi = 2.0 * asin(t);

  // Make sure it's a unit vector
  cross.Normalise();

  Quaternion q(cross, phi);

  return q;
}

// }}}




void ThreeDToolGLCommon::SetMaterial(const GLMaterialType material)
{
  GLfloat diffuse[] = { 0,0,0,0 };
  GLfloat ambient[] = { 0,0,0,0 };
  GLfloat specular[] = { 1,1,1,1 };
  GLfloat shininess[] = { 50 };
  GLfloat emission[] = { 0,0,0,0 };


  switch (material) {
  default:
  case(GLMATERIAL_BRASS):
    ambient[0]  = 0.33; ambient[1]  = 0.22; ambient[2]  = 0.03; 
    ambient[3]  = 1.0;
    diffuse[0]  = 0.78; diffuse[1]  = 0.57; diffuse[2]  = 0.11; 
    diffuse[3]  = 1.0;
    specular[0] = 0.99; specular[1] = 0.91; specular[2] = 0.81; 
    specular[3] = 1.0;
    emission[0] = 0.00; emission[1] = 0.00; emission[2] = 0.00; 
    emission[3] = 0.0;
    shininess[0] = 27.8;
    break;
  case(GLMATERIAL_REDPLASTIC):
    ambient[0]  = 0.00; ambient[1]  = 0.00; ambient[2]  = 0.00; 
    ambient[3]  = 1.0;
    diffuse[0]  = 0.50; diffuse[1]  = 0.00; diffuse[2]  = 0.00; 
    diffuse[3]  = 1.0;
    specular[0] = 0.70; specular[1] = 0.60; specular[2] = 0.60; 
    specular[3] = 1.0;
    emission[0] = 0.00; emission[1] = 0.00; emission[2] = 0.00; 
    emission[3] = 0.0;
    shininess[0] = 32.0;
    break;
  case(GLMATERIAL_EMERALD):
    ambient[0]  = 0.0215; ambient[1]  = 0.1745; ambient[2]  = 0.0215; 
    ambient[3]  = 1.0;
    diffuse[0]  = 0.07568; diffuse[1]  = 0.61424; diffuse[2]  = 0.07568; 
    diffuse[3]  = 1.0;
    specular[0] = 0.633; specular[1] = 0.727811; specular[2] = 0.633; 
    specular[3] = 1.0;
    emission[0] = 0.00; emission[1] = 0.00; emission[2] = 0.00; 
    emission[3] = 0.0;
    shininess[0] = 76.8;
    break;
  case(GLMATERIAL_SLATE):
    ambient[0]  = 0.02; ambient[1]  = 0.02; ambient[2]  = 0.02; 
    ambient[3]  = 1.0;
    diffuse[0]  = 0.02; diffuse[1]  = 0.01; diffuse[2]  = 0.01; 
    diffuse[3]  = 1.0;
    specular[0] = 0.40; specular[1] = 0.40; specular[2] = 0.40; 
    specular[3] = 1.0;
    emission[0] = 0.00; emission[1] = 0.00; emission[2] = 0.00; 
    emission[3] = 0.0;
    shininess[0] = 0.78125;
    break;
  }
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT, GL_EMISSION, emission);
}


void 
ThreeDToolGLCommon::SetAntialiasing(bool on)
{
  if (on) {
    texture_interpolation = GL_LINEAR;
  }
  else {
    texture_interpolation = GL_NEAREST;
  }
}


// {{{ GetImage()

void 
ThreeDToolGLCommon::GetImage(VW::ImageMono<unsigned char>& image)
{
  
  MakeCurrent();

  unsigned int w=WindowWidth();
  unsigned int h=WindowHeight();

  // Check that sizes match, and if not exit
  if (image.IsAlloced()) {
    if ((image.GetWidth()!=w) 
	|| (image.GetHeight()!=h)) {
      std::cerr 
	<< "ThreeDToolGTKGLWidget::GetImage - sizes do not match (image is "
	<< image.GetWidth() << "x" <<image.GetHeight() << " but display is "
	<< w << "x" << h << endl;
      exit(0);
    }
  } else {
    // Alloc the image
    image.AllocImageData(w,h);
  }

  // For some reason, reading in luminance format doesn't work, so read
  // RGB then convert
  VW::ImageRGB<unsigned char> rgb(w,h);

  // Must read the image backwards
  for (unsigned int row=0;row<h;row++) 
    glReadPixels(0,h-row-1,
		 w,1,
		 GL_BGRA,
		 GL_UNSIGNED_BYTE,
		 static_cast<void*>(rgb.GetRowStart(row)));
    
  VW::ImageCopy(rgb,image);

}

void 
ThreeDToolGLCommon::GetImage(VW::ImageRGB<unsigned char>& image)
{
  MakeCurrent();

  unsigned int w=WindowWidth();
  unsigned int h=WindowHeight();

  // Check that sizes match, and if not exit
  if (image.IsAlloced()) {
    if ((image.GetWidth()!=w) 
	|| (image.GetHeight()!=h)) {
      std::cerr 
	<< "ThreeDToolGTKGLWidget::GetImage - sizes do not match (image is "
	<< image.GetWidth() << "x" << image.GetHeight() << " but display is "
	<< w << "x" << h << endl;
      exit(0);
    }
  } else {
    // Alloc the image
    image.AllocImageData(w,h);
  }

  // Must read the image backwards
  for (unsigned int row=0;row<h;row++) 
    glReadPixels(0,h-row-1,
		 w,1,
		 GL_BGRA,
		 GL_UNSIGNED_BYTE,
		 static_cast<void*>(image.GetRowStart(row)));
    
}

// }}}


void ThreeDToolGLCommon::
TranslateDrawingPosition(const Vector3D& rO)
{
  current_drawing_position.rW = current_drawing_position.rW + 
    current_drawing_position.qWO.RotationMatrix() * rO;

  glMatrixMode(GL_MODELVIEW);
  glTranslatef(rO.GetX(), rO.GetY(), rO.GetZ());
}

void ThreeDToolGLCommon::RotateDrawingPosition(const Quaternion& qON)
{
  current_drawing_position.qWO = current_drawing_position.qWO * qON;

  glMatrixMode(GL_MODELVIEW);
  double ang = qON.Angle();
  if (ang != 0.0) {
    Vector3D axis(qON.Axis());
    glRotatef(ang * 180.0 / M_PI, axis.GetX(), axis.GetY(), axis.GetZ());
  }
}
 
void ThreeDToolGLCommon::
RotateDrawingPosition(const RotationMatrix& RON)
{
  // Turn rotation matrix into quaternion
  Quaternion qON(RON);

  RotateDrawingPosition(qON);
}

void ThreeDToolGLCommon::PushDrawingPosition()
{
  drawing_position_stack.push_back(current_drawing_position);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  if (VERBOSE) cerr << "Now " << drawing_position_stack.size() 
		    << " transformations on drawing position stack." << endl;
}
 
void ThreeDToolGLCommon::PopDrawingPosition()
{
  assert(drawing_position_stack.size() != 0);

  current_drawing_position = drawing_position_stack.back();
  drawing_position_stack.pop_back();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  if (VERBOSE) cerr << "Now " << drawing_position_stack.size() 
		    << " transformations on drawing position stack." << endl;
}
void ThreeDToolGLCommon::_CallDrawEvent(bool b)
{
  // Simply call the callback
  DrawEvent(b);
}

void ThreeDToolGLCommon::_CallProcessHitsEvent(int item, double x, double y)
{
  // Simply call the callback
  ProcessHitsEvent(item,x,y);
}
