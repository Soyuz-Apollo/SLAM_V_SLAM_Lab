/** @file threepointpose.cpp
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
 * alternative coordinate frane using the method of Faugeras to solve for a 
 * quaternion rotation
 *
 */

#include "VNL/Algo/rpolyroots.h"
#include "VNL/Algo/symmetriceigensystem.h"
#include "threepointpose.h"

using namespace VW;

#define REAL_THRESH 1.0e-6
#undef VERBOSE

#ifdef VERBOSE 
#define DEBUG(X) X
#else
#define DEBUG(X)
#endif


ThreePointPose::ThreePointPose(const Point2D _x1, const Point2D _x2, const Point2D _x3, 
			       const Point3D _p1, const Point3D _p2, const Point3D _p3,
			       const CalibrationMatrix _K)
  : x1(_x1), x2(_x2), x3(_x3), p1(_p1), p2(_p2), p3(_p3), K(_K) 
{
  Kinv = K.Inverse();
  double a2 = (p3-p2).NormSquared();
  double b2 = (p1-p3).NormSquared();
  double c2 = (p1-p2).NormSquared();
  Vector3D u1=Kinv*Vector3D(x1.GetX(), x1.GetY(), 1.0), 
           u2=Kinv*Vector3D(x2.GetX(), x2.GetY(), 1.0), 
           u3=Kinv*Vector3D(x3.GetX(), x3.GetY(), 1.0);

  std::cerr << "THREEPOINTPOSE: image points:\n ";
  std::cerr << "[" << u1 << "], [" << u2 << "], [" << u3 << "]" << std::endl;

  u1.Normalise();
  u2.Normalise();
  u3.Normalise();

  std::cerr << "u: " << u1 << "; " << u2 << "; " << u3 << std::endl;

  ComputeFischlerBolles(u1, u2, u3, a2, b2, c2);
  if (NumSolns()>0)
    ComputeQRotn();
}


// ThreePointPose::ThreePointPose(const Point3D u1, const Point3D u2, const Point3D u3,
// 			       const Point3D p1, const Point3D p2, const Point3D p3)
// {
//   double a2 = (p3-p2).NormSquared();
//   double b2 = (p1-p3).NormSquared();
//   double c2 = (p1-p2).NormSquared();
//   ComputeFischlerBolles(u1, u2, u3, a2, b2, c2);
//   ComputeQRotn();
// }


// ThreePointPose::ThreePointPose(const Point2D x1, const Point2D x2, const Point2D x3, 
// 			       const CalibrationMatrix &K,
// 			       const Point3D p1, const Point3D p2, const Point3D p3,
// 			       std::vector<Quaternion>& _q,
// 			       std::vector<Vector3D>& _t)
// {
//   CalibrationMatrix Kinv(K.Inverse());
//   double a2 = (p3-p2).NormSquared();
//   double b2 = (p1-p3).NormSquared();
//   double c2 = (p1-p2).NormSquared();
//   Vector3D u1=Kinv*Vector3D(x1.GetX(), x1.GetY(), 1.0), 
//            u2=Kinv*Vector3D(x2.GetX(), x2.GetY(), 1.0), 
//            u3=Kinv*Vector3D(x3.GetX(), x3.GetY(), 1.0);
//   u1.Normalise();
//   u2.Normalise();
//   u3.Normalise();
//   ComputeFischlerBolles(u1, u2, u3, a2, b2, c2);
//   ComputeQRotn();
//   _q = q; _t = t;
// }


// ThreePointPose::ThreePointPose(const Point3D u1, const Point3D u2, const Point3D u3,
// 			       const Point3D p1, const Point3D p2, const Point3D p3,
// 			       std::vector<Quaternion> &_q,
// 			       std::vector<Point3D> &_t)
// {
//   double a2 = (p3-p2).NormSquared();
//   double b2 = (p1-p3).NormSquared();
//   double c2 = (p1-p2).NormSquared();
//   ComputeFischlerBolles(u1, u2, u3, a2, b2, c2);
//   ComputeQRotn();
//   _q = q; _t = t;
// }


