/** 
 * @file   threedtoolglcommon.h
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
/**
@class   VW::ThreeDToolGLCommon
@ingroup GLTools
@author  AJ Davison, BJ Tordoff
@date    09/09/2002
@brief   A base class for doing 3D drawing in VW.
 
Although this class cannot be instantiated itself (use one of its derived
classes e.g. VW::ThreeDToolGLUTGLWidget), it provides
a comprehensive set of helper functions for drawing in 3D. When the
display requires redrawing, a user callback is called which should
draw all the required objects.

See the HOWTO for more details.
*/


#ifndef _THREEDTOOL_GL_COMMON_H
#define _THREEDTOOL_GL_COMMON_H

// System includes
#include <string>
#include <vector>

#ifdef _WIN32 // on windows GL barfs if you don't include all of windows first...
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// VW includes
#include <VNL/matrixfixed.h>
#include <VW/events.h>
#include <VW/GeomObjects/quaternion.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/point3d.h>



namespace VW {


  // Some necessary forward declarations
  class RotationMatrix;
  class CalibrationMatrix;
  template <class T> class ImageMono;
  template <class T> class ImageRGB;



  /** Some predefined material types for GL use. */
  enum GLMaterialType {
    GLMATERIAL_BRASS, 
    GLMATERIAL_REDPLASTIC, 
    GLMATERIAL_EMERALD, 
    GLMATERIAL_SLATE
  };








  /** A class to store a translation, rotation drawing position */
  class DrawingPosition {
  public:
    DrawingPosition();
    void Zero();
    
    Vector3D rW;
    Quaternion qWO;
  };












  /** Little class for storing centre of rotation information.
   * Store Min, Max x, y, z of all objects plotted (defining a box containing
   * everything).
   * Centre of rotation is the centre of this box.
   */
  class RotationCentre {
  public:
    RotationCentre(DrawingPosition &current_drawing_position);
    Vector3D Centre() const;
    void Reset();
    
    /** Change the rotation centre depending on a new point */
    void Include(const Vector3D& v0);
    
    /** Include the current origin */
    void IncludeCurrent();
    
    double XMax() const;
    double YMax() const;
    double ZMax() const;
    double XMin() const;
    double YMin() const;
    double ZMin() const;
    
  protected: 
    Vector3D centre;
    double xmin, xmax, ymin, ymax, zmin, zmax;
    bool initialised;
    
    // It knows about the current drawing position of the 3D widget
    DrawingPosition &drawing_position;
  };












class ThreeDToolGLCommon
{
 public:
 /**
 Default constructor. Initialises the class with a camera size of
 DEFAULT_CAMERA_WIDTH x DEFAULT_CAMERA_HEIGHT and generic camera parameters. The
 camera size can be set later by calling SetCameraSize().
 */
  ThreeDToolGLCommon ();
 /**
  Constructor. Initialises the class with a specified camera size and generic
  camera parameters. The camera size is the dimensions of the virtual camera
  used for rendering, not the size of the tool's drawing area. The camera view
  will be scaled to the drawing area (maintaining its aspect ratio).
  @param camera_width The width of the camera.
  @param camera_height The height of the camera.
 */
  ThreeDToolGLCommon (const unsigned int camera_width, 
          const unsigned int camera_height);
  virtual ~ThreeDToolGLCommon(void);


  

  /** @name Setting up the view */
  //@{

  /**
  Set the size of the camera. This function will also reset the camera
  parameters to the generic settings. The camera size is the dimensions of the
  virtual camera used for rendering, not the size of the tool's drawing area.
  The camera view will be scaled to the drawing area (maintaining its aspect 
  ratio).
  */
  void SetCameraSize(unsigned int width, unsigned int size);

  /**
  What is the width of the virtual camera? (Note this is not the same as the
  window width).
  */
  unsigned int CameraWidth() {return camera_width;}
  
  /**
  What is the height of the virtual camera? (Note this is not the same as the
  window height).
  */
  unsigned int CameraHeight() {return camera_height;}

