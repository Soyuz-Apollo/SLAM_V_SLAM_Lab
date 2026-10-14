/** 
 * @file   sequencerfirewire.h
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
 * @class   VW::SequencerFirewire
 * @ingroup Sequencers Hardware
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/2002
 * @brief A sequencer for handling firewire digital cameras (not DV!).
 *
 * The firewire sequencer uses the FirewireCamera class to provide a
 * sequencer-style interface to any digital cameras connected. Each
 * connected camera is given a channel, and at present it is assumed
 * that you want to use them all (ie. there is no facility for starting
 * and stopping particular cameras).
 */



#ifndef _SequencerFirewire_h_
#define _SequencerFirewire_h_

// System includes
#include <vector>

// VW includes
#include "VW/image.h"
#include "VW/sequencers.h"

// Local includes
#include "firewirecamera.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

namespace VW {


  /**
   * Retrieve a sequence of image from one or more firewire cameras.
   * This class implements the sequencer API for firewire cameras, providing
   * easy access to continuous or single-stepped image sequences. The camera
   * DMA is started when either 'Continuous' or 'Next' is first called
   * and is only stopped on object destruction (largely due to the fact it 
   * seg-faults when the DMA is stopped). Conversely the grabbing thread
   * is stopped when 'Stop' is called, so there should be no CPU overhead
   * to leaving DMA running.
   */
  class SequencerFirewire : public SequencerBase {

  public:
    SequencerFirewire(unsigned int x_reduction=1,
		      unsigned int y_reduction=1);
    virtual ~SequencerFirewire();

    /** 
     * Initialise the grabber.
     * This tells the grabber to scan for cameras and
     * initialise them. It must be called before trying to use the
     * sequencer.
     */
    virtual bool      Init();

  /** Stop continuous capture from the sequence (overloaded from base). */
  virtual void Stop() { 
    SequencerBase::Stop(); 
    // Note, must stop ISO after thread, otherwise thread never completes
    StopISO(); 
  }


    /** The total number of cameras attached to the Firewire BUS. */
    int                          GetNumberOfCameras(void) const;
    /** Returns a pointer to one camera. */
    virtual VW::FirewireCamera*  GetCamera(unsigned int which_camera);

    /**
     * Get a pointer to the raw buffer.
     * A raw pointer is returned, so it is up to the user to deal
     * with it before the firewire DMA chooses to overwrite it.
     * Wrapping an image around the bizarre format YUV images
     * that the firewire cameras use will almost certainly be
     * tricky. Note that the data is usually unsigned char
     */
    inline void* GetBuffer(unsigned int which_channel=0);