bool ThreePointPose::ComputeJacobians()
{
  bool retval=true;

  // saved versions kept during Jacobian computations 
  std::vector<double> s1_save, s2_save, s3_save;   // depths for each solution
  std::vector<Point3D> P1_save, P2_save, P3_save;  // points in camera frame
  std::vector<Quaternion> q_save;     // rotation from world to camera frame
  std::vector<Vector3D> t_save;       // translation from world to camera frame

  s1_save = s1; s2_save = s2; s3_save = s3;
  P1_save = P1; P2_save = P2; P3_save = P3;
  q_save = q;
  t_save = t;

  std::cout << "Q:\t" << q_save[0] << std::endl; 

  CalibrationMatrix Kinv(K.Inverse());
  double a2 = (p3-p2).NormSquared();
  double b2 = (p1-p3).NormSquared();
  double c2 = (p1-p2).NormSquared();

  J.resize(NumSolns());
  Jdone.resize(NumSolns());

  const double eps = 1.0e-6;
  double x[6] = {x1.GetX(), x1.GetY(), x2.GetX(), x2.GetY(), x3.GetX(), x3.GetY()};
  for (unsigned int i=0; i<6; i++) {
    x[i] += eps;

    Vector3D u1(Kinv*Vector3D(x[0], x[1], 1.0));
    Vector3D u2(Kinv*Vector3D(x[2], x[3], 1.0));
    Vector3D u3(Kinv*Vector3D(x[4], x[5], 1.0));
    u1.Normalise();
    u2.Normalise();
    u3.Normalise();

    std::cerr << "u: " << u1 << "; " << u2 << "; " << u3 << std::endl;

    ComputeFischlerBolles(u1, u2, u3, a2, b2, c2);
    if (s1.size() != s1_save.size()) {
      std::cerr << "Differing number of solutions.  Jacobians aborted\n";
      retval = false;
      for (unsigned int k=0; k<Jdone.size(); k++) Jdone[k] = false;
      break;
    }
    std::cerr << "P1[0]: " << P1[0] << std::endl;
    ComputeQRotn();

    std::cout << "Q(" << i << "+eps):\t" << q[0] << std::endl; 
    
    for (unsigned int k=0; k<NumSolns(); k++) {
      J[k][i][0] = (t[k].GetX()-t_save[k].GetX())/eps;
      J[k][i][1] = (t[k].GetY()-t_save[k].GetY())/eps;
      J[k][i][2] = (t[k].GetZ()-t_save[k].GetZ())/eps;
      J[k][i][3] = (q[k].GetR()-q_save[k].GetR())/eps;
      J[k][i][4] = (q[k].GetX()-q_save[k].GetX())/eps;
      J[k][i][5] = (q[k].GetY()-q_save[k].GetY())/eps;
      J[k][i][6] = (q[k].GetZ()-q_save[k].GetZ())/eps;
      Jdone[k] = true;
    }

    x[i] -= eps;

  }

  // restore result data
  s1 = s1_save; s2 = s2_save; s3 = s3_save;
  P1 = P1_save; P2 = P2_save; P3 = P3_save;
  q = q_save;
  t = t_save;
  
  return retval;


}