  /** Set camera position.
   * Only used when the external application wants
   * to set the view position itself (rather than the normal case of this 
   * being done using the class's internal mouse-drag system).
   * Here new_position is cW (camera position vector in world frame)
   * And  new_orientation is qWC (quaternion associated with rotation RWC
   * between world frame and camera frame)
   */
  void SetCameraPositionOrientation(const Vector3D&   new_position,
				    const Quaternion& new_orientation);
  void GetCameraPositionOrientation(Vector3D& current_position,
			            Quaternion& current_orientation);

  /** Make the camera look at a position. The current position and vertical are
   * maintained (if possible), so that only the orientation is affected.
   */
  void LookAt(const Point3D& some_point);

  /** Set the parameters of the perspective projection used by the display. */
  void SetCameraParameters(const double fku, const double fkv,
         const double u0, const double v0);

  /** Set the parameters of the perspective projection used by the display.
   * Note that the calibration matrix should be normalised, so that the 
   * GL view will be correct even if the window size is different from the
   * image for which the calibration was calculated.
   * @param normK the normalised calibration matrix (ie. for an image -1<x<1, -0.75<y<0.75)
   */
  void SetCameraParameters(const VW::CalibrationMatrix& normK);

  /** Set the parameters of the orthographic projection used by the display. */
  void SetOrthographicCameraParameters(const double xWidth, 
				       const double yWidth,
				       const double u0, const double v0);

  /** Push the current camera position and calibration onto the stack.
   * Top member of the stack is the current position and calibration
   * (see also PopCameraSettings).
   */
  void PushCameraSettings();

  /** Pop the previous camera position and calibration from the stack,
   * added by a previous PushCameraSettings.
   */
  void PopCameraSettings();

  //@}




  /** @name Shortcuts for some basic GL properties */
  //@{

  /** Set pen colour (characters for R, G, B, alpha) */
  void SetPenColour(const unsigned char red, const unsigned char green, 
        const unsigned char blue, const unsigned char alpha);
  void GetPenColour(unsigned char& red, unsigned char& green,
        unsigned char& blue, unsigned char& alpha);

   /** Set background colour (characters for R, G, B, alpha) */
  void SetBackgroundColour(const unsigned char red, const unsigned char green, 
		    const unsigned char blue, const unsigned char alpha);
 
  /** Shortcut for enabling lighting effects. */
  void EnableLighting();
  /** Shortcut for disabling lighting effects. */
  void DisableLighting();
  /** Shortcut for setting material properties and colours to match a
   * pre-defined type. 
   */
  void SetMaterial(const GLMaterialType material);

  // At the moment this only affects textures, but could affect other things too
  void SetAntialiasing(bool on);
  
  //@}


  /** @name Moving around the world */
  //@{

  /** Translate the current drawing position (NB: can also use a vnl_vector) */
  void TranslateDrawingPosition(const Vector3D& r);
  /** Rotate the current drawing position (NB: can also use a vnl_quaternion) */
  void RotateDrawingPosition(const Quaternion& q);
  /** Rotate the current drawing position */
  void RotateDrawingPosition(const RotationMatrix& R);

  /** Push the current drawing position onto the stack.
   * Top member of the stack is the current drawing position 
   * (see also PopDrawingPosition).
   */
  void PushDrawingPosition();

  /** Pop the previous drawing position from the stack,
   * added by a previous PushDrawingPosition.
   */
  void PopDrawingPosition();

  /** Move to a drawing position so that GL calls can be made to draw
   * 2D stuff in camera coordinates (mostly used internally)
   */
  void Set2DDrawingPosition(const double u, const double v);

  //@}





  /** @name Drawing functions */ 
  //@{

  /** Draw XYZ coordinate axes.
   * Axes are drawn at the world origin with extent calculated from the
   * the rotation centre span.
   */
  void DrawAxes(bool number_axes = true);

  // Point at r0
  void DrawPoint(const Vector3D& r0);
  // Selection version of DrawPoint
  void DrawPoint(const Vector3D& r0, const int name);

  // Straight Line between r0 and r1
  void DrawLine(const Vector3D& r0, 
		const Vector3D& r1);
  // Selection version of DrawLine
  void DrawLine(const Vector3D& r0, 
		const Vector3D& r1, const int name);
  
