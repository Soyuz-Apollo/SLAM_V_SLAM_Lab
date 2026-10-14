/** 
 * @file   cornermatcher.h
 * @author BJ Tordoff
 * @date   14/01/2000
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
 * 2/12/04: Separated from cornermatcher.h
 *
 */
/**
 * @class   VW::MatchData
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    14/01/2000
 * @brief   A storage class for match data.
 * 
 * The match-data class stores one potential match, including
 * the list-index of the two points and the correlation score.
 */

#ifndef _matchdata_h
#define _matchdata_h

#include <iostream>
#include <vector>

namespace VW {

  class MatchData {
  public: // functions
    MatchData();
    MatchData(const unsigned int old_i, 
	      const unsigned int new_i, 
	      const float match_score=0,
	      const bool is_valid=true);
    
  unsigned int GetNewIndex() const;
  unsigned int GetOldIndex() const;
  float GetScore() const ;
  void SetScore(float new_score);

  // To enable us to sort corner lists
    bool operator> (const MatchData& m) const;
    bool operator< (const MatchData& m) const;
    bool operator== (const MatchData& m) const;

  friend std::ostream & operator << (std::ostream & s, const MatchData & m);
  friend std::istream & operator >> (std::istream & s, MatchData & m);
public: // Data
  unsigned int new_index;
  unsigned int old_index;
  float score;
  bool valid; // Is this a valid match, or a dummy
    
};


/** Sort into order of increasing match score. */
void SortAscending(std::vector<MatchData>& list);
/** Sort into order of decreasing match score. */
void SortDescending(std::vector<MatchData>& list);


  class Point2D;

  /** A utility function to make lists of matched point2d's from lists of
   * features and correspondence data
   */
template <class FeatureType>
inline void MatchesToPointLists(const std::vector<FeatureType>& old_features,
				const std::vector<FeatureType>& new_features,
				const std::vector<VW::MatchData>& matches,
				std::vector<VW::Point2D>& old_points,
				std::vector<VW::Point2D>& new_points)
{
  const unsigned int num_matches = matches.size();
  old_points.clear(); old_points.reserve(num_matches);
  new_points.clear(); new_points.reserve(num_matches);
  for (unsigned int m=0;m<num_matches;m++) {
    const VW::MatchData& match = matches[m];
    new_points.push_back(VW::Point2D(new_features[match.GetNewIndex()]));
    old_points.push_back(VW::Point2D(old_features[match.GetOldIndex()]));
  }
}

  /** A utility function to make lists of matched point2d's from lists of
   * features and correspondence data, including a list of the scores (priors)
   */
template <class FeatureType>
inline void MatchesToPointLists(const std::vector<FeatureType>& old_features,
				const std::vector<FeatureType>& new_features,
				const std::vector<VW::MatchData>& matches,
				std::vector<VW::Point2D>& old_points,
				std::vector<VW::Point2D>& new_points,
				std::vector<double>& scores)
{
  const unsigned int num_matches = matches.size();
  old_points.clear(); old_points.reserve(num_matches);
  new_points.clear(); new_points.reserve(num_matches);
  scores.clear(); scores.reserve(num_matches);
  for (unsigned int m=0;m<num_matches;m++) {
    const VW::MatchData& match = matches[m];
    new_points.push_back(VW::Point2D(new_features[match.GetNewIndex()]));
    old_points.push_back(VW::Point2D(old_features[match.GetOldIndex()]));
    scores.push_back(match.GetScore());
  }
}



}; // end namespace VW
 








    
inline VW::MatchData::MatchData() 
  : new_index(0), 
    old_index(0),
    score(0),
    valid(false) 
{
} 

inline VW::MatchData::MatchData(const unsigned int old_i, 
				const unsigned int new_i, 
				const float match_score,
				const bool is_valid)
  : new_index(new_i), 
    old_index(old_i),
    score(match_score),
    valid(is_valid)
{
}

inline unsigned int 
VW::MatchData::GetNewIndex() const 
{
  return(new_index);
};

inline unsigned int 
VW::MatchData::GetOldIndex() const 
{
  return(old_index);
};

inline float 
VW::MatchData::GetScore() const 
{
  return(score);
};

inline void 
VW::MatchData::SetScore(float new_score) 
{
  score=new_score;
};

// To enable us to sort corner lists
inline 
bool VW::MatchData::operator> (const VW::MatchData& m) const 
{
  return this->score > m.GetScore();
}

inline 
bool VW::MatchData::operator< (const VW::MatchData& m) const 
{
  return this->score < m.GetScore();
}

inline 
bool VW::MatchData::operator== (const VW::MatchData& m) const 
{
  return this->score == m.GetScore();
}

inline std::ostream& 
VW::operator<< (std::ostream & s, const VW::MatchData & m) 
{
  return s << m.new_index << " " << m.old_index;
}

inline std::istream& 
VW::operator>> (std::istream & s, VW::MatchData & m) 
{
  return s >> m.new_index >> m.old_index;
}

#endif