void ThreePointPose::ComputeJacobian(const int soln)
{
  // saved versions kept during Jacobian computations 
  std::vector<double> s1_save, s2_save, s3_save;   // depths for each solution
  std::vector<Point3D> P1_save, P2_save, P3_save;  // points in camera frame
  std::vector<Quaternion> q_save;     // rotation from world to camera frame
  std::vector<Vector3D> t_save;       // translation from world to camera frame

  s1_save = s1; s2_save = s2; s3_save = s3;
  P1_save = P1; P2_save = P2; P3_save = P3;
  q_save = q;
  t_save = t;

  CalibrationMatrix Kinv(K.Inverse());
  double a2 = (p3-p2).NormSquared();
  double b2 = (p1-p3).NormSquared();
  double c2 = (p1-p2).NormSquared();

  if (J.size()!=NumSolns()) {
    J.resize(NumSolns());
    Jdone.resize(NumSolns());
  }
  Jdone[soln] = true;
  
  const double eps = 1.0e-6;
  double x[6] = {x1.GetX(), x1.GetY(), x2.GetX(), x2.GetY(), x3.GetX(), x3.GetY()};
  for (unsigned int i=0; i<6; i++) {
    x[i] += eps;

    Vector3D u1(Kinv*Vector3D(x[0], x[1], 1.0));
    Vector3D u2(Kinv*Vector3D(x[2], x[3], 1.0));
    Vector3D u3(Kinv*Vector3D(x[4], x[5], 1.0));
    u1.Normalise();
    u2.Normalise();
    u3.Normalise();

    std::cerr << "u: " << u1 << "; " << u2 << "; " << u3 << std::endl;

    ComputeFischlerBolles(u1, u2, u3, a2, b2, c2);
    // now find which soln for s[123] is closest and take this to
    // be the corresponding one
    int closest=0;
    double best_diff2=1.0e+60;  // big number
    for (unsigned int j=0; j<s1.size(); j++) {
      double diff2 = (s1[j]-s1_save[soln])*(s1[j]-s1_save[soln]) +
	(s2[j]-s2_save[soln])*(s2[j]-s2_save[soln]) +
	(s3[j]-s3_save[soln])*(s3[j]-s3_save[soln]);
      if (diff2 < best_diff2) {
	closest = j;
	best_diff2 = diff2;
      }
    }

    DEBUG(if (closest != soln) {
      std::cerr << "Note: solution indices don't match\n" << std::endl;
    });

    Quaternion tmpq;
    Vector3D tmpt;
    ComputeQRotn(closest, tmpq, tmpt);

    DEBUG(std::cerr << "Q(" << i << "+eps):\t" << q[0] << std::endl); 
    
    J[soln][i][0] = (tmpt.GetX()-t_save[soln].GetX())/eps;
    J[soln][i][1] = (tmpt.GetY()-t_save[soln].GetY())/eps;
    J[soln][i][2] = (tmpt.GetZ()-t_save[soln].GetZ())/eps;
    J[soln][i][3] = (tmpq.GetR()-q_save[soln].GetR())/eps;
    J[soln][i][4] = (tmpq.GetX()-q_save[soln].GetX())/eps;
    J[soln][i][5] = (tmpq.GetY()-q_save[soln].GetY())/eps;
    J[soln][i][6] = (tmpq.GetZ()-q_save[soln].GetZ())/eps;

    x[i] -= eps;

  }

  // restore result data
  s1 = s1_save; s2 = s2_save; s3 = s3_save;
  P1 = P1_save; P2 = P2_save; P3 = P3_save;
  q = q_save;
  t = t_save;
  
  return;


}