  /** Draw a triangle.
  @param p1 A point on the triangle
  @param p2 A point on the triangle
  @param p3 A point on the triangle
  */
  void DrawTriangle(
    const Vector3D& p1, const Vector3D& p2,const Vector3D& p3) const;
  
  /** Version of DrawTriangle() (see above) supporting object selection. */
  void DrawTriangle(
    const Vector3D& p1, const Vector3D& p2,const Vector3D& p3, int name) const;
  
  // Draw a rectangle; centre rW orientation qWO
  /** Draw a rectangle.
  @param rW The centre of the rectangle in the world frame
  @param qWO The transformation between the world frame and the object frame
  in which the rectangle is aligned: In the object frame, the height of the
  rectangle is aligned with the y axis; the width with the x axis; and the
  rectangle is perpendicular to the z axis
  @param rectangle_width The width of the rectangle (i.e. extent in the object
  x-axis direction)
  @param rectangle_height The height of the rectangle (i.e. extent in the
  object y-axis direction)
  */
  void DrawRectangle(const Vector3D& rW,
		     const Quaternion& qWO,
		     const double rectangle_width,
		     const double rectangle_height);
  /** Version of DrawRectangle() (see above) supporting object selection. */
  void DrawRectangle(const Vector3D& rW, 
		     const Quaternion& qWO,
		     const double rectangle_width,
		     const double rectangle_height, const int name);

  // Draw a textured rectangle; centre rW orientation qWO
  void DrawTexturedRectangle(const Vector3D& rW, 
			     const Quaternion& qWO,
			     const double rectangle_width,
			     const double rectangle_height,
			     const VW::ImageMono<unsigned char> &imageref);
  // Selection version of DrawTexturedRectangle
  void DrawTexturedRectangle(const Vector3D& rW, 
			     const Quaternion& qWO,
			     const double rectangle_width,
			     const double rectangle_height,
			     const VW::ImageMono<unsigned char> &imageref,
			     const int name);

  // As above with the texture drawn on a quadrilateral on the plane, rather
  // than on a rectangle
  // Specify the position of the four corners of the quadrilateral relative to
  // the centre position
  void DrawWarpedTexturedRectangle(const Vector3D& rW, 
		      const Quaternion& qWO,
		      double corner00[2], double corner01[2], 
		      double corner10[2], double corner11[2],
                      const VW::ImageMono<unsigned char> &imageref,
                      const int name);

  /** Draw a cinema screen image: texture at correct position, depth zC
   * to fill the virtual camera image.
   *
   * @param zC the depth at which to place the cinema screen
   * @param imageref the image to be drawn
   */
  void DrawCinemaScreen(const double zC, 
			const VW::ImageMono<unsigned char> &imageref);

  /** Draw a cinema screen image: texture at correct position, depth zC
   * to fill the virtual camera image. The distortion parameter and distortion centre are
   * used to undistort the image as it is texture mapped.
   *
   * @param zC the depth at which to place the cinema screen
   * @param imageref the image to be drawn
   * @param kappa the coefficeint of distortion (pixels^{-2}).
   * @param distortion_centre the image location at which the radial distortion is centred.
   */
  void DrawCinemaScreen(const double zC, 
			const VW::ImageMono<unsigned char> &imageref,
			double kappa,
			const VW::Point2D& distortion_centre);

  // Sphere at current location, radius R
  void DrawSphere(const double R);
  // Sphere at centre r0, radius R
  void DrawSphere(const Vector3D& r0, const double R);
  // Selection version of DrawSphere
  void DrawSphere(const Vector3D& r0, const double R, 
		  const int name);

  /** Cone at current location */
  void DrawCone(const double bottomradius,
		const double topradius,
		const double height);
  /** Disc (well, more a ring really) at current location */
  void DrawDisc(const double innerradius,
		const double outerradius);
  /** Cylinder at current location */
  void DrawCylinder(const double radius,
		    const double height,
		    const bool drawends=false);
  /** Cylinder */
  void DrawCylinder(const Vector3D& base_centre, 
		    const Quaternion& orientation,
		    const double radius,
		    const double height,
		    const bool drawends=false);
  // Selection version of DrawCylinder
  void DrawCylinder(const Vector3D& base_centre, 
		    const Quaternion& orientation,
		    const double radius,
		    const double height, const int name,
		    const bool drawends=false);

