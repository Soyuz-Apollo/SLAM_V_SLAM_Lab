/** 
 * @file     trajectorymap.h
 * @author   BJ Tordoff
 * @date     02/11/2002
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
 * @class    VW::TrajectoryMap
 * @ingroup  GeometricObjects
 * @author   BJ Tordoff
 * @date     02/11/02
 * @brief    List structure designed to hold trajectories of poitns, lines etc.
 */


#ifndef _trajectorymap_h_
#define _trajectorymap_h_

#include <string>
#include <sstream>
#include <iostream>
#include <deque>
#include <list>
#include <vector>


namespace VW {


//////////////////// Single trajectory

// The trajectory class (a glorified circular buffer)
template <class Feature>
class Trajectory {
 public:
  Trajectory(unsigned int max_size=5,
	     unsigned int num_priors=0);
  Trajectory(const Feature& start_element,
	     const double propagation_score=0,
	     const std::vector<double>& priors = std::vector<double>(),
	     unsigned int max_size=5);
  Trajectory(const Trajectory<Feature>& t);
  
  ~Trajectory();

  Trajectory<Feature>& operator=(const Trajectory<Feature>& t);

  // Adding a feature (automatically truncates list)
  void Add(const Feature& new_feature,
	   const double propagation_score=0,
	   const std::vector<double>& priors=std::vector<double>());

  // Getting features off the list
  const Feature& GetOldestFeature(void) const;
  const Feature& GetNewestFeature(void) const;
  const Feature& GetOldFeature(const unsigned int age) const;
  double   GetPropagationScore(const unsigned int age=0) const;
  void     SetNumberOfPriors(const unsigned int num_priors);
  double   GetPrior(const unsigned int age=0,
		    const unsigned int which_prior=0) const;
  void     SetPrior(const unsigned int age,
		    const unsigned int which_prior,
		    const double new_score);

  /** The total number of frames for which this trajectory has existed. Note
   * that this may be longer than the trajectory length if (age>MaxSize).
   */
  unsigned int Age() const;

  /** The total number of frames stored. Note
   * that this may be less than the age if (age>MaxSize).
   */
  unsigned int size() const;

  /** Clear this trajectory. The buffer is emptied and age reset to zero.
   */
  void clear();

  // Mainly for debugging...
  
  std::string print() const;


 private:
  // Functions
  /** Increment the buffer pointers and wrap as appropriate. */
  void IncrementBufferPosition();

  // Data
  Feature*        _feature;
  double*         _propagation_score;
  std::vector<double>* _prior;
  unsigned int    _buffer_size;
  unsigned int    _buffer_pos;
  unsigned int    _total_age;
  unsigned int    _num_priors;
};

// (Inlines at end of file)







//////////////////// Trajectory map


// The trajectory map class (a glorified list of trajectories)
template <class Feature>
class TrajectoryMap : public std::list<Trajectory<Feature> > {
 public:
  TrajectoryMap(unsigned int max_trajectory_length=5) 
    : _maxtrajlen(max_trajectory_length) {};
  ~TrajectoryMap() {};
  
  // Some useful shortcuts
  
  /** Begin a new trajectory using 'new_element' as the starting point. 
  *
  */
  void StartNewTrajectory(const Feature& new_element,
			  const double propagation_score=0,
			  const std::vector<double>& priors = std::vector<double>());
  
  /** Begin a set of new trajectories using 'new_element' as their
   * starting points. This is especially useful for creating initial
   * trajectories from something like a corner list. The propagation
   * scores are initialised to zero.
   */
  void StartNewTrajectories(const std::vector<Feature>& new_elements);

  /** Begin a set of new trajectories using 'new_element' as their
   * starting points and with specified propagation scores. This is 
   * especially useful for creating initial
   * trajectories from something like a corner list. The length of
   * the vectors must be the same.
   */
  void StartNewTrajectories(const std::vector<Feature>& new_elements,
			    const std::vector<double>& propagation_scores);

  /** Retrieve the current ends of the trajectories as a vector. */
  void GetTrajectoryEnds(std::vector<Feature>& end_list) const;

  /** This function returns a list of pointers to all trajectories over a 
   * given length and their index within the list. It is useful for 
   * multi-view calculations. It is quite *SLOW* though, so use sparingly
   * (ie. not inside a RANSAC loop).
   */
  void GetOnesLongerThan(const unsigned int minimum_length, 
			 std::vector<const Trajectory<Feature>*>& features, 
			 std::vector<unsigned int>& indeces) const;

  // Mainly for debugging...
  
  std::string print() const;
  
