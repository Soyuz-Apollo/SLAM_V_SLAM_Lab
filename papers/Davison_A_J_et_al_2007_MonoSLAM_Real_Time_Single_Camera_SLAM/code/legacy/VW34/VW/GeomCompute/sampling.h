/** 
 * @file     sampling.h
 * @author   BJ Tordoff
 * @date     02/08/2002
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


#ifndef _sampling_h_
#define _sampling_h_

#include <math.h>
#include <iostream>
#include <vector>
#include <list>
#include <assert.h>

#define VERBOSE false

namespace VW {
  /**
   * Select 'number_to_pick' samples from a list using even random
   * samples. The list is assumed to be 'total_num_features' long, and
   * the resulting indices are placed in 'sample_indices'.
   */
  inline void RandomSample (unsigned int number_to_pick,
			    unsigned int total_num_features,
			    std::vector<unsigned int>& sample_indices);

  /**
   * Select 'number_to_pick' samples from a list using weighted
   * samples. The resulting indices are placed in 'sample_indices'.
   */
  inline void WeightedSample (unsigned int number_to_pick,
			      const std::vector<double>& cumulative_weights, 
			      std::vector<unsigned int>& sample_indices);

  /** Create a cumulative probability list from a probability list. */
  inline void AccumulateWeights(const std::vector<double>& weights,
				std::vector<double>& cumulative_weights,
				std::vector<unsigned int>* include_only=0);
  
  /** Create a random list of the numbers from 0 to (max_index-1)
	* where each number is gauranteed to be listed exactly once.
	*/
  void CreateRandomisedList(std::vector<unsigned int>& indices,
									 unsigned int max_index);

}



// INLINES

// {{{ VW::RandomSample ()

inline void VW::RandomSample (unsigned int number_to_pick,
			      unsigned int total_num_features,
			      std::vector<unsigned int>& sample_indices) {

  // This function contains two methods to find a set of samples
  // without replacement. The first was written by Joss and appears
  // to be slower than the second written by BJT. However, the one by
  // BJT can potentially stall and is only faster when 
  // total_num_features is *MUCH* bigger than number_to_pick.

  sample_indices.clear();
  sample_indices.reserve(number_to_pick);
  if (0) {
    // Joss method - use a list and delete entries when selected. The main
    // problem with this is that there is no quick way to jump to a given 
    // list position.

    // Create a list of all the indices
    std::list<unsigned int> l;
    //  l.reserve (total_num_features);  // Reserve not provided by list<>
    for (unsigned int i = 0; i < total_num_features; i++) l.push_back (i);
    
    
    for (unsigned int i = 0; i < number_to_pick; i++) {
      const unsigned int w = static_cast<unsigned int>(floor (l.size() * double (rand()) / (double(RAND_MAX) + 1.0)));
      // Find the 'w-th' element
      std::list<unsigned int>::iterator li=l.begin();
      for (unsigned int k=0;k<w;k++) li++;
      sample_indices.push_back(*li);
      if (VERBOSE) std::cerr << "Sample " << i << "=" << (*li) << std::endl;
      l.erase(li);
    }
  } else {
    // BJT use vectors but avoid the deletion problem by just resamping 
    // upon conflict (this causes it to be non deterministic time).
    for (unsigned int i = 0; i < number_to_pick;) {
      // choose an index 
      const unsigned int w = static_cast<unsigned int>(floor (total_num_features * double (rand()) 
			     / (double(RAND_MAX) + 1.0)));
      
      // check if the index is already listed
      bool listed=false;
      for (std::vector<unsigned int>::const_iterator j = sample_indices.begin();
	   j != sample_indices.end();
	   j++) 
	if ((*j) == w) { listed = true; break; }
      
      // If it's not already listed, list it and increment the 'found' counter
      if (!listed) {
	if (VERBOSE) std::cerr << "Sample " << i << "=" << w << std::endl;
	sample_indices.push_back(w);
	i++;
      }
    }
  }
}

// }}}

// {{{ VW::WeightedSample ()
inline void VW::WeightedSample (unsigned int number_to_pick,
				const std::vector<double>& cumulative_weights, 
				std::vector<unsigned int>& sample_indices) 
{
	assert(cumulative_weights.size() >= number_to_pick);
  double cum_total = cumulative_weights[cumulative_weights.size()-1];
  sample_indices.resize(number_to_pick);
  for (unsigned int sample_num=0;sample_num<number_to_pick;sample_num++) {
    // First, pick a number between 0 and the total cumulative weight
    double this_weight = cum_total * double (rand()) / double(RAND_MAX);
    
    // Now find the weight (binary search)
    int size_of_search_region=int(cumulative_weights.size());
    unsigned int low_limit=0;
    while(size_of_search_region>1) {
      unsigned int test_index=static_cast<unsigned int>(low_limit+(size_of_search_region/2)-1);
      if (cumulative_weights[test_index] > this_weight) {
	// weight is below test
	size_of_search_region=(test_index - low_limit + 1);
      } else {
	// weight is above test
	size_of_search_region-=(test_index - low_limit + 1);
	low_limit = test_index + 1;
      }
    }


      // Check if it's listed
      bool listed=false;
      for (std::vector<unsigned int>::const_iterator j = sample_indices.begin();
	   j != sample_indices.end();
	   j++) 
	if ((*j) == low_limit) { listed = true; break; }
      // if it is, decrement the counter so it's repeated, otherwise store
      if (listed) {
	sample_num--;
      } else {
	sample_indices[sample_num] = low_limit;
      }
    }

}
// }}}

// {{{ VW::AccumulateWeights ()
inline void VW::AccumulateWeights(const std::vector<double>& weights,
				  std::vector<double>& cumulative_weights,
				std::vector<unsigned int>* include_only) {

  if (include_only==0) { // use all weights
    cumulative_weights.resize(weights.size());
    double c=0;
    std::vector<double>::iterator j = cumulative_weights.begin();
    for (std::vector<double>::const_iterator i = weights.begin();
	 i != weights.end();
	 i++,j++) {
      c += (*i);
      (*j) = c;
    }
  } else { // use only selected weights
    cumulative_weights.resize(include_only->size());
    double c=0;
    std::vector<double>::iterator j = cumulative_weights.begin();
    for (std::vector<unsigned int>::const_iterator i = include_only->begin();
	 i != include_only->end();
	 i++,j++) {
      c += weights[*i];
      (*j) = c;
    }
  }
}
// }}}

#endif