  // Draw covariance matrix: centre r0, covariance P, draw number_of_sigma
  void DrawCovariance(const Vector3D& r0, 
		       const VNL::MatrixFixed<3,3,double>& P,
		       double number_of_sigma);
  // Selection version of DrawCovariance
  void DrawCovariance(const Vector3D& r0, 
		       const VNL::MatrixFixed<3,3,double>& P,
		       const double number_of_sigma, const int name);

  // Draw a camera graphic
  void DrawCameraGraphic(const Vector3D& rW, 
			 const Quaternion& qWO,
			 const double width);
  // Selection version of DrawCameraGraphic
  void DrawCameraGraphic(const Vector3D& rW, 
			 const Quaternion& qWO,
			 const double width, const int name);

  // 2D drawing: draw things which appear on the screen at the front,
  // specified in 2D camera coordinates

  /** Set up the window for drawing in 2D using standard Gl commands. This
  function disables lighting and shading, saves the matrices, and calls
  Set2DDrawingPosition() to set up the correct projection and clipping
  to draw in 2D in a 3D control. This function call should be followed by
  glBegin(), some GL commands, then glEnd() and finished with Draw2DEnd().
  @param u The x-co-ordinate of the drawing origin (in image co-ordinates)
  @param v The y-co-ordinate of the drawing origin (in image co-ordinates)
  **/
  void Draw2DBegin(double u = 0.0, double v = 0.0);
  /// Signal the end of a block started with Draw2DBegin().
  void Draw2DEnd();

  /** Draw a 2D line between two image co-ordinates. This version takes two
  vectors, an alternative version takes four doubles.*/
  void Draw2DLine(const VW::Vector2D& p1, const VW::Vector2D p2);
  /** Draw a 2D line between two image co-ordinates. This version takes four
  doubles; an alternative version takes two Vector2Ds.*/
  void Draw2DLine(const double u1, const double v1, 
          const double u2, const double v2);
  /** Draw a 2D line between two image co-ordinates, with a name for selection.
  This version takes four doubles; an alternative version takes two
  Vector2Ds.*/
  void Draw2DLine(const VW::Vector2D& p1, const VW::Vector2D p2, int name);
  /** Draw a 2D line between two image co-ordinates, with a name for selection.
  This version takes two vectors, an alternative version takes four doubles.*/
  void Draw2DLine(const double u1, const double v1,
      const double u2, const double v2, const int name);

  /** Draw a 2D line between two image co-ordinates with an arrow at each end,
  and a name for selection. The sizees are the lengths of the two arrows, in
  pixels (10.0 is a good size; use 0.0 for no arrow). This version takes four
  doubles; an alternative version takes two Vector2Ds.*/
  void Draw2DArrow(
    const double u1, const double v1, double size1,
    const double u2, const double v2, double size2, int name);
  /** Draw a 2D line between two image co-ordinates with an arrow at each end.
  The sizees are the lengths of the two arrows, in pixels (10.0 is a good
  size; use 0.0 for no arrow). This version takes four doubles; an alternative
  version takes two Vector2Ds.*/
  void Draw2DArrow(
    const double u1, const double v1, double size1,
    const double u2, const double v2, double size2);
  /** Draw a 2D line between two image co-ordinates with an arrow at each end.
  The sizees are the lengths of the two arrows, in pixels (10.0 is a good size;
  use 0.0 for no arrow). This version takes two vectors, an alternative version
  takes four doubles.*/
  void Draw2DArrow(
    const VW::Vector2D& p1, double size1,
    const VW::Vector2D& p2, double size2);
  /** Draw a 2D line between two image co-ordinates with an arrow at each end,
  and a name for selection. The sizees are the lengths of the two arrows, in
  pixels (10.0 is a good size; use 0.0 for no arrow). This version takes two
  vectors, an alternative  version takes four doubles.*/
  void Draw2DArrow(
    const VW::Vector2D& p1, double size1,
    const VW::Vector2D& p2, double size2, int name);
    