void ThreePointPose::ComputeFischlerBolles(Point3D j1, Point3D j2, Point3D j3,
					   double a2, double b2, double c2)
{
  // Compute depths of three points given j1, j2, j3, unit normals in 
  // visual directions of 3 3D points p1, p2, p3.  Multiple solutions returned
  // in Point3D vectors.  Method of Fischler and Bolles.  

  bool permute=false;

  //if (fabs(a2-b2) <REAL_THRESH && fabs(b2-c2)>REAL_THRESH) {
  if (fabs(a2-b2) < fabs(b2-c2)) {
    // Switch point 3 and 1 for numerical stability
    // Could rederive eqns for more efficient and implementation but
    // I'm lazy
    DEBUG(std::cerr << "Permute\n");
    permute=true;
    Point3D jtmp=j3; j3=j1; j1=jtmp;
    double dtmp=c2; c2=a2; a2=dtmp; 
  }

  double cosa = j2*j3, cos2a = cosa*cosa;
  double cosb = j1*j3, cos2b = cosb*cosb;
  double cosc = j1*j2, cos2c = cosc*cosc;

  DEBUG(std::cerr << "a2,b2,c2=" << a2 << " " << b2 << " " << c2 << std::endl);
  DEBUG(std::cerr << "cosa,cosb,cosc=" << cosa << " " << cosb << " " << cosc << std::endl);

  double coeff[5];

  coeff[0] = 4.0*b2*c2*cos2a - (a2-b2-c2)*(a2-b2-c2); // D4
  coeff[1] = -4.0*c2*(a2+b2-c2)*cosa*cosb - 8.0*b2*c2*cos2a*cosc + 4.0*(a2-b2-c2)*(a2-b2)*cosc; // D3
  coeff[2] = 4.0*c2*(a2-c2)*cos2b + 8.0*c2*(a2+b2)*cosa*cosb*cosc + 4.0*c2*(b2-c2)*cos2a - 2.0*(a2-b2-c2)*(a2-b2+c2) - 4.0*(a2-b2)*(a2-b2)*cos2c; // D2
  coeff[3] = -8.0*a2*c2*cos2b*cosc - 4.0*c2*(b2-c2)*cosa*cosb - 4.0*a2*c2*cosa*cosb + 4.0*(a2-b2)*(a2-b2+c2)*cosc; // D1
  coeff[4] = 4.0*a2*c2*cos2b - (a2-b2+c2)*(a2-b2+c2);

  VNL::Vector<double> polycoeffs(coeff, 5);
  VNL::RPolyRoots poly(polycoeffs);

  DEBUG(for (int i=0; i<4; i++) {
    std::cerr << "Root " << i << "=" << poly.Real(i) << "+j" << poly.Imag(i) << std::endl;
  });

  double a2overb2 = a2/b2;

  // clear result data arrays
  s1.clear(); s2.clear(); s3.clear(); 
  P1.clear(); P2.clear(); P3.clear();
  q.clear(); t.clear();

  for (int i=0; i<4; i++) {
    // for each real solution
    if (poly.Imag(i) < REAL_THRESH) {
      double u, v;
      u = poly.Real(i);
      // tmpu is relative depth of point 2, so must be +ve
      if (u>0.0) {
	double U, V, W;
	U = 1.0-a2overb2;
	V = a2overb2*cosb - cosa*u;
	if (fabs(U)<REAL_THRESH) {
	  U = 1.0;
	  V = -cosa*u;
	  W = (1.0-a2/c2)*u*u + 2.0*a2/c2*cosc*u - a2/c2;
	} else {
	  W = u*u -a2overb2;
	}
	double discriminant = V*V-U*W;
	DEBUG(std::cerr << "discr=" << discriminant << " UVW=" << U << " " << V << " " << W << std::endl);
	if (discriminant>=0.0) {
	  double sqrtdiscr = sqrt(discriminant);

	  // quadratic solution 1
	  v = (-V+sqrtdiscr)/U;
	  if (v>0) {
	    double tmps1 = sqrt(a2/(u*u + v*v - 2.0*u*v*cosa));
	    double tmps2 = u*tmps1;
	    double tmps3 = v*tmps1;
	    Point3D tmpP1(tmps1*j1), tmpP2(tmps2*j2), tmpP3(tmps3*j3);
	    if (fabs((tmpP3-tmpP2).NormSquared() -  a2) < REAL_THRESH &&
		fabs((tmpP1-tmpP3).NormSquared() -  b2) < REAL_THRESH &&
		fabs((tmpP1-tmpP2).NormSquared() -  c2) < REAL_THRESH) {
	      // final check: point satisfies original length constraints
	      if (permute) {
		P1.push_back(tmpP3);  s1.push_back(tmps3);
		P2.push_back(tmpP2);  s2.push_back(tmps2);
		P3.push_back(tmpP1);  s3.push_back(tmps1);
	      } else {
		P1.push_back(tmpP1);  s1.push_back(tmps1);
		P2.push_back(tmpP2);  s2.push_back(tmps2);
		P3.push_back(tmpP3);  s3.push_back(tmps3);
	      } 
	    } else {
	      DEBUG(std::cerr << "Solution rejected: [" 
		    << a2 << " " << b2 << " " << c2 << "], ["
		    << (tmpP3-tmpP2).NormSquared() << " "
		    << (tmpP1-tmpP3).NormSquared() << " " 
		    << (tmpP1-tmpP2).NormSquared() << "]\n");
	    }
	  }

	  // quadratic solution 2
	  v = (-V-sqrtdiscr)/U;
	  if (v>0) {
	    double tmps1 = sqrt(a2/(u*u + v*v - 2.0*u*v*cosa));
	    double tmps2 = u*tmps1;
	    double tmps3 = v*tmps1;
	    Point3D tmpP1(tmps1*j1), tmpP2(tmps2*j2), tmpP3(tmps3*j3);
	    if (fabs((tmpP3-tmpP2).NormSquared() -  a2) < REAL_THRESH &&
		fabs((tmpP1-tmpP3).NormSquared() -  b2) < REAL_THRESH &&
		fabs((tmpP1-tmpP2).NormSquared() -  c2) < REAL_THRESH) {
	      // final check: point satisfies original length constraints
	      if (permute) {
		P1.push_back(tmpP3);  s1.push_back(tmps3);
		P2.push_back(tmpP2);  s2.push_back(tmps2);
		P3.push_back(tmpP1);  s3.push_back(tmps1);
	      } else {
		P1.push_back(tmpP1);  s1.push_back(tmps1);
		P2.push_back(tmpP2);  s2.push_back(tmps2);
		P3.push_back(tmpP3);  s3.push_back(tmps3);
	      }
	    }  else {
	      DEBUG(std::cerr << "Solution rejected: [" 
		    << a2 << " " << b2 << " " << c2 << "], ["
		    << (tmpP3-tmpP2).NormSquared() << " "
		    << (tmpP1-tmpP3).NormSquared() << " " 
		    << (tmpP1-tmpP2).NormSquared() << "]\n");
	    }
	  }
	}
      }
    }
  }
  
  return;
}


