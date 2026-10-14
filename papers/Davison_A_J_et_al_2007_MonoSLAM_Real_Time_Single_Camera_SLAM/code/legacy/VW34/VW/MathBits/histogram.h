/** @file histogram.h
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

#ifndef _histogram_h_
#define _histogram_h_

#include <assert.h>
#include <vector>

namespace VW {

/**
 * @class   HistogramLocation
 * @author  BJ Tordoff
 * @date    04/06/2004
 * @brief   A simple class for storing a location.
 *
 */
class HistogramLocation
{
public:
	double x, value;

	bool operator< (const HistogramLocation & l) const
	{
		return (value < l.value);
	}
};

/**
 * @class   Histogram
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    04/06/2004
 * @brief   A simple class for storing and retrieving 1D histogram values.
 *
 */
class Histogram
{
  friend std::ostream & operator<< (std::ostream &,
				    const VW::Histogram &);
public:
  /** Create uninitialised. */
  Histogram();

  /** Create with an array giving the axis values (tickmarks are the bin 
   * boundaries, so there is one less bin than tickmarks). */
  Histogram(const std::vector<double>& tickmarks);
  
  /** Create with bins equally spread. */
  Histogram(const double min_val,
	    const double max_val,
	    const unsigned int num_bins);
  
  ~Histogram ();
  
  /** Set the bin boundary locations (tickmarks are the bin 
   * boundaries, so there is one less bin than tickmarks).
   * All data will be lost. 
   */
  void SetTickMarks(const std::vector<double>& tickmarks);

  /** Add a piece of data to the histogram (amount is added to the 
   * appropriate bin) */
  void AddData(double xval, double amount=1.0);
  /** Add a set of data to the histogram (amount is added to the
   * appropriate bins) */
  void AddData(const std::vector<double>& xvals, double amount=1.0);

  /** Reset all bins to zero */
  void Clear();

  /** Number of bins. */
  unsigned int Size() const;

  /** Domain spanned by tick-marks (axis). */
  double Domain() const;
  /** Minimum axis value */
  double DomainMin() const;
  /** Maximum axis value */
  double DomainMax() const;

  /** Range spanned by bin contents. */
  double Range() const;
  /** Minimum bin height */
  double RangeMin() const;
  /** Maximum bin height */
  double RangeMax() const;

  double Tick (unsigned int which) const;

  /** Number of datapoints in this bin */
  double GetBinFrequency(unsigned int bin_index) const;
  /** Height is frequency/size */
  double GetBinHeight(unsigned int bin_index) const;

  /** Estimate the histogram value by bilinear interpolation. */
  double GetInterpValue (double axis_val) const;
  /** Get the histogram value of the closest bin. */
  double GetClosestValue (double axis_val) const;

  /** If any peaks are found they are stored in peaks, strongest first, 
   * otherwise return value is false. 
   * If max_num is non zero then only the strongest
   * max_num peaks are returned. */
	bool FindPeaks (std::vector < HistogramLocation > &peaks,
			unsigned int max_num = 0) const;

  /** If any troughs are found they are stored in peaks, otherwise
   * return value is false. */
	bool FindTroughs (std::vector < HistogramLocation > &troughs) const;

	operator      bool () const
	{
		return _data != 0;
	}
  /** Get a raw data element (same as H.GetData(i)). */
	double operator[] (int i)
	{
		return _data[i];
	}
  /** Get a raw data element (same as H.GetData(i)). */
	const double operator[] (int i) const
	{
		return _data[i];
	}

private:			// Funcs
	void AllocateData (unsigned int x);
	void DeallocateData ();

private:			// Data
	std::vector<double> _x_vals;
	double *_data;
};

std::ostream& operator<< (std::ostream& stream, const VW::Histogram& H);


}; // End namespace VW









// INLINES
inline double VW::Histogram::Tick (unsigned int which) const
{
	return _x_vals[which];
}

inline double
VW::Histogram::GetBinFrequency(unsigned int bin_index) const
{
    return _data[bin_index];
}

inline double
VW::Histogram::GetBinHeight(unsigned int bin_index) const
{
  return _data[bin_index]/(_x_vals[bin_index+1] - _x_vals[bin_index]);
}

inline unsigned int 
VW::Histogram::Size() const
{
    return unsigned(_x_vals.size());
}

inline double 
VW::Histogram::Domain() const
{
    assert(_x_vals.size() > 0);
    return _x_vals[_x_vals.size()-1] - _x_vals[0];
}

inline double 
VW::Histogram::DomainMin() const
{
    assert(_x_vals.size() > 0);
    return _x_vals[0];
}

inline double 
VW::Histogram::DomainMax() const
{
    assert(_x_vals.size() > 0);
    return _x_vals[_x_vals.size()-1];
}

inline double 
VW::Histogram::Range() const
{
    assert(_data != 0);
    const unsigned int N = unsigned(_x_vals.size()-1);
    double vmin=_data[0];
    double vmax=_data[0];
    for (unsigned int i=1;i<N;i++) {
      if (_data[i]>vmax) vmax=_data[i];
      if (_data[i]<vmin) vmin=_data[i];
    }
    return vmax-vmin;
}

inline double 
VW::Histogram::RangeMin() const
{
    assert(_data != 0);
    const unsigned int N = unsigned(_x_vals.size()-1);
    double vmin=_data[0];
    for (unsigned int i=1;i<N;i++) {
      if (_data[i]<vmin) vmin=_data[i];
    }
    return vmin;
}

inline double 
VW::Histogram::RangeMax() const
{
    assert(_data != 0);
    const unsigned int N = unsigned(_x_vals.size()-1);
    double vmax=_data[0];
    for (unsigned int i=1;i<N;i++) {
      if (_data[i]<vmax) vmax=_data[i];
    }
    return vmax;
}

inline void 
VW::Histogram::Clear()
{
  if (_data != 0) {
    const unsigned int n=unsigned(_x_vals.size()-1);
    for (unsigned int i=0;i<n;i++) _data[i]=0.0;
  }
}

#endif
