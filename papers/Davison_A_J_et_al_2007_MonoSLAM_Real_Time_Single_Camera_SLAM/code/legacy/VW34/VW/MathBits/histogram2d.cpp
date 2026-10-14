/** @file histogram2d.cpp
 *  @author  BJ Tordoff
 *  @date    04/06/2004
 *
 *              (c) Copyright University of Cambridge
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

#include <iostream>
#include <algorithm> // For sort etc
#include <cmath>
#include "histogram2d.h"

VW::Histogram2D::Histogram2D()
  : _data(0)
{
}

VW::Histogram2D::Histogram2D(const std::vector<double>& xvals,
			 const std::vector<double>& yvals)
  : _x_vals(xvals), _y_vals(yvals), _data(0)
{
  AllocateData(_x_vals.size(), _y_vals.size());
}

VW::Histogram2D::~Histogram2D()
{
  DeallocateData();
  _x_vals.clear();
  _y_vals.clear();
}

void VW::Histogram2D::SetAxes(const std::vector<double>& xvals,
			  const std::vector<double>& yvals)
{
  if (_data != 0) DeallocateData();

  _x_vals = xvals; 
  _y_vals = yvals;
  AllocateData(_x_vals.size(), _y_vals.size());
}

double VW::Histogram2D::GetInterpValue(double x_val, double y_val) const
{
  // First find bin just greater
  unsigned int index_x = 0, index_y = 0;
  double interp_x = 0.0, interp_y = 0.0;
  const unsigned int nx = _x_vals.size();
  if (_x_vals[0] > x_val) {
    // Requested a value off the side!
    std::cerr << "Histogram2D::GetInterpValue: Requested an X value off the side!\n";
    exit(0);
  } else {
    for (unsigned int i=1;i<nx;i++) {
      if (_x_vals[i] > x_val) {
	index_x = i-1;
	interp_x = (x_val - _x_vals[i-1]) / (_x_vals[i] - _x_vals[i-1]);
	break;
      }
    }
  }
  const unsigned int ny = _y_vals.size();
  if (_y_vals[0] > y_val) {
    // Requested a value off the side!
    std::cerr << "Histogram2D::GetInterpValue: Requested a Y value off the side!\n";
    exit(0);
  } else {
    for (unsigned int i=1;i<ny;i++) {
      if (_y_vals[i] > y_val) {
	index_y = i-1;
	interp_y = (y_val - _y_vals[i-1]) / (_y_vals[i] - _y_vals[i-1]);
	break;
      }
    }
  }
  // Some shorthand
  const double d00 = _data[index_y][index_x];
  const double d01 = _data[index_y][index_x+1];
  const double d10 = _data[index_y+1][index_x];
  const double d11 = _data[index_y+1][index_x+1];

  const double px = interp_x,     py = interp_y;
  const double qx = (1.0-interp_x), qy = (1.0-interp_y);

  return (  qy * qx * d00
	  + qy * px * d01
	  + py * qx * d10
	  + py * px * d11 );
}

double VW::Histogram2D::GetClosestValue(double x_val, double y_val) const
{
  unsigned int index_x = 0, index_y = 0;
  double best_diff = fabs(_x_vals[0] - x_val);
  const unsigned int nx = _x_vals.size();
  for (unsigned int i=1;i<nx;i++) {
    const double this_diff = fabs(_x_vals[i] - x_val);
    if (this_diff < best_diff) {
      best_diff = this_diff;
      index_x = i;
    }
  }
  const unsigned int ny = _y_vals.size();
  best_diff = fabs(_y_vals[0] - y_val);
  for (unsigned int i=1;i<ny;i++) {
    const double this_diff = fabs(_y_vals[i] - y_val);
    if (this_diff < best_diff) {
      best_diff = this_diff;
      index_y = i;
    }
  }
  return _data[index_y][index_x];
}

void VW::Histogram2D::AllocateData(unsigned int x, unsigned int y)
{
  assert(_data == 0);
  _data = new double*[y];
  for (unsigned int row=0;row<y;row++) _data[row] = new double[x];
}

void VW::Histogram2D::DeallocateData()
{
  if (_data != 0) {
    for (unsigned int row=0;row<_y_vals.size();row++) delete[] _data[row];
    delete[] _data;
    _data = 0;
  }
}



static VW::Histogram2DLocation
FitQuadratic3x3(const double data[3][3])
{
  // Fit a bi-quadratic patch to data, which is arranged row-first:
  //  data[0][0] data[0][1] data[0][2]
  //  data[1][0] data[1][1] data[1][2]
  //  data[2][0] data[2][1] data[2][2]
  // The values returned are offsets from the central position, 
  // which is taken to be 0,0.

//   std::cerr << data[0][0] << " " << data[0][1] << " " << data[0][2] << "\n";
//   std::cerr << data[1][0] << " " << data[1][1] << " " << data[1][2] << "\n";
//   std::cerr << data[2][0] << " " << data[2][1] << " " << data[2][2] << "\n";

  // Quadratic is calculated from a 3x3 grid around the point:
  //
  //             j-1    j    j+1
  //           -------------------
  //       sim |     |     |     |
  //           -------------------
  //       si  |     |     |     |
  //           -------------------
  //       sip |     |     |     |
  //           -------------------
  //
  // If the value at the central location is S(x0,y0), 
  // and at the maximum is S(xm,ym) the equation is:
  //
  //  S(x0+u,y0+u) = 0.5 * (u v) | A  C/2 | |u| + (D E) |u| + F
  //                             | C/2  B | |v|         |v|
  //
  // where:
  //
  //       d^2(S)       d^2(S)        d^2(S) 
  //   A = ------   B = ------   C = -------- 
  //       d(x)^2       d(y)^2       d(x)d(y)
  //
  //       d(S)         d(S)
  //   D = ----     E = ----     F = S(x,y)
  //       d(x)         d(y)
  //
  // evaluated at x=x0, y=y0.
  // 
  // The solution is then obtained using the fact that the gradient 
  // is zero at the maximum, ie:
  // 
  //  |u| = -1 * | A  C/2 |^(-1) | D |
  //  |v|        | C/2  B |      | E |

  // Gradients are calculated using the 1st and second order Sobel filters
  double A,B,C,D,E;

  A = (    data[0][0]  - 2*data[0][1]  +   data[0][2]
       + 2*data[1][0]  - 4*data[1][1]  + 2*data[1][2]
       +   data[2][0]  - 2*data[2][1]  +   data[2][2]  )/8.0; // d^2I / dx^2
  
  B = (    data[0][0]  + 2*data[0][1]  +   data[0][2]  
       - 2*data[1][0]  - 4*data[1][1]  - 2*data[1][2]  
       +   data[2][0]  + 2*data[2][1]  +   data[2][2]  )/8.0; // d^2I / dy^2
  
  C = (  + data[0][0]  - data[0][2] 
	 - data[2][0]  + data[2][2]  )/4.0;                   // d^2I / dxdy
  
  D = ( -   data[0][0] +   data[0][2]   
	- 2*data[1][0] + 2*data[1][2]   
	-   data[2][0] +   data[2][2]  )/8.0;                 // dI / dx
  
  E = ( - data[0][0]  - 2*data[0][1]  - data[0][2] 
	+ data[2][0]  + 2*data[2][1]  + data[2][2]  )/8.0;    // dI / dy

//  std::cerr << "[A,B,C,D,E]=["<<A<<","<<B<<","<<C<<","<<D<<","<<E<<"]\n";

  // Calculate the inverse of the determinant
  const double detinv = 1/(A*B - 0.25*C*C);
  
  // Calculate peak position and value
  VW::Histogram2DLocation peak;
  peak.x = -1* (B*D - 0.5*C*E) * detinv; // X-Offset of quadratic peak
  peak.y = -1* (A*E - 0.5*C*D) * detinv; // Y-Offset of quadratic peak
  
  // When the feature has weak gradient in one direction, bad shit
  // happens, but the most this fit should move the peak is half
  // way to a neighbouring pixel (otherwise it would be higher).
  if (peak.x>=0.5) {
//    std::cerr << "FitQuadratic3x3: Eeek! delta-x is " << peak.y << std::endl;
    peak.x=0.49999;
  }
  if (peak.x<-0.5) {
//    std::cerr << "FitQuadratic3x3: Eeek! delta-x is " << peak.y << std::endl;
    peak.x=-0.5;
  }
  if (peak.y>=0.5) {
//    std::cerr << "FitQuadratic3x3: Eeek! delta-y is " << peak.y << std::endl;
    peak.y=0.49999;
  }
  if (peak.y<-0.5) {
//    std::cerr << "FitQuadratic3x3: Eeek! delta-y is " << peak.y << std::endl;
    peak.y=-0.5;
  }
  peak.value =
  0.5*(A*peak.x*peak.x + B*peak.y*peak.y) + C*peak.x*peak.y // Quadratic part
     + D*peak.x        + E*peak.y                           // Linear part
     + data[1][1];                                          // Constant part

  return peak;
}

bool VW::Histogram2D::FindPeaks(std::vector<Histogram2DLocation>& peaks,
			    unsigned int max_num) const
{
  bool _got_a_peak = false;
  if (_data == 0) {
    return false;
  } else {
    const unsigned int nx = _x_vals.size(), ny = _y_vals.size();
    double vals[3][3];
    // Initially look only in the interior (don't consider maxima on the 
    // boundary)
    for (unsigned int y=1 ; y<(ny-1) ; ++y) {
      double* row_above_ptr = _data[(y-1)%ny]+1;
      double* row_ptr = _data[y]+1;
      double* row_below_ptr = _data[(y+1)%ny]+1;
      for (unsigned int x=1 ; x<(nx-1) ; ++x) {
	// Central columns
	vals[0][0] = row_above_ptr[-1]; 
	vals[0][1] = row_above_ptr[ 0]; 
	vals[0][2] = row_above_ptr[+1]; 
	vals[1][0] = row_ptr[-1]; 
	vals[1][1] = row_ptr[ 0]; 
	vals[1][2] = row_ptr[+1]; 
	vals[2][0] = row_below_ptr[-1]; 
	vals[2][1] = row_below_ptr[ 0]; 
	vals[2][2] = row_below_ptr[+1];
	if ((vals[1][1] > vals[0][0])
	    && (vals[1][1] > vals[0][1])
	    && (vals[1][1] > vals[0][2])
	    && (vals[1][1] > vals[1][0])
	    && (vals[1][1] > vals[1][2])
	    && (vals[1][1] > vals[2][0])
	    && (vals[1][1] > vals[2][1])
	    && (vals[1][1] > vals[2][2])) {
	  // Do a sub-pixel fit
	  Histogram2DLocation peak = FitQuadratic3x3(vals);
	  // And add the current offset
	  //  std::cerr << "Got a peak at index ("<<y<<","<<x<<")=("<<_y_vals[y]<<","<<_x_vals[x]<<"), delta=("<<peak.x<<","<<peak.y<<")\n";
	  if (peak.x > 0) {
	    const double dx = _x_vals[x+1] - _x_vals[x];
	    peak.x = _x_vals[x] + peak.x*dx;
	  } else {
	    const double dx = _x_vals[x] - _x_vals[x-1];
	    peak.x = _x_vals[x] + peak.x*dx;
	  }
	  if (peak.y > 0) {
	    const double dy = _y_vals[y+1] - _y_vals[y];
	    peak.y = _y_vals[y] + peak.y*dy;
	  } else {
	    const double dy = _y_vals[y] - _y_vals[y-1];
	    peak.y = _y_vals[y] + peak.y*dy;
	  }
	  peaks.push_back(peak);
	  _got_a_peak = true;
	  //	  std::cerr << "Peak angles ("<<peak.x<<","<<peak.y<<") with height "<<peak.value<<std::endl;
	}
	++row_above_ptr;
	++row_ptr;
	++row_below_ptr;
      }
    }
    if (_got_a_peak) {
      std::sort(peaks.begin(), peaks.end());
      // Sorting puts lowest first - we want highest
      std::reverse(peaks.begin(), peaks.end());
      if ((max_num>0) && (max_num<peaks.size())) {
	peaks.erase(peaks.begin()+max_num, peaks.end()); 
      }
    }
  }
  return _got_a_peak;
}

bool VW::Histogram2D::FindTroughs(std::vector<Histogram2DLocation>& troughs) const
{
  assert(false);
  return false;
}

std::ostream& VW::operator<<(std::ostream& stream, const VW::Histogram2D& H)
{
  const unsigned int nx = H.SizeX(), ny = H.SizeY();
  // First the x ticks
  stream << "xticks=[";
  for (unsigned int x=0 ; x<nx ; ++x) {
    stream << H.XTick(x) << " ";
  }
  stream << "];\n";

  // then the y ticks
  stream << "yticks=[";
  for (unsigned int y=0 ; y<ny ; ++y) {
    stream << H.YTick(y) << " ";
  }
  stream << "];\n";

  // then the histogram values
  stream << "H=[\n";
  for (unsigned int y=0 ; y<ny ; ++y) {
    for (unsigned int x=0 ; x<nx ; ++x) {
      stream << H[y][x] << " ";
    }
    stream << std::endl;
  }
  stream << "];" << std::endl;
  return stream;
}
