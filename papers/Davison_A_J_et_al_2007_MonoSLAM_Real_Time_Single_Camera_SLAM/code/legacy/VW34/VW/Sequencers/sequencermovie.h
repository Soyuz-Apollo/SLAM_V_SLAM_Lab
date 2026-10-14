/** @file   sequencermovie.h 
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
 * @class   VW::SequencerMovie
 * @ingroup Sequencers 
 * @ingroup CoreSequencers
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   A sequencer for handling a few types of Movie file.
 *
 * As this uses a hacked version of xanim, you'll need to get a copy. This
 * sequencer can read images from any movie that xanim can handle.
 */


#ifndef _SequencerMovie_h_
#define _SequencerMovie_h_

// System includes
#include <string>
#include <vector>

// VW includes
#include <VW/Image/imagergb.h>
#include <VW/Image/imagecopy.h>

// Local includes
#include "sequencerbase.h"

namespace VW {

class SequencerMovie : public SequencerBase {

 public:
  SequencerMovie(); 
  SequencerMovie(const std::string& filename); 
  SequencerMovie(const std::vector<std::string>& filenames); 

  void AddChannel(const std::string& file_name);

  /** Gain access to the sequencers buffer */
  inline void* GetBuffer(unsigned int which_channel=0);

  /** Take a copy of the grabbed image */
  template <class ImType>
  void CopyImage(ImType& image, unsigned int which_channel=0) {
    VW::ImageCopy(*im[which_channel],image);
  }

  /** Rewind the grabbing of all movies to the beginning. */
  virtual void Rewind();

  /** The name(s) of the movies. */
  std::string GetMovieName(unsigned int which_channel=0) {
    return(movienames[which_channel]);
  }

  /** The pid(s) of the y4mtoppm processes */
  int GetY4MPID(unsigned int which_channel=0) {
    return(y4m_pid[which_channel]);
  }

  /** remove temporary files created for mplayer and y4mtoppm */
  void _cleanup(int which_channel);

 protected:
  void _InitialiseMovies();
  void _OpenFile(unsigned int which_channel);
  virtual bool _Step();

  std::vector<ImageRGB<unsigned char>* > im;
  std::vector<FILE*> fp;
  std::vector<std::string> movienames;
  std::vector<bool> file_ok;
  std::vector<int> y4m_pid, mplayer_pid;
  std::vector<char *> streamname;

};

};

// INLINES
inline void* 
VW::SequencerMovie::GetBuffer(unsigned int which_channel) 
{
  return(static_cast<void*>(im[which_channel]->GetRawBuffer()));
}


#endif