 private:
  unsigned int _maxtrajlen;
};










// Inline functions given here, not in the .cc file

///////////////////////////////////////////////////////////

// Inlines for Trajectory

template <class Feature>
inline Trajectory<Feature>::Trajectory(unsigned int max_size,
				       unsigned int num_priors) 
  : _buffer_size(max_size),
    _buffer_pos(max_size-1),  // So that first addition makes it wrap_around
    _total_age(0),
    _num_priors(num_priors)
{
  _feature           = new Feature[_buffer_size];
  _propagation_score = new double [_buffer_size];
  _prior             = new std::vector<double>[_buffer_size];
};
template <class Feature>
inline Trajectory<Feature>::~Trajectory()  
{
  delete[] _feature;
  delete[] _propagation_score;
  delete[] _prior;
};
template <class Feature>
  inline Trajectory<Feature>::Trajectory(const Feature& start_element,
						const double propagation_score,
						const std::vector<double>& priors,
						unsigned int max_size)
  : _buffer_size(max_size),
    _buffer_pos(0),
    _total_age(1),
    _num_priors(priors.size())  
{
  _feature           = new Feature[_buffer_size];
  _propagation_score = new double [_buffer_size];
  _prior             = new std::vector<double>[_buffer_size];
  _feature[_buffer_pos]=start_element;
  _propagation_score[_buffer_pos]=propagation_score;
  _prior[_buffer_pos]=priors;
};
template <class Feature>
  inline Trajectory<Feature>::Trajectory(const Trajectory<Feature>& t)
  :  _buffer_size(t._buffer_size),
     _buffer_pos(t._buffer_pos),
     _total_age(t._total_age),
     _num_priors(t._num_priors)
{
  _feature = new Feature[_buffer_size];
  _propagation_score = new double[_buffer_size];
  _prior = new std::vector<double>[_buffer_size];
  for (unsigned int i=0;i<_buffer_size;i++) {
    _feature[i] = t._feature[i];
    _propagation_score[i] = t._propagation_score[i];
    _prior[i] = t._prior[i];
  }
}
template <class Feature>
inline void Trajectory<Feature>::Add(const Feature& new_feature,
				     const double propagation_score,
				     const std::vector<double>& priors) 
{
  IncrementBufferPosition();
  _feature[_buffer_pos] = new_feature;
  _propagation_score[_buffer_pos] = propagation_score;
  _prior[_buffer_pos] = priors;
  _total_age++;
}

template <class Feature>
inline const Feature& Trajectory<Feature>::GetOldestFeature() const 
{
  assert(_total_age>0);
  if (_total_age>_buffer_size)
    return (_feature[ (_buffer_pos + 1) % _buffer_size ]);
  else
    return (_feature[ _buffer_pos + 1 - _total_age ]);
};

template <class Feature>
inline const Feature& Trajectory<Feature>::GetNewestFeature() const 
{
  assert(_total_age>0);
  return (_feature[ _buffer_pos ]);
};

template <class Feature>
inline const Feature& Trajectory<Feature>::GetOldFeature(const unsigned int age) const 
{
  assert(age<_buffer_size);
  if (age>_buffer_pos)
    return (_feature[ _buffer_pos + _buffer_size - age ]);
  else
    return (_feature[ _buffer_pos - age ]);
}

template <class Feature>
inline void Trajectory<Feature>::IncrementBufferPosition()
{
  _buffer_pos++;
  _buffer_pos=(_buffer_pos % _buffer_size);
}

template <class Feature>
inline unsigned int Trajectory<Feature>::Age() const
{
  return(_total_age);
}

template <class Feature>
inline unsigned int Trajectory<Feature>::size() const
{
  if (_total_age<_buffer_size)
    return(_total_age);
  else
    return(_buffer_size);
}

template <class Feature>
inline void Trajectory<Feature>::clear()
{
  _total_age=0;
  _buffer_pos=_buffer_size-1;
}

template <class Feature>
inline Trajectory<Feature>& Trajectory<Feature>::operator=(const Trajectory<Feature>& t)
{
  if (this != &t) {
    _buffer_size=t._buffer_size;
    _buffer_pos=t._buffer_pos;
    _total_age=t._total_age;
    delete[] _feature;
    delete[] _propagation_score;
    _feature = new Feature[_buffer_size];
    _propagation_score = new double[_buffer_size];
    for (unsigned int i=0;i<_buffer_size;i++) {
      _feature[i] = t._feature[i];
      _propagation_score[i] = t._propagation_score[i];
    }
  }
  return(*this);
}
template <class Feature>
inline double   Trajectory<Feature>::GetPropagationScore(const unsigned int age) const
{
  assert(age<_buffer_size);
  if (age>_buffer_pos)
    return (_propagation_score[ _buffer_pos + _buffer_size - age ]);
  else
    return (_propagation_score[ _buffer_pos - age ]);
}

template <class Feature>
inline double   Trajectory<Feature>::GetPrior(const unsigned int age,
					      const unsigned int which_prior) const
{
  assert(age<_buffer_size);
  assert(which_prior<_num_priors);
  if (age>_buffer_pos)
    return (_prior[ _buffer_pos + _buffer_size - age ][which_prior]);
  else
    return (_prior[ _buffer_pos - age ][which_prior]);
}

template <class Feature>
inline void     Trajectory<Feature>::SetPrior(const unsigned int age,
					      const unsigned int which_prior,
					      const double new_score)
{
  assert(age<_buffer_size);
  assert(which_prior<_num_priors);
  if (age>_buffer_pos)
    _prior[ _buffer_pos + _buffer_size - age ][which_prior] = new_score;
  else
    _prior[ _buffer_pos - age ][which_prior] = new_score;
}

template <class Feature>
inline void     Trajectory<Feature>::SetNumberOfPriors(const unsigned int num_priors)
{
  if (num_priors!=_num_priors) {
    // Go through each vector in the buffer resizing
    for (unsigned int i=0;i<_buffer_size;i++)
      _prior[i].resize(num_priors);
    
    // If created new, blank them
    if (num_priors>_num_priors) {
      for (unsigned int i=0;i<_buffer_size;i++)
	for (unsigned int j=_num_priors;j<num_priors;j++)
	  _prior[i][j]=0;
    }
    _num_priors = num_priors;
  }
}