  /** Draw a dashed line between the specified positions. */
  void Draw2DDashedLine(const double u1, const double v1,
                                            const double u2, const double v2,
                                            const double ndashes,
                                            const double dash_gap_size_ratio);
  /** Draw a dashed line between the specified positions, selection version. */
  void Draw2DDashedLine(const double u1, const double v1,
                                            const double u2, const double v2,
                                            const double ndashes,
                                            const double dash_gap_size_ratio,
                                            const int name);
 
  // Draw a rectangle: arguments centre, width, height
  void Draw2DRectangle(const double u, const double v, 
		    const double width, const double height);
  // Selection version
  void Draw2DRectangle(const double u, const double v, 
		    const double width, const double height, const int name);

  void Draw2DTexturedRectangle(const double u, const double v, 
			       const double width, const double height, 
			       const VW::ImageMono<unsigned char> &imageref);
  void Draw2DTexturedRectangle(const double u, const double v, 
			       const double width, const double height, 
			       const VW::ImageMono<unsigned char> &imageref,
			       const int name);

  /** Draw circle. 
   * This actually draws a ring; set inner_radius to 0 for filled disk.
   */
  void Draw2DCircle(const double u, const double v, 
		    const double inner_radius, const double outer_radius);
  /** Draw circle (selection version). 
   * This actually draws a ring; set inner_radius to 0 for filled disk.
   */
  void Draw2DCircle(const double u, const double v, 
		    const double inner_radius, const double outer_radius,
		    const int name);
  /** Draw covariance. 
   * This draws a 2D covariance ellipse; u, v centre
   * P covariance matrix. All values in camera pixel coordinates
   * ring_thickness is how thick to draw it (in pixels).
   */
  void Draw2DCovariance(const double u, const double v, 
			const VNL::MatrixFixed<2,2,double>& P,
			const double number_of_sigma,
			const double ring_thickness);
  /** Draw covariance (selection version). 
   * This draws a 2D covariance ellipse; u, v centre
   * P covariance matrix. All values in camera pixel coordinates
   * ring_thickness is how thick to draw it (in pixels).
   */
  void Draw2DCovariance(const double u, const double v, 
			const VNL::MatrixFixed<2,2,double>& P,
			const double number_of_sigma,
			const double ring_thickness, const int name);

  /** Print a string at a given image location. */
  void Draw2DText(const double u, const double v,
				      const std::string& string);

  /** Plot rotation centre.
   * Centre is plotted as a single white point.
   */
  void DrawRotationCentre();

  //@}

  /** @name Other bits n bobs */
  //@{

  /** Include a point in the rotation centre calculation. */
  void IncludeInRotationCentre(const Vector3D& r);

  /** Cause a redraw event immediately. 
   * This is generally a bad idea, and one should use RequestDraw() instead,
   * which causes a redraw event at the next good opportunity.
   */
  void DoDrawing();

  /** Clears screen (this is done automatically when Draw() is called) */
  void Clear();

  /** Capture the current view as an image.
   * Retrieves a mono image of the current view.
   */
  void GetImage(VW::ImageMono<unsigned char>& image);
  /** Capture the current view as an image.
   * Retrieves a colour (RGB) image of the current view.
   */
  void GetImage(VW::ImageRGB<unsigned char>& image);

  //@}



  // Signals which can be connected to user functions

  /** The display needs redrawing.
   * When this signal is emitted the display has already been cleared and
   * the viewpoint set appropriately. The user just needs to draw the scene
   * at the origin.
   */
  VWEvents::Event1<void,bool> DrawEvent;
  VWEvents::Event3<void,int,double,double> ProcessHitsEvent;


  // Virtual functions: must be supplied by derived class
  // Get width and height of window
  virtual unsigned int WindowWidth() const = 0;
  virtual unsigned int WindowHeight() const = 0;
  virtual void SwapBuffers() = 0;
  virtual void MakeCurrent() = 0;
  virtual void RequestDraw() = 0;

  bool Dragging() {return dragging;}

  /** This constant sets default projection zoom: normally choose
   * your own with SetCameraParameters()
   */
  static const double DEFAULT_WIDTH_OVER_FKU;
  /** Sets the camera width used in the default constructor. Usually set your
  own using the other constructor, or calling SetCameraSize().
  */
  static const unsigned int DEFAULT_CAMERA_WIDTH;
  /** Sets the camera height used in the default constructor. Usually set your
  own using the other constructor, or calling SetCameraSize().
  */
  static const unsigned int DEFAULT_CAMERA_HEIGHT;