void ThreePointPose::ComputeFinsterwalder(const Point3D j1, const Point3D j2, const Point3D j3,
					  const Point3D p1, const Point3D p2, const Point3D p3
			     //std::vector<Pose> pose
					   ) 
{
  // Compute depths of three points given j1, j2, j3, unit normals in 
  // visual directions of 3 3D points p1, p2, p3.  Method of Finsterwalder
  // claimed by Haralick et al to be most accurate.  Singularity when two
  // visual directions are normal though.

  double a2 = (p3-p2).NormSquared();
  double b2 = (p1-p3).NormSquared();
  double c2 = (p1-p2).NormSquared();
  double cosa = j2*j3, sin2a = 1-cosa*cosa;
  double cosb = j1*j3, sin2b = 1-cosb*cosb;
  double cosc = j1*j2, sin2c = 1-cosc*cosc;

  double tmp = (cosa*cosb*cosc - 1.0);
  double coeff[4];
  coeff[0] = c2*(c2*sin2b - b2*sin2c); //G
  coeff[1] = b2*(b2 - a2)*sin2c + c2*(c2 + 2.0*a2)*sin2b + 2.0*b2*c2*tmp; //H
  coeff[2] = b2*(b2 - c2)*sin2a + a2*(a2 + 2.0*c2)*sin2b + 2.0*a2*b2*tmp;//I
  coeff[3] = a2*(a2*sin2b - b2*sin2a); //J

  VNL::Vector<double> polycoeffs(coeff, 4);
  VNL::RPolyRoots poly(polycoeffs);
  double lambda = poly.RealRoots()[0];

  std::cerr << "lambda=" << lambda <<std::endl;

  double A = 1.0 + lambda;
  double B = -cosa;
  double C = (b2 - a2 - lambda*c2)/b2;
  double D = -lambda*cosc;
  double E = (a2 + lambda*c2)*cosb/b2;
  double F = (-a2 + lambda*(b2-c2))/b2;

  double X = sqrt(B*B-A*C), Y = sqrt(E*E-C*F);
  if (B*E-C*D < 0) Y = -Y;

  std::cerr << "X=" << X << "Y=" << Y << std::endl;

  double q = c2/b2;

  // +ve root  eqn (11)
  double P = (X-B)/C, Q = (Y-E)/C;

  double U = 1-q*P*P;
  double V = P*q*cosb - P*Q*q - cosc;
  double W = -Q*Q*q + 2.0*Q*q*cosb + 1.0 - q;

  std::vector<double> u, v;
  double discriminant, tmpu, tmpv;
  
  discriminant = V*V - W*U;
  if (discriminant>=0.0) {
    tmpu = (-V + sqrt(V*V - W*U))/U;
    tmpv = P*tmpu + Q;
    // first soln for u, ensure we get +ve depths
    if (tmpu>0.0 && tmpv>0.0) {
      u.push_back(tmpu);
      v.push_back(tmpv);
    }
    tmpu = (-V - sqrt(V*V - W*U))/U;
    tmpv = P*tmpu + Q;
    // second soln for u, ensure we get +ve depths
    if (tmpu>0.0 && tmpv>0.0) {
      u.push_back(tmpu);
      v.push_back(tmpv);
    }
  }

  // -ve root  eqn (11)
  P = (-B-X)/C; Q = (-E-Y)/C;

  U = 1-q*P*P;
  V = P*q*cosb - P*Q*q - cosc;
  W = -Q*Q*q + 2.0*Q*q*cosb + 1.0 - q;

  discriminant = V*V - W*U;
  if (discriminant>=0.0) {
    tmpu =(-V + sqrt(V*V - W*U))/U;
    tmpv = P*tmpu + Q;
    // first soln for u, ensure we get +ve depths
    if (tmpu>0.0 && tmpv>0.0) {
      u.push_back(tmpu);
      v.push_back(tmpv);
    }
    tmpu = (-V - sqrt(V*V - W*U))/U;
    tmpv = P*tmpu + Q;
    // second soln for u, ensure we get +ve depths
    if (tmpu>0.0 && tmpv>0.0) {
      u.push_back(tmpu);
      v.push_back(tmpv);
    }
  }

  for (unsigned int i=0; i<u.size(); i++) {
    double s1 = sqrt(a2/(u[i]*u[i] + v[i]*v[i] - 2.0*u[i]*v[i]*cosa));
    double s2 = u[i]*s1;
    double s3 = v[i]*s1;

    Point3D P1(s1*j1), P2(s2*j2), P3(s3*j3);
    std::cerr << "Solution " << i << ": ";
    std::cerr << s1 << "," << s2 << "," << s3 << std::endl;
  }

  return;
}

