/** 
 * @file   essentialmatrixcomputepreemptive.h
 * @author BJ Tordoff
 * @date   19/04/2005		   		
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
#ifndef _ematrixcomputepreemptive_h_
#define _ematrixcomputepreemptive_h_

#include <vector>

namespace VW {
  // Forward declare the transform
  class EssentialMatrix;
  class Point2D;

class EssentialMatrixComputePreemptive {
 public:
  public:
    EssentialMatrixComputePreemptive(double image_noise,
											  double search_area,
											  unsigned int max_num_hypotheses);
    
	 /** Compute with random sampling */
    bool Compute(VW::EssentialMatrix& E,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1,
					  std::vector<double>& posteriors);

	 /** Compute with guided sampling according to "priors" */
    bool Compute(VW::EssentialMatrix& E,
					  const std::vector<VW::Point2D>& x0,
					  const std::vector<VW::Point2D>& x1,
					  const std::vector<double>& priors,
					  std::vector<double>& posteriors);


    /** Set the expected image noise standard deviation (in pixels). 
     * This is used to determine the Gaussian model for the inlier process. */
    void SetImageNoise(double image_noise);

    /** Set the are searched for each feature. This determines the uniform
     * distribution used to model the outlier process. */
    void SetSearchArea(double search_area);

    /** Set the number of hypotheses. */
    void SetNumHypotheses(unsigned int num);

	 /** Set the maximum time in millisecs. If not set, not time limit
	  * is imposed.
	  */
	 void SetMaxTime(double millisecs);
 
  private:
    double _image_noise;
    double _search_area;
    unsigned int _num_hyp;
	 double _max_time;
		
  };

}; // End namespace VW;

 inline VW::EssentialMatrixComputePreemptive::
EssentialMatrixComputePreemptive(double image_noise,
									double search_area,
									unsigned int num_hypotheses)
  : _image_noise(image_noise),
    _search_area(search_area),
    _num_hyp(num_hypotheses),
	 _max_time(-1.0)
{
}

inline void VW::EssentialMatrixComputePreemptive::
SetImageNoise(double image_noise)
{
  _image_noise = image_noise;
}

inline void VW::EssentialMatrixComputePreemptive::
SetSearchArea(double search_area)
{
  _search_area = search_area;
}

inline void VW::EssentialMatrixComputePreemptive::
SetNumHypotheses(unsigned int num)
{
  _num_hyp = num;
}

inline void VW::EssentialMatrixComputePreemptive::
SetMaxTime(double millisecs)
{
  _max_time = millisecs;
}


#endif