  // Inlines for TrajectoryMap


template <class Feature>
inline void TrajectoryMap<Feature>::StartNewTrajectory(const Feature& new_element,
						       const double propagation_score,
						       const std::vector<double>& priors) 
{
  this->push_back(Trajectory<Feature>(new_element,
				      propagation_score,
				      priors,
				      _maxtrajlen));
}

  template <class Feature>
  inline void TrajectoryMap<Feature>::StartNewTrajectories(const std::vector<Feature>& new_elements) {
    typename std::vector<Feature>::const_iterator i = new_elements.begin();
    for (;
	 i != new_elements.end();
	 i++) {
      this->push_back(Trajectory<Feature>((*i),0,std::vector<double>(),_maxtrajlen));
    }
  }
  template <class Feature>
  inline void TrajectoryMap<Feature>::StartNewTrajectories(const std::vector<Feature>& new_elements,
							   const std::vector<double>& propagation_scores) {
    assert(new_elements.size() == propagation_scores.size());
    typename std::vector<Feature>::const_iterator i = new_elements.begin();
    typename std::vector<double>::const_iterator j = propagation_scores.begin();
    for (;
	 i != new_elements.end();
	 i++,j++) {
      Trajectory<Feature> t((*i),(*j),std::vector<double>(),_maxtrajlen);
      this->push_back(t);
    }
  }

template <class Feature>
inline void TrajectoryMap<Feature>::GetTrajectoryEnds(std::vector<Feature>& end_list) const 
{
  for(typename TrajectoryMap<Feature>::const_iterator i=this->begin();
      i!=this->end();
      i++) {
    end_list.push_back(i->GetNewestFeature());
  }
}

template <class Feature>
inline void TrajectoryMap<Feature>::GetOnesLongerThan(const unsigned int minimum_length, 
						      std::vector<const Trajectory<Feature>*>& features, 
						      std::vector<unsigned int>& indeces) const 
{
  features.clear();
  features.reserve(this->size());
  indeces.clear();
  indeces.reserve(this->size());
  unsigned int index=0;
  for (typename TrajectoryMap<Feature>::const_iterator i=this->begin();
       i!=this->end();
       i++,index++) {
    if (i->Age()>=minimum_length) {
      features.push_back(&(*i));
      indeces.push_back(index);
    }	
  }

}

template <class Feature>
inline std::string Trajectory<Feature>::print() const
{
  std::ostringstream s;
  if (_total_age>0) {
    for (int i=int(_buffer_pos);i>=0;i--) {
      std::cerr << i << std::endl;
      s << "("<<_feature[i]<<") ";
    }
    if (_total_age>_buffer_size) {
      for (int i=int(_buffer_size-1);i>int(_buffer_pos);i--)
	s << "("<<_feature[i]<<") ";
    }
  } else {
    s << "(<no data>)";
  }
  return s.str();
}
template <class Feature>
inline std::string TrajectoryMap<Feature>::print() const
{
  std::ostringstream s;
  if (this->size()>0) {
    unsigned int index=0;
    for (typename TrajectoryMap<Feature>::const_iterator i=this->begin();
	 i!=this->end();
	 i++,index++) {
      std::cerr << "Tmap " << index << std::endl;
      s << i->print() << std::endl;
    }
  } else {
    s << "(<no data>)" <<std::endl;
  }
  return s.str();
}


} // end namespace VW

#endif /* _point2d_h */












