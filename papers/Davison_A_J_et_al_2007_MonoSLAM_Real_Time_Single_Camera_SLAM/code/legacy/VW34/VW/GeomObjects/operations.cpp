/** 
 * @file   operations.cc
 * @author BJ Tordoff
 * @date   02/11/2002		   		
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


#ifndef _operations_cc_
#define _operations_c_

#include <iostream>
#include "operations.h"

using namespace std;

VNL::MatrixFixed<3,3,double> VW::GetDerivativeRotationMatrix(const VW::Vector3D& axis,
							const double angle)
{
  VNL::MatrixFixed<3,3,double> result,result2;
  const double n=axis.Norm();
  if (n==0.0) {
    cerr << "VW::GetDerivativeRotationMatrix: Vector ("<<axis<<") is not a valid rotation axis." << endl;
    exit(0);
  }

  const double x = axis.GetX()/n;
  const double y = axis.GetY()/n;
  const double z = axis.GetZ()/n;

  if (angle>0.0) {
    const double st = sin(angle);
    const double ct = cos(angle);
    
    // First part is cos(theta)*Ax (a_x=cross product matrix of axis)
    result[0][0] =  0;    result[0][1] = -ct*z; result[0][2] =  ct*y;
    result[1][0] =  ct*z; result[1][1] =  0;    result[1][2] = -ct*x;
    result[2][0] = -ct*y; result[2][1] =  ct*x; result[2][2] =  0;
    
    // Second part is sin(theta)*(I - aa') (a=axis)
    result2[0][0] = st*(1-x*x);    result2[0][1] = -st*x*y;       result2[0][2] = -st*x*z;
    result2[1][0] = result2[0][1]; result2[1][1] = st*(1-y*y);    result2[1][2] = -st*y*z;
    result2[2][0] = result2[0][2]; result2[2][1] = result2[1][2]; result2[2][2] = st*(1-z*z);
    
    
    result -= result2;
  } else {
   // First part is cos(theta)*Ax (a_x=cross product matrix of axis)
    result[0][0] =  0; result[0][1] = -z; result[0][2] =  y;
    result[1][0] =  z; result[1][1] =  0; result[1][2] = -x;
    result[2][0] = -y; result[2][1] =  x; result[2][2] =  0;
  }

  return result;  
}



#endif