 protected:
  /** Mouse drag.
   * This should be called by specific
   * GUI classes inherited from this one when the mouse is dragged.
   * @param x0 The previous x position of the mouse
   * @param y0 The previous y position of the mouse
   * @param x1 The current x position of the mouse
   * @param y1 The current  y position of the mouse
   * @param button The button held down (left=1, middle=2, right=3)
   */
  void Dragged(const int x0, const int y0, 
	       const int x1, const int y1, const int button);
  /** Mouse drag has finished.
   * This should be called by specific
   * GUI classes inherited from this one when the mouse drag is released.
   */
  void DragRelease();
  /** Mouse button has been clicked.
   * This should be called by specific
   * GUI classes inherited from this one when a mouse click occurs.
   */
  void Clicked(const int x0, const int y0, const int button);

  /** Call the user draw callback. This is virtual so that child
   * classes (such as ThreeDToolGTKGLWidget) can overload it to
   * also call using SigC etc.
   */
  virtual void _CallDrawEvent(bool b);
  /** Call the user ProcessHits callback. This is virtual so that child
   * classes (such as ThreeDToolGTKGLWidget) can overload it to
   * also call using SigC etc.
   */
  virtual void _CallProcessHitsEvent(int item, double x, double y);

  // Whether we're mid-drag
  bool dragging;  

  // Whether we're using orthographic projection
  bool orthographic_projection_flag;

  // Are textures rendered using nearest neighbour or bilinear interpolation (antialiased)
  GLint texture_interpolation;
  
  // Width and height of "true camera image": this is what the camera
  // calibration stuff relates to, and will not change if the window
  // is resized
  unsigned int camera_width, camera_height;

  bool currently_drawing_in_selection_mode_flag;
  // Click location (used in selection mode)
  int click_x0, click_y0;
  bool gl_initialised;

  RotationCentre *rotation_centre;

  // View projection constants
  // In our language of camera calibration matrix
  double fku, fkv, u0, v0;
  // In GL language
  // In GL, the coordinate frame is such that 
  // -z is forward; y is up; x is right
  // zN and zF set the near and far clipping planes
  // Things are only drawn if they have z values between -zN and -zF 
  // (here in metres
  // since we are using metres for all distances in this class)
  static const double zN ;
  static const double zF ;

  // It seems that some graphics cards (an S3 one I tried) don't like
  // drawing on the near clipping plane (which is where all 2D drawing 
  // is done) 
  // So we move the clipping plane back a tiny bit from where we'll draw
  static const double move_clipping_plane_factor ;
  
  // xL, xR, yB, yT are the left, right, bottom, top borders of the near
  // plane of a frustum
  double xL, xR, yB, yT;
  // xW, xM, yH, yM are just an alternative parameterisation of these:
  // Width and height: xW = xR - xL    ;    yH = yT - yB
  // Middles: xM = (xL + xR) / 2    ;   yM = (yB + yT) / 2
  double xW, xM, yH, yM;

  // TrackBall functions for internal use: takes input of normalised 
  // image drag coordinates and produces quaternion rotation
  static const double TRACKBALLSIZE ;
  
  double ProjectOntoSphere(double px, double py);

  Quaternion TrackBall(const double p1x, const double p1y, 
		       const double p2x, const double p2y);

  // Quadric objects (hopefully we increase efficiency by storing just one
  // of each? But we only store a pointer so....?
  GLUquadricObj *sphereptr;
  GLUquadricObj *cylinderptr;
  GLUquadricObj *circleptr;

  // For internal use: initialise GL on first use
  void InitGL();

  // For internal use: draw a number at current raster position
  void DrawNumberHere(double const number);

  // Slightly crude: the function that actually draws a textured
  // rectangle for internal use only (separated from
  // DrawTexturedRectangle() so that it can also be called by
  // DrawCinemaScreen() without registering its position with the
  // rotation centre).
  void ActuallyDrawTexturedRectangle(const Vector3D rW, 
				     const Quaternion qWO,
				     const double rectangle_width,
				     const double rectangle_height,
			       const VW::ImageMono<unsigned char> &imageref);