    /** 
     * Recover the image for one camera.
     * The image is copied from the raw image buffer into the VW image,
     * employing some conversion on the way. This is not a fast thing to
     * do. For absolute speed use the raw buffer.
     */
    template <class ImType>
    void CopyImage(ImType& image, unsigned int which_channel=0) {
	  if (VERBOSE) cerr << "VW::SequencerFirewire::CopyImage" << endl;
      char* camera_capture_buffer=(char *)(camera_capture_stuff[which_channel].capture_buffer);
	  if (VERBOSE) cerr << "   Get size" << endl;
      unsigned int width = camera_list[which_channel]->GetImageWidth();
      unsigned int height = camera_list[which_channel]->GetImageHeight();
      if (VERBOSE) cerr << "   Size=("<<width<<"x"<<height<<")" << endl;
      pthread_mutex_lock(&buffer_access_lock);
      switch(camera_list[which_channel]->GetImageMode())
	{
	case FirewireParams::M_160x120_YUV444:
	  if (VERBOSE) cerr << "   Grabbing: Mode is 160x120, YUV444" << endl;
	  ImageConversions::YUV444Buffer_to_VWImage(camera_capture_buffer,&image,
						    width,height,
						    x_subsamp, y_subsamp);
	  break;
	case FirewireParams::M_320x240_YUV422:
	case FirewireParams::M_640x480_YUV422:
	case FirewireParams::M_800x600_YUV422:
	case FirewireParams::M_1024x768_YUV422:
	case FirewireParams::M_1280x960_YUV422:
	case FirewireParams::M_1600x1200_YUV422:
	  if (VERBOSE) cerr << "   Grabbing: Mode is "<<width<<"x"<<height
			    <<", YUV422" << endl;
	  ImageConversions::YUV422Buffer_to_VWImage(camera_capture_buffer,&image,
						    width,height,
						    x_subsamp, y_subsamp);
	  break;
	case FirewireParams::M_640x480_YUV411:
	  if (VERBOSE) cerr << "   Grabbing: Mode is 640x480, YUV411" << endl;
	  ImageConversions::YUV411Buffer_to_VWImage(camera_capture_buffer,&image,
						    width,height,
						    x_subsamp, y_subsamp);
	  break;
	case FirewireParams::M_640x480_RGB:
	case FirewireParams::M_800x600_RGB:
	case FirewireParams::M_1024x768_RGB:
	case FirewireParams::M_1280x960_RGB:
	case FirewireParams::M_1600x1200_RGB:
	  if (VERBOSE) cerr << "   Grabbing: Mode is "<<width<<"x"<<height
			    <<", RGB" << endl;
	  ImageConversions::RGBBuffer_to_VWImage(camera_capture_buffer,&image,
						 width,height,
						 x_subsamp, y_subsamp);
	  break;
	case FirewireParams::M_640x480_MONO:
	case FirewireParams::M_800x600_MONO:
	case FirewireParams::M_1024x768_MONO:
	case FirewireParams::M_1280x960_MONO:
	case FirewireParams::M_1600x1200_MONO:
	  if (VERBOSE) cerr << "   Grabbing: Mode is "<<width<<"x"<<height
			    <<", MONO" << endl;
	  ImageConversions::MonoBuffer_to_VWImage(camera_capture_buffer,&image,
						  width,height,
						  x_subsamp, y_subsamp);
	  break;
	default:
	  pthread_mutex_unlock(&buffer_access_lock);
	  std::cerr << "VW::SequencerFirewire::CopyImage - unknown mode ("<<camera_list[which_channel]->GetImageMode()<<")\n";
	  exit(0);
	  return;
	};
      pthread_mutex_unlock(&buffer_access_lock);
 	  if (VERBOSE) cerr << "Grabbing: Done" << endl;
    // Set image's timestemp to the grab-time
      //image.SetTimeStamp(grabtime); 
    }

  protected:

    virtual bool _Step();

    // Data
    static raw1394handle_t                   raw_device_handle;    // Access to firewire bus
    static std::vector<VW::FirewireCamera*>  camera_list;          // All cameras
    static dc1394_cameracapture             *camera_capture_stuff; // DMA buffers etc.
    bool                                     cameras_running;      // Camera ISO running?
    bool                                     dma_running;          // Camera DMA running?
    int                                      frame_num;            // Current frame number
    unsigned int                             x_subsamp, y_subsamp; // Image subsampling


    // Buffer access is protected by a mutex lock
    pthread_mutex_t buffer_access_lock;

    // Initialisation
    virtual void      AddNewCamera(std::vector<VW::FirewireCamera*>& c_list,
				   unsigned int which_element);
    void              SetDefaultCameraSettings(void);
    void              ProbeForCameras();
    /** Setup the DMA transfer in each camera. 
     * Note that this doesn't start transferring images, just creates the channel.
     */
    bool              StartDMA();
    /** Stop the cameras running - ie. stop DMA.
     * At the moment this seems to be a fatal thing to do (ie. don't expect to
     * be able to restart tham having called this function). However, to stop 
     * the cameras carrying on after your program has finished, call it. */
    bool              StopDMA();

    /** Start isochronous image transfer. 
     * This begins the actual transfer of images from camera to memory.
     */
    bool              StartISO();
    /** Stop isochronous image transfer. 
     * This halts image transfer, and appears a safe thing to do.
     */
    bool              StopISO();
  };


}; // End namespace


/***************** INLINES *****************/

inline int VW::SequencerFirewire::GetNumberOfCameras(void) const
{
  return number_of_channels;
}

inline VW::FirewireCamera* 
VW::SequencerFirewire::GetCamera(unsigned int which_camera) 
{
  return(which_camera<number_of_channels ? camera_list[which_camera] : NULL);
};

inline void* VW::SequencerFirewire::GetBuffer(unsigned int which_channel) 
{
  return(static_cast<void*>(camera_capture_stuff[which_channel].capture_buffer));
}



#endif
