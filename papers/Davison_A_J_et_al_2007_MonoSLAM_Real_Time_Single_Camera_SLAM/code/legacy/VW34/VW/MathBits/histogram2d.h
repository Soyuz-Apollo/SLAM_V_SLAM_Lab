/** @file histogram2d.h
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

#ifndef _histogram2d_h_
#define _histogram2d_h_

#include <assert.h>
#include <vector>

namespace VW {

/**
 * @class   Histogram2DLocation
 * @ingroup MathsStuff
 * @author  BJ Tordoff
 * @date    04/06/2004
 * @brief   A simple class for storing a location.
 *
 */
class Histogram2DLocation
{
public:
	double x, y, value;

	bool operator< (const Histogram2DLocation & l) const
	{
		return (value < l.value);
	}
};

/**
 * @class   Histogram2D
 * @author  BJ Tordoff
 * @date    04/06/2004
 * @brief   A simple class for storing and retrieving 2D histogram values.
 *
 */
class Histogram2D
{
  friend std::ostream & operator<< (std::ostream &,
				    const Histogram2D &);
public:
  /** Create uninitialised. */
	  Histogram2D ();

  /** Create with arrays giving the axis values. */
	  Histogram2D (const std::vector < double >&xvals,
		       const std::vector < double >&yvals);

	 ~Histogram2D ();

	void SetAxes (const std::vector < double >&xvals,
		      const std::vector < double >&yvals);

  /** Number of bins in X direction. */
	unsigned int SizeX () const;
  /** Number of bins in Y direction. */
	unsigned int SizeY () const;

  /** Range spanned by Y-axis. */
	double RangeX () const;
  /** Range spanned by Y-axis. */
	double RangeY () const;

	double XTick (unsigned int which) const;
	double YTick (unsigned int which) const;
	double GetData (unsigned int x_index, unsigned int y_index) const;

  /** Estimate the histogram value by bilinear interpolation. */
	double GetInterpValue (double x_val, double y_val) const;
  /** Get the histogram value of the closest bin. */
	double GetClosestValue (double x_val, double y_val) const;

  /** If any peaks are found they are stored in peaks, strongest first, 
   * otherwise return value is false. 
   * If max_num is non zero then only the strongest
   * max_num peaks are returned. */
	bool FindPeaks (std::vector < Histogram2DLocation > &peaks,
			unsigned int max_num = 0) const;

  /** If any troughs are found they are stored in peaks, otherwise
   * return value is false. */
	bool FindTroughs (std::vector < Histogram2DLocation > &troughs) const;

	operator      bool () const
	{
		return _data != 0;
	}
  /** Note that direct array access (H[j][i]) is row-major, so is same as
   * H.GetData(i,j). */
	double *operator[] (int i)
	{
		return _data[i];
	}
	const double *operator[] (int i) const
	{
		return _data[i];
	}

private:			// Funcs
	void AllocateData (unsigned int x, unsigned int y);
	void DeallocateData ();

private:			// Data
	std::vector <double> _x_vals, _y_vals;
	double **_data;
};


std::ostream & operator<< (std::ostream & stream, const Histogram2D & H);

}; // End namespace VW

// INLINES

inline double
VW::Histogram2D::XTick (unsigned int which) const
{
	return _x_vals[which];
}

inline double 
VW::Histogram2D::YTick (unsigned int which) const
{
	return _y_vals[which];
}

inline double
VW::Histogram2D::GetData(unsigned int x_index,
                     unsigned int y_index) const
{
    return _data[y_index][x_index];
}

inline unsigned int 
VW::Histogram2D::SizeX() const
{
    return _x_vals.size();
}

inline unsigned int 
VW::Histogram2D::SizeY() const
{
    return _y_vals.size();
}
     
inline double 
VW::Histogram2D::RangeX () const
{
    assert(_x_vals.size() > 0);
    return _x_vals[_x_vals.size()-1] - _x_vals[0];
}

inline double 
VW::Histogram2D::RangeY() const
{
    assert (_y_vals.size() > 0);
    return _y_vals[_y_vals.size() - 1] - _y_vals[0];
}
#endif
