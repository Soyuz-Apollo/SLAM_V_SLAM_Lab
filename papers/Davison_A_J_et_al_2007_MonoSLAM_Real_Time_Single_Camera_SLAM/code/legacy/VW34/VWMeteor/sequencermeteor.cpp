/** 
 * @file   sequencermeteor.cc
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
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>

#include "sequencermeteor.h"

using namespace std;
using namespace VW;

SequencerMeteor::SequencerMeteor() :
  device_name("/dev/mmetfgrab0"),
  file_descriptor(-1),
  columns(0),
  rows(0),
  video_mode(MeteorParams::PAL),
  input_type(MeteorParams::RGB),
  bpp(32),
  num_frames(1),
  field_mode(false),
  cameras_running(false) {
  Init();
}
SequencerMeteor::SequencerMeteor(MeteorParams::MeteorVideoMode m,
				 MeteorParams::MeteorInputType t,
				 unsigned int desired_width,
				 unsigned int desired_height,
				 unsigned int bits_per_pixel,
				 unsigned int number_of_frames,
				 bool use_fields) :
  device_name("/dev/mmetfgrab0"),
  file_descriptor(-1),
  columns(desired_width),
  rows(desired_height),
  video_mode(m),
  input_type(t),
  bpp(bits_per_pixel),
  num_frames(number_of_frames),
  field_mode(use_fields),
  cameras_running(false) {
  Init();
}

bool SequencerMeteor::Init() {

  // Set a few parms based on the mode
  switch(video_mode) {
  case MeteorParams::NTSC:
    maxrows = 480;
    maxcols = 640;
    maxfps = 30;
    break;
  case MeteorParams::PAL:
    maxrows = 576;
    maxcols = 768;
    maxfps =  25;
    break;
  case MeteorParams::SECAM:
    maxrows = 576;
    maxcols = 768;
    maxfps  = 25;
    break;
  }

  // Step 1: Open the device cleanly ----------------------------------------

  if(file_descriptor != -1) {
    Close();
  }
  // met_fd is now -1 for sure 
  if ((file_descriptor = open(device_name.c_str(), O_RDWR)) < 0) {
    cerr << "SequencerMeteor::Init(): failed to open device " << device_name<< endl;
    return(false);
  }
  if (VERBOSE) cerr << "SequencerMeteor::Init(): opened device " << device_name<< endl;
  

  // Step 2: Work out appropriate geom etc -----------------------------------
  // First set values, then adjust if necessary
  signal_mode   = METEOR_SIG_FRAME;
  
  unsigned int desired_columns=columns;
  unsigned int desired_rows=rows;

   if (columns==0) {
     columns = maxcols;
     x_increment = 1;
   } else {
     x_increment = maxcols/columns;
     columns   = maxcols/x_increment;
   }
   if (rows==0) {
     rows = maxrows;
   } else {
     y_increment = maxrows/rows;
     rows      = maxrows/y_increment;
   }

   fps       = maxfps;

  if (input_type==MeteorParams::RGB) {
    // RGB cannot be resized in hardware, so only subsampling
    // is allowed. Meteor image remains max, but is sampled.
     number_of_channels=3;
   columns   = maxcols;
    rows      = maxrows;
    if (bpp==16) {
      pixelsize = 2;
    } else if (bpp==32 || bpp==24) {
      pixelsize = 4;
    } else {
      pixelsize = (bpp+1)>>3;
    }
  } else {
    // resizing can be done in hardware, but must be 4x3
     number_of_channels=1;
   rows = maxrows/x_increment;
    x_increment=y_increment=1;
    pixelsize = (bpp+1)>>3;
  }
  if ((columns!=desired_columns) || (rows!=desired_rows))
    cerr << "*** SequencerMeteor::Init(): Warning changed image size from ("<<desired_columns<<"x"<<desired_rows<<") to ("<<columns<<"x"<<rows<<")"<<endl;

  switch (pixelsize) {
  case 1:
    geometry.oformat = METEOR_GEO_YUV_PLANAR;
    break;
  case 2:
    geometry.oformat = METEOR_GEO_RGB16;
    break;
  case 3:
  case 4:
    geometry.oformat = METEOR_GEO_RGB24;
    break;
  default:
    cerr << "SequencerMeteor::Init(): Illegal pixel size " << pixelsize << endl;
    Close();
    return(false);
  }

  if (VERBOSE) cerr << "SequencerMeteor::Init(): Set pixelsize of " << pixelsize << endl;
  
  if (field_mode) {
    signal_mode   = METEOR_SIG_FIELD;
    rows         /= 2;
    num_frames    = (2*num_frames+1)/2;  /* ensure even number of fields */
  }
  
  geometry.rows     = rows;
  geometry.columns  = columns;
  geometry.frames   = num_frames;
  geometry.oformat |= field_mode;


  
  // Step 3: Now configure the meteor ---- -----------------------------------
  //
  
  if (ioctl(file_descriptor, METEORSETGEO, &geometry) < 0) {
    cerr << "SequencerMeteor::Init(): ioctl SetGeometry failed"<<endl;
    Close();
    return(false);
  }
  
  if (ioctl(file_descriptor, METEORGFROFF, &frame_offset) < 0) {
    cerr << "SequencerMeteor::Init(): ioctl FrameOffset failed"<<endl;
    Close();
    return(false);
  }
  
  if ((memory_buffer=(unsigned char *) mmap((caddr_t)0, frame_offset.fb_size,
					    PROT_READ|PROT_WRITE, 
					    MAP_FILE|MAP_SHARED, 
					    file_descriptor, (off_t)0)) == (unsigned char *)(-1)) {
    cerr << "SequencerMeteor::Init(): mmap failed"<<endl;
    Close();
    return(false);
  }
    if (VERBOSE) cerr << "SequencerMeteor::Init(): Memory mapped." << endl;

  int c = int(video_mode);
  if (ioctl(file_descriptor, METEORSFMT, &c) < 0) {
    cerr << "SequencerMeteor::Init(): ioctl SetFormat failed"<<endl;
    Close();
    return(false);
  }
  c = int(input_type);
  if (ioctl(file_descriptor, METEORSINPUT, &c) < 0) {
    perror("SequencerMeteor::Init(): ioctl Setinput failed");
    Close();
    return(false);
  }
  
  if (c==METEOR_INPUT_DEV_RGB && geometry.oformat==METEOR_GEO_RGB16) {
    /* set RGB section for 15 bit colour */
    u_short s = (0x4 << 4);
    if (ioctl(file_descriptor, METEORSBT254, &s)) {
      perror("SequencerMeteor::Init(): ioctl SetBt254 failed");
      Close();
      return(false);
    }
  }
    if (VERBOSE) cerr << "SequencerMeteor::Init(): Config OK." << endl;

  // Step 4: locate the control memory and zero frames_captured
  //
  meteor_memory = (struct meteor_mem * )(memory_buffer + frame_offset.mem_off);
  meteor_memory->frames_captured=0;

  return(true);
}

