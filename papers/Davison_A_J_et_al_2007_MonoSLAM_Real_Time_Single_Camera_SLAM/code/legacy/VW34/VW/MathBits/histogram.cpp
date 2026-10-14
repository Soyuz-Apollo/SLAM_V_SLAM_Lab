/** @file histogram.cpp
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
#include "histogram.h"

using namespace std;

VW::Histogram::Histogram()
  : _data(0)
{
}

VW::Histogram::Histogram (const double min_val,
			  const double max_val,
			  const unsigned int num_bins)
{
  const double bin_size = (max_val - min_val) / num_bins;
  _x_vals.resize(num_bins+1);
  _x_vals[0] = min_val;
  for (unsigned int i=1;i<num_bins;i++) {
    _x_vals[i] = min_val + i*bin_size;
  }
  _x_vals[num_bins] = max_val;
  AllocateData(num_bins);
}

VW::Histogram::Histogram(const std::vector<double>& ticks)
  : _x_vals(ticks), _data(0)
{
  AllocateData(unsigned(ticks.size()-1)); // one less bin than tickmarks!
}

VW::Histogram::~Histogram()
{
  DeallocateData();
  _x_vals.clear();
}

void VW::Histogram::SetTickMarks(const std::vector<double>& tickmarks)
{
  if (_data != 0) DeallocateData();

  _x_vals = tickmarks; 
  AllocateData(unsigned(_x_vals.size()-1)); // one less bin than tickmarks!
}

void VW::Histogram::AddData(double xval, double amount)
{
  // Find the right bin
  unsigned int index_x = 0;
  const unsigned int nx = unsigned(_x_vals.size());
  if ((xval < _x_vals[0])
      || (xval > _x_vals[_x_vals.size()-1])) {
    // Requested a value off the side, so ignore!
    return;
  }
  for (unsigned int i=1;i<nx;i++) {
    if (_x_vals[i] > xval) {
      index_x = i-1;
      break;
    }
  }
  _data[index_x] += amount;
}

void VW::Histogram::AddData(const std::vector<double>& xvals, 
			    double amount)
{
  std::vector<double>::const_iterator i = xvals.begin(), ie = xvals.end();
  for ( ; i != ie ; i++) AddData(*i,amount);
}

double VW::Histogram::GetInterpValue(double val) const
{
  // First find bin just greater
  unsigned int index_x = 0;
  double interp_x = 0.0;
  const unsigned int nx = unsigned(_x_vals.size());
  if (_x_vals[0] > val) {
    // Requested a value off the side!
    std::cerr << "Histogram::GetInterpValue: Requested an X value off the side!\n";
    exit(0);
  } else {
    for (unsigned int i=1;i<nx;i++) {
      if (_x_vals[i] > val) {
	index_x = i-1;
	interp_x = (val - _x_vals[i-1]) / (_x_vals[i] - _x_vals[i-1]);
	break;
      }
    }
  }

  // Some shorthand
  const double d0 = _data[index_x];
  const double d1 = _data[index_x+1];

  const double px = interp_x;
  const double qx = (1.0-interp_x);

  return (  qx * d0
	  + px * d1 );
}

double VW::Histogram::GetClosestValue(double val) const
{
  unsigned int index_x = 0;
  double best_diff = fabs(double(_x_vals[0] - val));
  const unsigned int nx = unsigned(_x_vals.size());
  for (unsigned int i=1;i<nx;i++) {
    const double this_diff = fabs(_x_vals[i] - val);
    if (this_diff < best_diff) {
      best_diff = this_diff;
      index_x = i;
    }
  }
  return _data[index_x];
}

void VW::Histogram::AllocateData(unsigned int x)
{
  assert(_data == 0);
  _data = new double[x];
  // Make sure it all starts at 0
  for (unsigned int i=0;i<x;i++) _data[i] = 0.0;
}

void VW::Histogram::DeallocateData()
{
  if (_data != 0) {
    delete[] _data;
    _data = 0;
  }
}



static VW::HistogramLocation
FitQuadratic3(const double data[3])
{
  // Quadratic is ax^2 + bx + c = v
  const double c = data[1]; // DC value
  const double b = 0.5*(data[2]-data[0]); // 1st derivative
  const double a = 0.5*(data[2]-2*data[1]+data[0]); // 2nd derivative


  // Calculate peak position and value
  VW::HistogramLocation peak;
  peak.x = -0.5*b/a; // Offset of quadratic peak
  
  // When the feature has weak gradient in one direction, bad shit
  // happens, but the most this fit should move the peak is half
  // way to a neighbouring pixel (otherwise it would be higher).
  if (peak.x>=0.5) {
    peak.x=0.49999;
  }
  if (peak.x<-0.5) {
    peak.x=-0.5;
  }
  peak.value = (a*peak.x + b)*peak.x + c;
  return peak;
}

bool VW::Histogram::FindPeaks(std::vector<HistogramLocation>& peaks,
			      unsigned int max_num) const
{
  bool _got_a_peak = false;
  if (_data == 0) {
    return false;
  } else {
    const unsigned int nx = _x_vals.size();
    double vals[3];
    // Initially look only in the interior (don't consider maxima on the 
    // boundary)
    double* row_ptr = _data+1;
    for (unsigned int x=1 ; x<(nx-1) ; ++x) {
      // Central columns
      vals[0] = row_ptr[-1]; 
      vals[1] = row_ptr[ 0]; 
      vals[2] = row_ptr[+1]; 
      if ((vals[1] > vals[0]) && (vals[1] > vals[2])) {
	// Do a sub-pixel fit
	HistogramLocation peak = FitQuadratic3(vals);
	// And add the current offset
	if (peak.x > 0) {
	  const double dx = _x_vals[x+1] - _x_vals[x];
	  peak.x = _x_vals[x] + peak.x*dx;
	} else {
	  const double dx = _x_vals[x] - _x_vals[x-1];
	  peak.x = _x_vals[x] + peak.x*dx;
	}
	peaks.push_back(peak);
	_got_a_peak = true;
      }
      ++row_ptr;
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
  return _got_a_peak;
}

bool VW::Histogram::FindTroughs(std::vector<HistogramLocation>& troughs) const
{
	cerr << "VW::Histogram::FindTroughs - sorry not implemented yet." << endl;
  assert(false);
  return false;
}

std::ostream& VW::operator<<(std::ostream& stream, const VW::Histogram& H)
{
  const unsigned int nx = H.Size();
  // First the ticks
  stream << "ticks=[";
  for (unsigned int x=0 ; x<nx ; ++x) {
    stream << H.Tick(x) << " ";
  }
  stream << "];\n";

  // then the histogram values
  stream << "H=[\n";
  for (unsigned int x=0 ; x<(nx-1) ; ++x) {
    stream << H[x] << " ";
  }
  stream << "];" << std::endl;
  return stream;
}