// Compute rotation using Faugeras' method (see book) with quaternions
// and get average translation.  Assumption is Pi = R pi + t.
void ThreePointPose::ComputeQRotn()
{
  Vector3D m, n, u, v;
  VNL::MatrixFixed<4,4,double> A;

  for (unsigned int i=0; i<P1.size(); i++) {
    m = p2-p1; n = P2[i]-P1[i];
    u = m+n;   v = m-n;

    A[0][0] = v.GetX()*v.GetX() + u.GetY()*u.GetY() + u.GetZ()*u.GetZ();
    A[0][1] = v.GetX()*v.GetY() - u.GetX()*u.GetY();
    A[0][2] = v.GetX()*v.GetZ() - u.GetX()*u.GetZ();
    A[0][3] = u.GetY()*v.GetZ() - u.GetZ()*v.GetY();
    A[1][1] = u.GetX()*u.GetX() + v.GetY()*v.GetY() + u.GetZ()*u.GetZ();
    A[1][2] = v.GetY()*v.GetZ() - u.GetY()*u.GetZ();
    A[1][3] = v.GetX()*u.GetZ() - u.GetX()*v.GetZ();
    A[2][2] = u.GetX()*u.GetX() + u.GetY()*u.GetY() + v.GetZ()*v.GetZ();
    A[2][3] = u.GetX()*v.GetY() - u.GetY()*v.GetX();
    A[3][3] = v.GetX()*v.GetX() + v.GetY()*v.GetY() + v.GetZ()*v.GetZ();

    m = p3-p1; n = P3[i]-P1[i];
    u = m+n;   v = m-n;

    A[0][0] += v.GetX()*v.GetX() + u.GetY()*u.GetY() + u.GetZ()*u.GetZ();
    A[0][1] += v.GetX()*v.GetY() - u.GetX()*u.GetY();
    A[0][2] += v.GetX()*v.GetZ() - u.GetX()*u.GetZ();
    A[0][3] += u.GetY()*v.GetZ() - u.GetZ()*v.GetY();
    A[1][1] += u.GetX()*u.GetX() + v.GetY()*v.GetY() + u.GetZ()*u.GetZ();
    A[1][2] += v.GetY()*v.GetZ() - u.GetY()*u.GetZ();
    A[1][3] += v.GetX()*u.GetZ() - u.GetX()*v.GetZ();
    A[2][2] += u.GetX()*u.GetX() + u.GetY()*u.GetY() + v.GetZ()*v.GetZ();
    A[2][3] += u.GetX()*v.GetY() - u.GetY()*v.GetX();
    A[3][3] += v.GetX()*v.GetX() + v.GetY()*v.GetY() + v.GetZ()*v.GetZ();

    A[1][0]=A[0][1]; A[2][0]=A[0][2]; A[3][0]=A[0][3];
    A[2][1]=A[1][2]; A[3][1]=A[1][3]; A[3][2]=A[2][3];

    VNL::SymmetricEigensystem<double> V(A);
    VNL::VectorFixed<4,double> qv = V.GetEigenvector(0);

    DEBUG(
	  std::cerr << "Min eigenval = " << V.GetEigenvalue(0) << std::endl;
	  for (int j=0; j<4; j++) {
	    std::cerr << j << ": " 
		      << V.GetEigenvalue(j) 
		      << "[" << V.GetEigenvector(j) << "]" << std::endl;
	  });

    Quaternion tmpq(qv[0], qv[1], qv[2], qv[3]);
    Vector3D tmpt(((P1[i]+P2[i]+P3[i]) - (tmpq.Rotate(p1)+tmpq.Rotate(p2)+tmpq.Rotate(p3)))/3.0);

    // store inverse transformation
    tmpq = tmpq.Conjugate();
    tmpt = -tmpq.Rotate(tmpt);
    if (tmpq.GetR()<0) {
      q.push_back(Quaternion(-tmpq.GetX(), -tmpq.GetY(), -tmpq.GetZ(), -tmpq.GetR()));
    } else {
      q.push_back(tmpq);
    }
    t.push_back(tmpt);

    DEBUG(
	  std::cerr << "p: [" << p1 << "], [" << p2 << "], [" << p3 << "]\n";
	  std::cerr << "P: [" << P1[i] << "], [" << P2[i] << "], [" << P3[i] << "]\n";
	  std::cerr << "q: [" << tmpq << "]\n";
	  std::cerr << "t: [" << tmpt << "]\n";
	  );
  }

  return;
}