// get next image and send interrupt
bool SequencerMeteor::step() {
  bool success=true;
  if (!cameras_running) success=StartGrabbing();
  return (success);
}


bool SequencerMeteor::StartGrabbing(void)
{
  int c;
  fprintf(stderr, "meteor_start(): starting to grab with Meteor\n");
  
  // Use forms to attach signal
  // Seems unfortunate to have to use forms here, but ...
  //fl_add_signal_callback(SIGUSR2, &VW_GotFrame, 0);
  
  signal(SIGUSR2, &(SequencerMeteor::pthread_sighandler));

  c = SIGUSR2 | signal_mode;
  if (ioctl(file_descriptor, METEORSSIGNAL, &c) < 0) {
    perror("meteor_start():ioctl SetSignal failed");
    Close();
    return(false);
  }
  
  if (num_frames < 2) {
    /* continuous frame capture */
    c = METEOR_CAP_CONTINOUS ;
    if (ioctl(file_descriptor, METEORCAPTUR, &c)) {
      perror("meteor_start():ioctl CaptContinuous failed");
      Close();
      return(false);
    }
  } else {
    capture_frame.command = METEOR_CAP_N_FRAMES;
    capture_frame.lowat = 0;
    capture_frame.hiwat = 0;
    if (ioctl(file_descriptor, METEORCAPFRM, &capture_frame)) {
      perror("meteor_start():ioctl CaptFrm failed");
      Close();
      return(false);
    }
  }
  return(true);
}

bool SequencerMeteor::StopGrabbing(void)
{
  int c;
  cerr << "meteor_stop(): stopping grab with Meteor." << endl;
  
  // first disable application signals 
  c = 0;
  if (ioctl(file_descriptor, METEORSSIGNAL, &c) < 0) {
     cerr << "meteor_stop(): ioctl SetSignal failed." << endl;
    Close();
    return(false);
  }
  
  //
  if (geometry.frames < 2) {
    c = METEOR_CAP_STOP_CONT;
    if (ioctl(file_descriptor, METEORCAPTUR, &c)) {
      cerr << "meteor_stop(): ioctl CaptContinuous failed." << endl;
      Close();
      return(false);
    }
  } else {
    capture_frame.command = METEOR_CAP_STOP_FRAMES;
    if (ioctl(file_descriptor, METEORCAPFRM, &capture_frame)) {
      cerr << "meteor_stop(): ioctl CaptFrm failed." << endl;
      Close();
      return(false);
    }
  }
  return(true);
}

void SequencerMeteor::Close() {
  cerr << "SequencerMeteor::Close(): unmapping memory and closing device."<<endl;
  if(memory_buffer!=NULL) {
    munmap((char *)memory_buffer, frame_offset.fb_size); 
    memory_buffer = 0;
  }
  if(file_descriptor != -1) {
    close(file_descriptor);
    file_descriptor = -1;
  }
}