  // Specify the four corners of the polygon on the plane defined by rW and qWO that the texture
  // image is applied to. Corner positions are given relative to rW. e.g. setting 
  // corner00 = -rectangle_width/2.0, -rectangle_height/2.0
  // corner01 = -rectangle_width/2.0,  rectangle_height/2.0
  // corner10 =  rectangle_width/2.0, -rectangle_height/2.0
  // corner11 =  rectangle_width/2.0,  rectangle_height/2.0
  // .. is equivalent to calling ActuallyDrawTexturedRectangle
  void ActuallyDrawWarpedTexturedRectangle(const Vector3D rW, 
		      const Quaternion qWO,
		      double corner00[2], double corner01[2], 
		      double corner10[2], double corner11[2], 
                      const VW::ImageMono<unsigned char> &imageref);

  // A version of drawrectangle that also undistorts the target image.
  void ActuallyDrawDistortedTexturedRectangle(const Vector3D rW, 
					      const Quaternion qWO,
					      const double rectangle_width,
					      const double rectangle_height,
					      const VW::ImageMono<unsigned char> &imageref,
					      double kappa,
					      const VW::Point2D& distortion_centre);

  // Any drawing commands will first move through this transform
  // relative to the world frame
  // We reset to zero at the start of every draw cycle
  DrawingPosition current_drawing_position;
  // Drawing position stack
  // Operates like the glPushMatrix / glPopMatrix stack
  std::vector<DrawingPosition> drawing_position_stack;

  // For 3D interaction, store view-point position and orientation
  Vector3D camera_position;
  Quaternion camera_orientation;

  // For drawing cinema screens and the like
  GLuint texName;
  unsigned int texWidth,texHeight;

  // Baclground colour:
  double _R_background, _G_background, _B_background, _alpha_background;
};













// INLINERS

inline Vector3D 
RotationCentre::Centre() const 
{
  return centre;
}
  
inline double 
RotationCentre::XMax() const 
{
  return xmax;
}

inline double 
RotationCentre::YMax() const 
{
  return ymax;
}

inline double 
RotationCentre::ZMax() const 
{
  return zmax;
}

inline double 
RotationCentre::XMin() const 
{
  return xmin;
}
  
inline double 
RotationCentre::YMin() const 
{
  return ymin;
}

inline double 
RotationCentre::ZMin() const 
{
  return zmin;
}

inline void 
ThreeDToolGLCommon::EnableLighting()
{
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

inline void 
ThreeDToolGLCommon::DisableLighting()
{
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
}

inline void 
ThreeDToolGLCommon::IncludeInRotationCentre(const Vector3D& r)
{
  rotation_centre->Include(r);
}

inline void 
ThreeDToolGLCommon::PushCameraSettings()
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
}
 
inline void 
ThreeDToolGLCommon::PopCameraSettings()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}


inline void 
ThreeDToolGLCommon::SetBackgroundColour(const unsigned char red, 
  const unsigned char green, 
		  const unsigned char blue, const unsigned char alpha)
{
  _R_background = double(red)/255;
  _G_background = double(green)/255;
  _B_background = double(blue)/255;
  _alpha_background = double(alpha)/255;
}

inline void ThreeDToolGLCommon::Draw2DLine(
  const VW::Vector2D& p1, const VW::Vector2D p2)
{
  Draw2DLine(p1[0], p1[1], p2[0], p2[1]);
}

inline void ThreeDToolGLCommon::Draw2DLine(
  const VW::Vector2D& p1, const VW::Vector2D p2, int name)
{
  Draw2DLine(p1[0], p1[1], p2[0], p2[1], name);
}

inline void ThreeDToolGLCommon::Draw2DArrow(
    const VW::Vector2D& p1, double size1,
    const VW::Vector2D& p2, double size2)
{
  Draw2DArrow(p1[0], p1[1], size1, p2[0], p2[1], size2);
}

inline void ThreeDToolGLCommon::Draw2DArrow(
    const VW::Vector2D& p1, double size1,
    const VW::Vector2D& p2, double size2, int name)
{
  Draw2DArrow(p1[0], p1[1], size1, p2[0], p2[1], size2, name);
}

};
#endif
