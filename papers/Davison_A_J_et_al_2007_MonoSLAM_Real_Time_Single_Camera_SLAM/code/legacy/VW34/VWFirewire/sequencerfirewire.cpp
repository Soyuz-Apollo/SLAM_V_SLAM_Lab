/** 
 * @file   sequencerfirewire.cc
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



#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sys/time.h>
#include <libdc1394/dc1394_control.h>
#include <libraw1394/raw1394.h>

// Image classes and conversion functions
#include "VW/image.h"

// Inlcude the camera description class (grabber owns cameras)
#include "firewirecamera.h"

// Class definition
#include "sequencerfirewire.h"

using namespace std;
using namespace VW;


// Static stuff (threads needs access, so has to be static)
raw1394handle_t              VW::SequencerFirewire::raw_device_handle;
vector<VW::FirewireCamera*>  VW::SequencerFirewire::camera_list;
dc1394_cameracapture        *VW::SequencerFirewire::camera_capture_stuff;



// {{{ SequencerFirewire()

VW::SequencerFirewire::SequencerFirewire(unsigned int x_reduction,unsigned int y_reduction) :
  cameras_running(false),
  dma_running(false),
  frame_num(0),
  x_subsamp(x_reduction),
  y_subsamp(y_reduction) {
  // Set number of cams to zero so that we know it's not initialised
  number_of_channels=0;
}

// }}}
// {{{ ~SequencerFirewire()

SequencerFirewire::~SequencerFirewire() {
  Stop();
  StopISO();
  // This last one tends to be fatal anyway...
  StopDMA();
};

// }}}

// get next image and send interrupt

bool SequencerFirewire::_Step()
{
  if (VERBOSE) cerr << "SequencerFirewire::step" << endl;
  if (!cameras_running) {
    if (!StartISO()) {
      cerr << "SequencerFirewire::step - Failed to start cameras." << endl;
      return(false);
    }
  }

  if (VERBOSE) cerr << "SequencerFirewire::step - lock buffer" << endl;
  // Lock the DMA buffer
  pthread_mutex_lock(&buffer_access_lock);


  // If in single step mode, free the current DMA buffer to force
  // grabbing of the current image
//   if (!cont)
//    for (unsigned int cam_num = 0; cam_num < number_of_channels; cam_num++) {
//      // Shuffle back one
//      int buf_num = camera_capture_stuff[cam_num].dma_last_buffer;
//      camera_capture_stuff[cam_num].dma_last_buffer--;
//      if (camera_capture_stuff[cam_num].dma_last_buffer<0) 
//        camera_capture_stuff[cam_num].dma_last_buffer = 
// 	 camera_capture_stuff[cam_num].num_dma_buffers - 1;
//      // Free the buffer
//      dc1394_dma_done_with_buffer(&(camera_capture_stuff[cam_num]));
//      // move back to current
//      camera_capture_stuff[cam_num].dma_last_buffer = buf_num;
//   }
 


  if (VERBOSE) cerr << "SequencerFirewire::step - capture from "<<number_of_channels<<" channel(s)." << endl;
  // Do a single multi-capture
  dc1394_dma_multi_capture(camera_capture_stuff, number_of_channels);
  
  // Free up a buffer
  if (VERBOSE) cerr << "SequencerFirewire::step - free buffers" << endl;
  for (unsigned int cam_num = 0; cam_num < number_of_channels; cam_num++) {
    dc1394_dma_done_with_buffer(&(camera_capture_stuff[cam_num]));
  }
  
  // For steps, repeat to get red of the shiny stuff
  if (!cont)
  for (unsigned int i=0; i<2; i++) {
    dc1394_dma_multi_capture(camera_capture_stuff, number_of_channels);
    for (unsigned int cam_num = 0; cam_num < number_of_channels; cam_num++) {
      dc1394_dma_done_with_buffer(&(camera_capture_stuff[cam_num]));
    }
  
  }
  
  
  // Unlock the DMA buffer
  if (VERBOSE) cerr << "SequencerFirewire::step - unlock buffers" << endl;
  pthread_mutex_unlock(&buffer_access_lock);

  if (!thread_running) StopISO();

  frame_num++;
  if (VERBOSE) cerr << "SequencerFirewire::step - done" << endl;
  return(true);
}


// Initialisation functions

// This dummy function just exists to be overloaded!
// {{{ bool Init()

bool VW::SequencerFirewire::Init() {

  // Do the general Init
  SequencerBase::Init();

  if (VERBOSE) cerr << "SequencerFirewire::Init: Probe..." << endl;
  ProbeForCameras();
  if (VERBOSE) cerr << "SequencerFirewire::Init: Set defaults..." << endl;
  SetDefaultCameraSettings();
  if (VERBOSE) cerr << "SequencerFirewire::Init: Done." << endl;
  return(true);
}

// }}}
// {{{ ProbeForCameras()
void VW::SequencerFirewire::ProbeForCameras()
{
  // Check that the array is curently empty
  if (number_of_channels != 0) {
    if (VERBOSE)
      cerr << "SequencerFirewire::GetCameras() - Already got some!" 
	   << endl;
  }



  // Find the handle for the first firewire card.
  // port 0 is the first IEEE1394 card. We ONLY probe this one.
  int port = 0;
  nodeid_t *camera_nodes;
  raw_device_handle = dc1394_create_handle(port);
  if (raw_device_handle == 0) {
    if (VERBOSE)
      cerr << "SequencerFirewire::GetCameras - No cams found" << endl;
  }

  // Find out how many cameras there are on this handle
  // (this is stored in 'n', and passing the final zero prevents
  // lots of printout)
  int n=0;
  camera_nodes = dc1394_get_camera_nodes(raw_device_handle, &n, 0);
  number_of_channels=static_cast<unsigned int>(n);
  if (VERBOSE)
    cerr << "SequencerFirewire::GetCameras - Found " << number_of_channels
	 << " cameras." << endl;
  
  for (unsigned int i=0;i<number_of_channels;i++) {
    cerr << "Node "<<i<<": ID="<<camera_nodes[i]<<endl;
  }

  // Create an array of digicams and their buffers
  camera_list.resize(number_of_channels);
  for (unsigned int i = 0; i < number_of_channels; i++) {
    AddNewCamera(camera_list,i);
    camera_list[i]->SetNodeNumber(camera_nodes[i]);
    camera_list[i]->SetUpCamera(raw_device_handle);

    // Check if the camera is currently running, and if so stop it
    if (camera_list[i]->IsISOTransmitting()) {
      cerr << "VW::SequencerFirewire::ProbeForCameras() - "
	   << "cameras "<<i<<" appears to be on, so stop it."<<endl;
      camera_list[i]->StopISOTransmission();
    }

    if (VERBOSE) {
      cerr << "Camera added, node ID="<<camera_list[i]->GetNodeNumber() <<endl;
      cerr << "Camera " << i 
	   << ": Vendor=" << camera_list[i]->GetVendorString() 
	   << " Model=" << camera_list[i]->GetModelString() << endl;
    }
  }
  free(camera_nodes);
  
  // Also create the  buffers and capture space for each camera
  camera_capture_stuff = new dc1394_cameracapture[number_of_channels];


}

// }}}
// {{{ AddNewCamera()

void VW::SequencerFirewire::AddNewCamera(std::vector<VW::FirewireCamera*>& c_list,
				       unsigned int which_element) {
  c_list[which_element]=new FirewireCamera(VERBOSE);
}

// }}}
// {{{ SetDefaultCameraSettings()

void VW::SequencerFirewire::SetDefaultCameraSettings(void)
{
    for (unsigned int i = 0; i < number_of_channels; i++) {
	camera_list[i]->SetImageMode(FirewireParams::M_320x240_YUV422);
	camera_list[i]->SetFramerate(FirewireParams::FPS_15);
    }
}

// }}}

// Setup and run or stop the DMA
// {{{ StartDMA(void)

bool VW::SequencerFirewire::StartDMA(void)
{
    dma_running=false;
  // returns true for success, false for failure
  cerr << "SequencerFirewire::StartDMA()" << endl;

  // Initialise the mutex lock for the image buffers
     if (VERBOSE)
       cerr << "SequencerFirewire::StartDMA - Lock thread " << endl;
  pthread_mutex_init(&buffer_access_lock, NULL);
  
  // Start DMA for each camera
     if (VERBOSE)
       cerr << "SequencerFirewire::StartDMA - Try to start DMA " << endl;
  for (unsigned int i = 0; i < number_of_channels; i++) {
    
    if (camera_list[i]->SetupDMACapture(&camera_capture_stuff[i])) {
      if (VERBOSE)
	cerr << "SequencerFirewire::StartDMA - setup DMA " << i <<
	  endl;
    } else {
	cerr <<
	  "SequencerFirewire::StartDMA - Could not setup DMA " <<
	  i << endl;
      return (false);
    }
    int frame_width = camera_capture_stuff[i].frame_width;
    int frame_height = camera_capture_stuff[i].frame_height;
    if (VERBOSE)
      cerr << "SequencerFirewire::StartDMA - Mode is " <<
	frame_width << "x" << frame_height << endl;

    dma_running=true;
  }
  cerr << "SequencerFirewire::StartDMA - Done" << endl;
 
  return (true);
}

// }}}
// {{{ StopDMA(void)

bool VW::SequencerFirewire::StopDMA(void)
{
  cerr << "VW::SequencerFirewire::StopDMA()"<<endl;
  // Go through cameras, and if they're connected, stop transmission
  bool stop_succeeded = true;
  for (unsigned int i = 0; i < number_of_channels; i++)
  {
    if (!camera_list[i]->ReleaseDMACapture(&camera_capture_stuff[i]))
    {
      cerr << "SequencerFirewire::StopDMA - could not release ";
      cerr << "DMA channel for camera " << i << endl;
      stop_succeeded = false;
    } 
  }
  if(stop_succeeded)
  {
    if(VERBOSE)
    {
      cerr << "SequencerFirewire::StopDMA - successfully stopped ";
      cerr << "DMA for all cameras\n";
    }
    dma_running=false;
  }
  return (stop_succeeded);
}

// }}}
// {{{ StartISO(void)

bool VW::SequencerFirewire::StartISO(void)
{
  // no point setting up capture if DMA is not running...
  if (!dma_running) {
    if (!StartDMA()) return false;
  }
  // returns true for success, false for failure
  cerr << "SequencerFirewire::StartISO()" << endl;

  // Go through cameras, and if they're connected, start transmission
  for (unsigned int i = 0; i < number_of_channels; i++) {
    if (!camera_list[i]->StartISOTransmission())
      return (false);
  }
  cameras_running=true;
  cerr << "SequencerFirewire::StartISO() - done" << endl;

  return (true);
}

// }}}
// {{{ StopISO(void)

bool VW::SequencerFirewire::StopISO(void)
{
  if (VERBOSE) cerr << "VW::SequencerFirewire::StopCameras()"<<endl;
  // Go through cameras, and if they're connected, stop transmission
  bool stop_succeeded = true;
  for (unsigned int i = 0; i < number_of_channels; i++) {
    if (!camera_list[i]->StopISOTransmission()) {
      cerr << "SequencerFirewire::StopGrabbing - could not stop ";
      cerr << "ISO Transmission for camera " << i << endl;
      stop_succeeded = false;
    }
  }
  if (stop_succeeded && VERBOSE) {
    cerr << "SequencerFirewire::StopCameras - successfully stopped ";
    cerr << "ISO Transmission for all cameras\n";
  }
  StopDMA();
  cameras_running=false;
  if (VERBOSE) cerr << "VW::SequencerFirewire::StopCameras() - done"<<endl;
  return (stop_succeeded);
}

// }}}
