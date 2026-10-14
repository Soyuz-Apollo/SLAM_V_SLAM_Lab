/** 
 * @file   sequencermeteor.h
 * @author ID Reid, BJ Tordoff
 * @date   02/08/2002
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
 * @class   VW::SequencerMeteor
 * @ingroup Sequencers 
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/2002
 * @brief   A sequencer for handling the Matrox Meteor frame-grabber.
 */


#ifndef _SequencerMeteor_h_
#define _SequencerMeteor_h_ 

// System includes
#include <string>

// VW includes
#include "VW/image.h"
#include "VW/sequencers.h"

// Local includes
#include "ioctl_meteor.h"

#define VERBOSE false


namespace VW {

  /**
   * Grouping of the meteor-related enumerations.
   */
  namespace MeteorParams {
    /** 
     * The possible meteor modes.
     * Note that the meteor driver provides the values.
     */
    enum MeteorVideoMode {
      PAL   = METEOR_FMT_PAL,
      NTSC  = METEOR_FMT_NTSC,
      SECAM = METEOR_FMT_SECAM
    };
    /** 
     * The possible meteor input types.
     * Note that the meteor driver provides the values.
     */
    enum MeteorInputType {
      RCA    = METEOR_INPUT_DEV_RCA,
      SVIDEO = METEOR_INPUT_DEV_SVIDEO,
      RGB    = METEOR_INPUT_DEV_RGB
    };
  }

  class SequencerMeteor : public SequencerBase {

  public:
    SequencerMeteor();
    SequencerMeteor(MeteorParams::MeteorVideoMode m,
		    MeteorParams::MeteorInputType t,
		    unsigned int desired_width=0,
		    unsigned int desired_height=0,
		    unsigned int bits_per_pixel=32,
		    unsigned int number_of_frames=1,
		    bool use_fields=false);
    void Close();
    
   /**
    * Get a pointer to the raw buffer.
    * A raw pointer is returned, so it is up to the user to deal
    * with it before the meteor chooses to overwrite it.
    */
    inline unsigned char* GetBuffer(unsigned int which_channel=0);
    
    /** Take a copy of the grabbed image */
    template <class ImType>
    void CopyImage(ImType& image, unsigned int which_channel=0) {
      switch (input_type) {
      default:
      case MeteorParams::RGB:
	ImageConversions::Interlaced4Buffer_to_VWImage(GetCurrentMemoryBuffer(),
						       &image,
						       columns,rows,
						       x_increment,y_increment,
						       which_channel);
	break;
      case MeteorParams::RCA:
      case MeteorParams::SVIDEO:
	ImageConversions::BGRABuffer_to_VWImage(GetCurrentMemoryBuffer(),
						&image,
						columns,rows,
						x_increment,y_increment);
	break;
      }
    }

    // **** We overload 'Continuous', 'Stop' and  'Next'
    // as we don't require a seperate thread.
    void Continuous() { 
      if (!cameras_running) { 
	single_frame=false; 
	StartGrabbing(); 
	cameras_running=true; 
      }
    }
    void Stop() { 
      if (cameras_running) { 
	single_frame=false; 
	cameras_running=false; 
	StopGrabbing(); 
      } 
    }
    void Next() { 
      single_frame=true; 
      if (!cameras_running) {
	StartGrabbing(); 
	cameras_running=true;
      }; 
    }
  protected:
    
    // Overload the frame_arrived thingy so that single-capture 
    //stops the grabber

    /** we have been signalled that a frame has arrived and we are now "inside"
     * the signalling object, so invoke the appropriate callback
    */
    void _FrameArrived() 
    {
      // if in single-shot mode, then stop grabbing
      if (single_frame) Stop();
      NewFrameEvent(0l); // Event provided by SequencerBase
    }


    bool Init();
    bool StartGrabbing();
    bool StopGrabbing();
    virtual bool step();
    static SequencerMeteor *instantiation;

    // Memory buffer functions
    inline unsigned char *  GetCurrentMemoryBuffer()
    {
      return((unsigned char *)(memory_buffer + frame_offset.frame_offset[(num_frames<2)?0:meteor_memory->cur_frame-1]));
    }
    static inline void pthread_sighandler(int val) 
    {
      // We assume that the static variable instantiation has been set to
      // "this" for the calling object, and only one Sequencer instantiation is
      // allowed, so we know who the culprit is...
      instantiation->_FrameArrived();
    }


    unsigned char              *memory_buffer; // location of memory buffer
    std::string                 device_name;
    int                         file_descriptor;
    
    // Geometry settings
    unsigned int                maxcols, maxrows, maxfps;
    unsigned int                columns, rows, pixelsize;
    unsigned int                x_increment,y_increment;
    
  // The main user-definable params
    unsigned int                noframes;
    MeteorParams::MeteorVideoMode video_mode;
    MeteorParams::MeteorInputType input_type;
    unsigned int                bpp;
    unsigned int                num_frames;
    bool                        field_mode;
    
    // Stuff from the driver
    struct meteor_geomet        geometry;
    struct meteor_capframe      capture_frame;
    struct meteor_counts        counts;
    struct meteor_frame_offset  frame_offset;
    struct meteor_mem          *meteor_memory;
    
    // Other settings
    int                         bufsize;
    int                         signal_mode, 
    fps;
    int                         frame_num;
    bool                       cameras_running;
    bool                       single_frame;
};

}; // End namespace VW


// INLINES

inline unsigned char* VW::SequencerMeteor::GetBuffer(unsigned int which_channel) 
{
  return(GetCurrentMemoryBuffer()+which_channel);
}

#endif