// Compute rotation using Faugeras' method (see book) with quaternions
// and get average translation.  Assumption is Pi = R pi + t.  i is the
// solution number
void ThreePointPose::ComputeQRotn(const int i, Quaternion& q, Vector3D& t)
{
  Vector3D m, n, u, v;
  VNL::MatrixFixed<4,4,double> A;

  m = p2-p1; n = P2[i]-P1[i];
  u = m+n;   v = m-n;

  A[0][0] = v.GetX()*v.GetX() + u.GetY()*u.GetY() + u.GetZ()*u.GetZ();
  A[0][1] = v.GetX()*v.GetY() - u.GetX()*u.GetY();
  A[0][2] = v.GetX()*v.GetZ() - u.GetX()*u.GetZ();
  A[0][3] = u.GetY()*v.GetZ() - u.GetZ()*v.GetY();
  A[1][1] = u.GetX()*u.GetX() + v.GetY()*v.GetY() + u.GetZ()*u.GetZ();
  A[1][2] = v.GetY()*v.GetZ() - u.GetY()*u.GetZ();
  A[1][3] = v.GetX()*u.GetZ() - u.GetX()*v.GetZ();
  A[2][2] = u.GetX()*u.GetX() + u.GetY()*u.GetY() + v.GetZ()*v.GetZ();
  A[2][3] = u.GetX()*v.GetY() - u.GetY()*v.GetX();
  A[3][3] = v.GetX()*v.GetX() + v.GetY()*v.GetY() + v.GetZ()*v.GetZ();

  m = p3-p1; n = P3[i]-P1[i];
  u = m+n;   v = m-n;

  A[0][0] += v.GetX()*v.GetX() + u.GetY()*u.GetY() + u.GetZ()*u.GetZ();
  A[0][1] += v.GetX()*v.GetY() - u.GetX()*u.GetY();
  A[0][2] += v.GetX()*v.GetZ() - u.GetX()*u.GetZ();
  A[0][3] += u.GetY()*v.GetZ() - u.GetZ()*v.GetY();
  A[1][1] += u.GetX()*u.GetX() + v.GetY()*v.GetY() + u.GetZ()*u.GetZ();
  A[1][2] += v.GetY()*v.GetZ() - u.GetY()*u.GetZ();
  A[1][3] += v.GetX()*u.GetZ() - u.GetX()*v.GetZ();
  A[2][2] += u.GetX()*u.GetX() + u.GetY()*u.GetY() + v.GetZ()*v.GetZ();
  A[2][3] += u.GetX()*v.GetY() - u.GetY()*v.GetX();
  A[3][3] += v.GetX()*v.GetX() + v.GetY()*v.GetY() + v.GetZ()*v.GetZ();

  A[1][0]=A[0][1]; A[2][0]=A[0][2]; A[3][0]=A[0][3];
  A[2][1]=A[1][2]; A[3][1]=A[1][3]; A[3][2]=A[2][3];

  VNL::SymmetricEigensystem<double> V(A);
  VNL::VectorFixed<4,double> qv = V.GetEigenvector(0);

  DEBUG(
	std::cerr << "Min eigenval = " << V.GetEigenvalue(0) << std::endl;
	for (int j=0; j<4; j++) {
	  std::cerr << j << ": " 
		    << V.GetEigenvalue(j) 
		    << "[" << V.GetEigenvector(j) << "]" << std::endl;
	});

  
  if (qv[3] < 0) {
    q.Set(-qv[0], -qv[1], -qv[2], -qv[3]);
  } else {
    q.Set(qv[0], qv[1], qv[2], qv[3]);
  }
  t = ((P1[i]+P2[i]+P3[i]) - (q.Rotate(p1)+q.Rotate(p2)+q.Rotate(p3)))/3.0;

  // finally return inverse transformation
  q = q.Conjugate();
  t = -q.Rotate(t);

  return;
}


