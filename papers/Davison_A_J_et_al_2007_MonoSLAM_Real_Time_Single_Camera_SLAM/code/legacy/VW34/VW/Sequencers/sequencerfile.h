/** @file   sequencerfile.h 
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
 * @class   VW::SequencerFile
 * @ingroup Sequencers 
 * @ingroup CoreSequencers
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief   A sequencer for handling sequentailly ordered files.
 *
 * The file sequencer can take several ordered lists of files and present
 * each list as a channel within the sequencer paradigm. The sequence is
 * initialised using just the first frame, from which the numbering scheme
 * is deduced. The enumeration of the files must be using a zero-filled 
 * fixed-width field such as "some_name_00001.ppm". Channels can be added
 * in one go by passing a vector of filenames, or singly by repeatedly adding
 * a single filename. The netpbm package is used to allow loading of a wide
 * sleection of image types, of which jpeg,png,gif,ppm,pgm,eps are just a few.
 */


#ifndef _SequencerFile_h_
#define _SequencerFile_h_


// System includes
#include <string>
#include <vector>

// VW includes
#include <VW/Image/imagergb.h>
#include <VW/Image/imagecopy.h>

// Local includes
#include "sequencerbase.h"

namespace VW
{
  /** What should the sequencer do when it gets to the end of the sequence? */
  enum SequencerLoopMode
  {
    NO_LOOPING,///< Stop (and print an error to cerr if _Step() is called again)
    LOOP,      ///< Continue from the start of the sequence
    BOUNCE,    ///< Start playing the sequence backwards
    REPEATLAST ///< Continue to repeat the last frame
  };

class SequencerFile : public SequencerBase {

 public:
  /** Construct without initialising. */
  SequencerFile();
  /** Construct a single file-reader. */
  SequencerFile(const std::string& file_name);
  /** Construct multiple file-readers. */
  SequencerFile(const std::vector<std::string>& file_names);

  virtual bool Init() { return SequencerBase::Init(); }

  /** Add an extra file-channel.
   * Channels can be added at any time and simply begin from thier
   * first frame.
   */
  virtual void AddChannel(const std::string& file_name);

  /** Add a set of extra file-channels.
   * Channels can be added at any time and simply begin from thier
   * first frame.
   */
  void AddChannels(const std::vector<std::string>& file_names);

  /** Was the last read operation on this channel successful? */
  bool IsChannelOK(unsigned int which_channel=0) const;


  /** Gain access to the sequencers buffer */
  inline void* GetBuffer(unsigned int which_channel=0);

  /** Take a copy of the grabbed image */
  template <class ImType>
  void CopyImage(ImType& image, unsigned int which_channel=0) {
    if (which_channel < number_of_channels) {
      VW::ImageCopy(*im[which_channel],image);
    }
    else {
      std::cerr << "ERROR int Sequencer::CopyImage() : ";
      std::cerr << "requested non-existent channel" << std::endl;
    }
  }

  /** Rewind the grabbing of all movies to the beginning. */
  virtual void Rewind();

  /** Skip the next frame(s). */
  virtual void Skip(int num_frames=1);

  /** What should the sequencer do when it runs out of files in the sequence? */
  virtual void SetLooping(SequencerLoopMode mode=LOOP);

  /** The name(s) of the files. */
  std::string GetInitialFileName(unsigned int which_channel=0) {
    return(first_file_name[which_channel]);
  }
 protected:
  // Functions
  void _InitialiseFiles();
  bool _DetermineFilenamePatterns(unsigned int which_channel);
  virtual bool _Step();


  
  // Data
  std::vector<ImageRGB<unsigned char>* > im;
  
  std::vector<std::string> first_file_name;
  std::vector<std::string> format_string;
  std::vector<int> frame_num;
  std::vector<int> initial_frame_num;
  std::vector<int> play_speed;
  std::vector<bool> file_ok;
  SequencerLoopMode _loop;
};

} // End namespace VW

// INLINES
inline void* 
VW::SequencerFile::GetBuffer(unsigned int which_channel) 
{
  return(static_cast<void*>(im[which_channel]->GetRawBuffer()));
}

inline bool 
VW::SequencerFile::IsChannelOK(unsigned int which_channel) const
{
  if (which_channel>=file_ok.size()) return false;
  return file_ok[which_channel];
}

inline void 
VW::SequencerFile::AddChannels(const std::vector<std::string>& file_names)
{
  const unsigned int n=file_names.size();
  for (unsigned int i=0;i<n;i++)
    AddChannel(file_names[i]);
}

inline void VW::SequencerFile::SetLooping(SequencerLoopMode mode)
{
  _loop = mode;
}

#endif
