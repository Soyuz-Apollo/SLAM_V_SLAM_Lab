/** @file threepointpose.h
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
 *
 * @class   VW::ThreePointPose 
 * @ingroup GeomCompute
 * @author  ID Reid
 * @date    30/11/04
 * @brief   Compute 3D pose of 3 points given their image projections
 *
 * Determine the depths, locations and pose of three points given their 
 * image projections.  Uses the 3-point method of Fischler and Bolles
 * (which involves solving a degree-4 polynomial) to obtain possible
 * depths of points.  These depths lead directly to point locations, and 
 * can be used to determine the pose of the three points relative to an
 * alternative coordinate frame using the method of Faugeras to solve for a 
 * quaternion rotation
 *
 * Optional computation of Jacobian.  Done numerically via pertubations.
 *
 */


#ifndef _threepointpose_h_
#define _threepointpose_h_

// VW Stuff
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/point3d.h>
#include <VW/GeomObjects/quaternion.h>
#include <VW/GeomObjects/calibrationmatrix.h>

namespace VW {

class ThreePointPose {
  public:
  // Constructors

  // _x1, _x2, _x3 are image coordinates.  if no calibration supplied then 
  // we assume calibration K=I.  p1, p2, p3 are 3D coordinates of points in 
  // an arbitrary frame.  Computes q, t that map from camera coordinate frame
  // to p1, p2, p3.  Stores multiple solutions in a 
  // std::vector of quaternions and translations.
  ThreePointPose(const Point2D _x1, const Point2D _x2, const Point2D _x3, 
		 const Point3D _p1, const Point3D _p2, const Point3D _p3,
		 const CalibrationMatrix K=CalibrationMatrix(1.0,1.0, 0.0, 0.0, 0.0));

  // u1, u2, u3 are unit vectors in the camera coordinate system
  // so we assume calibration K=I.  p1, p2, p3 are 3D coordinates
  // of points in an arbitrary frame.  Computes q, t that map from camera
  // coordinate frame to p1, p2, p3.  Stores multiple solutions in a 
  // std::vector of quaternions and translations.
/*   ThreePointPose(const Point3D u1, const Point3D u2, const Point3D u3, */
/* 		 const Point3D p1, const Point3D p2, const Point3D p3);  */

  // next two same as above but returns q and t
/*   ThreePointPose(const Point2D x1, const Point2D x2, const Point2D x3,  */
/* 		 const CalibrationMatrix &K, */
/* 		 const Point3D p1, const Point3D p2, const Point3D p3, */
/* 		 std::vector<Quaternion> &_q, */
/* 		 std::vector<Point3D> &_t); */

/* ThreePointPose(const Point3D u1, const Point3D u2, const Point3D u3, */
/* 		 const Point3D p1, const Point3D p2, const Point3D p3, */
/* 		 std::vector<Quaternion> &_q, */
/* 		 std::vector<Point3D> &_t); */

  // Fischler and Bolles takes unit vectors in direction of visual
  // rays and the length of triangle sides.  Stores depths and points
  // in private data s[123] std::vector and P[123] std::vector
  void ComputeFischlerBolles(Point3D j1, Point3D j2, Point3D j3,
			     double a2, double b2, double c2);

  // Using p1, p2, p3 in world frame and computes q, t values that
  // map each soln in camera frame to p[123]
  void ComputeQRotn();

  // Using p1, p2, p3 in world frame and computes q, t values that
  // map ith soln in camera frame into p[123], return q and t
  void ComputeQRotn(const int i, Quaternion& q, Vector3D& t);

  // Method of Finsterwalder, reported by Lee et al to be more 
  // stable/accurate but has singularities.  Has not been properly 
  // implemented for various degenerate cases that require a permutation
  // of points
  void ComputeFinsterwalder(Point3D j1, Point3D j2, Point3D j3,
			     Point3D p1, Point3D p2, Point3D p3);

  // takes a vector of world points p, and computes the q and t
  // to align with camera points P using a least squares soln for
  // rotn and average for translation
/*   void ComputeQRotn(const std::vector<Point3D> &p, */
/* 		    const std::vector<Point3D> &P, */
/* 		    Quaternion &q, Vector3D &t); */


  // find the jacobian that relates rate of change of solution (for the
  // specified soln number i) to changes in input.  do it numerically by
  // small adjustments to input -- not efficient but analytic soln not
  // very tractable
  void ComputeJacobian(const int soln);

  // find jacobians for all solutions.  assumes small perturbations in input
  // do not change the number or order of solutions; fails with an error 
  // message if a problem detected.  probably better to use the call above
  // individually which is slower but tries harder to match solutions
  bool ComputeJacobians();

  unsigned int NumSolns() const { return s1.size(); }

  void GetAllDepths(std::vector<double>& _s1,
		 std::vector<double>& _s2,
		 std::vector<double>& _s3) const { _s1=s1; _s2=s2; _s3=s3; }
  void GetDepths(int i, double& _s1, double& _s2, double& _s3) const { 
    _s1=s1[i]; _s2=s2[i]; _s3=s3[i]; 
  }

  void GetAllPoints(std::vector<Point3D>& _P1,
		    std::vector<Point3D>& _P2,
		    std::vector<Point3D>& _P3) const { _P1=P1; _P2=P2; _P3=P3; }
  void GetPoints(int i, Point3D& _P1, Point3D& _P2, Point3D& _P3) const { 
    _P1=P1[i]; _P2=P2[i]; _P3=P3[i]; 
  }

  void GetAllPoses(std::vector<Quaternion>& _q,
		   std::vector<Vector3D>& _t) const { _q=q; _t=t; }
  void GetPose(int i, Quaternion& _q, Vector3D& _t) const {
    _q = q[i]; _t = t[i];
  }

  VNL::MatrixFixed<6,7,double>& GetJacobian(const int i) { 
    if (J.size()==0 || !Jdone[i]) ComputeJacobian(i);
    return J[i]; 
  }

  // given covariance of input points determine to first order the covariance
  // of a given solution.  matrix composition doesn't currently use symmetry 
  // to speed processing
  VNL::MatrixFixed<7,7,double> GetCovariance(const int i, VNL::MatrixFixed<6,6,double> P) { 
    if (J.size()==0 || !Jdone[i]) ComputeJacobian(i);
    return J[i].Transpose()*P*J[i]; 
  }

  operator bool() { return true; }

 private:

  // input data stored for later use
  Vector2D x1, x2, x3;
  Vector3D p1, p2, p3;
  CalibrationMatrix K, Kinv;

  // result data
  std::vector<double> s1, s2, s3;   // depths for each solution
  std::vector<Point3D> P1, P2, P3;  // points in camera frame
  std::vector<Quaternion> q;        // rotation from world to camera frame
  std::vector<Vector3D> t;          // translation from world to camera frame

  std::vector< VNL::MatrixFixed<6,7,double> > J;
  std::vector<bool> Jdone;
};


};



#endif