#if 0
// Compute rotation using Faugeras' method (see book) with quaternions
// and get average translation.  Assumption is P[i] = R p[i] + t.
void ThreePointPose::ComputeQRotn(const std::vector<Point3D> &p,
				  const std::vector<Point3D> &P,
				  Quaternion &q, Vector3D &t)
{
  Point3D m, n, u, v;
  VNL::MatrixFixed<4,4,double> A;
  A.Fill(0);

  for (unsigned int i=0; i<p.size(); i++) {
    u = p[i]+P[i];   v = p[i]-P[i];

    A[0][0] += v.GetX()*v.GetX() + u.GetY()*u.GetY() + u.GetZ()*u.GetZ();
    A[0][1] += v.GetX()*v.GetY() - u.GetX()*u.GetY();
    A[0][2] += v.GetX()*v.GetZ() - u.GetX()*u.GetZ();
    A[0][3] += u.GetY()*v.GetZ() - u.GetZ()*v.GetY();
    A[1][1] += u.GetX()*u.GetX() + v.GetY()*v.GetY() + u.GetZ()*u.GetZ();
    A[1][2] += v.GetY()*v.GetZ() - u.GetY()*u.GetZ();
    A[1][3] += v.GetX()*u.GetZ() - u.GetX()*v.GetZ();
    A[2][2] += u.GetX()*u.GetX() + u.GetY()*u.GetY() + v.GetZ()*v.GetZ();
    A[2][3] += u.GetX()*v.GetY() - u.GetY()*v.GetX();
    A[3][3] += v.GetX()*v.GetX() + v.GetY()*v.GetY() + v.GetZ()*v.GetZ();
  }

  A[1][0]=A[0][1]; A[2][0]=A[0][2]; A[3][0]=A[0][3];
  A[2][1]=A[1][2]; A[3][1]=A[1][3]; A[3][2]=A[2][3];

  VNL::SymmetricEigensystem<double> V(A);
  VNL::VectorFixed<4,double> qv = V.GetEigenvector(0);

  DEBUG(for (int i=0; i<4; i++) {
    cerr << i << ": " 
	 << V.GetEigenvalue(i) 
	 << "[" << V.GetEigenvector(i) << "]" << endl;
  });

  if (qv[3]<0) {
    q.Set(-qv[0], -qv[1], -qv[2], -qv[3]);
  } else {
    q.Set(qv[0], qv[1], qv[2], qv[3]);
  }

  // now get average translation
  t.Set(0,0,0);
  for (unsigned int i=0; i<p.size(); i++) {
    t += P[i] - q.Rotate(p[i]);
  }
  t /= p.size();

  DEBUG(cerr << "Rotation: [" << q << "]\n";
	cerr << "Translation: [" << t << "]\n";);
}
#endif


#if 0
// axis computation works though has a singularity.  angle computation
// doesn't work
void ThreePointPose::ComputePose(std::vector<Quaternion> q, 
				  std::vector<Point3D> t)
{
  Point3D n1 = P2-P1, n2 = P3-P1;
  Point3D m1 = p2-p1, m2 = p3-p1;
  Point3D axis = VectorProduct(m1-n1, m2-n2);
  axis.Normalise();
  m1.Normalise();
  n1.Normalise();
  Vector3D u=m1-axis, v=n1-axis;
  u.Normalise();
  v.Normalise();
  double ca = u*v;
  double sa = VectorProduct(u,v).Norm();
  double angle = atan2(sa,ca);
  std::cerr << "axis=" << axis << std::endl;
  std::cerr << "angle=" << angle << std::endl;

  return;
}
#endif
