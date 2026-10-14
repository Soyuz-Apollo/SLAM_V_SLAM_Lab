/** @file   sequencerbase.h 
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
 * @class   VW::SequencerBase 
 * @ingroup Sequencers
 * @ingroup CoreSequencers
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/2002
 * @brief   The base class from which all sequencers are made.
 *
 * All sequencers must provide a mechanism for moving on a single frame,
 * continuously getting frames and stopping. A frame may consist of several
 * channels, but the frame is not presented to the user until all channels
 * have provided an image (ie. until the slowest device responds). When a new
 * frame is ready the NewFrameEvent signal is emitted and the user can retrieve
 * images using the CopyImage function that all sequencers provide.
 *
 * For many reasons (mainly hardware related) it is usually only possible
 * to instantiate a single sequencer. Nothing in this base class prevents it,
 * but bad stuff will probably happen.
 *
 * NB: You *must* call the Init() function of a sequencer before use.
 */


#ifndef _SequencerBase_h_
#define _SequencerBase_h_

#include <VW/events.h>
#include "thread.h"

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE false

namespace VW {

class SequencerBase : private VW::Thread {

 public:

  SequencerBase();
  virtual ~SequencerBase() {};

  /** @name Sequence control functions */ //@{

  /** Start continuous capture from the sequence. */
  virtual void Continuous();

  /** Stop continuous capture from the sequence. */
  virtual void Stop();

  /** Capture the next single frame. */
  virtual void Next();

  //@}


  /** @name Initialisation functions */
  //@{

  /** All sequencers have an Init(), even if they don't need it, 
   * so call init() before using a sequencer (especially if from an
   * interface).
   */
  virtual bool Init();

  /** Set the delay between frames (only usually used for non-hardware
  sequencers). This is the time (in milliseconds) for which
  the sequencer sleeps between frames: the total frame period will be this plus
  the time taken to acquire the new frame. By default the delay is set to 1ms
  (zero or less is not a valid setting).*/
  virtual void SetDelay(int d);

  //@}



  /** @name Information functions */
  //@{

  /** Is the sequencer running in continuous mode? */
  inline bool          GetContinuous() const {return(cont);}

  /** What is the delay between frames */
  inline int           GetDelay() const;

  /** Most sequencers are able to present mutliple images per frame. */
  inline unsigned int  GetNumberOfChannels() const;

  /** Return the current frame number. 
   * In the case of movies or files this is the frame number
   * with respect to the start frame (ie, 1=start frame) and
   * won't correspond to the sequence frame numbers if started
   * part way through a sequence.
   */
  inline unsigned int  GetCurrentFrameNumber() const;

  //@}




  /** @name Image access functions*/
  //@{

  /** Take a copy of the grabbed image. */
   template <class ImType>
   void CopyImage(ImType& image, unsigned int which_channel=0) = 0;

  /** Get a pointer to the raw buffer. */
  virtual void* GetBuffer(unsigned int which_channel=0) = 0;

  //@}






  /** The "new frame has arrived" event.
   * Attach a function to this callback signal if you want to perform
   * an operation each time a new image arrives. This is typically done 
   * using <TT>my_sequencer.NewFrameEvent.connect(&MyFunction);</TT> 
   * where 'MyFunction' must take an integer argument and return void.
   */
  VWEvents::Event1<void,int> NewFrameEvent;


 protected:

  /** Wait for the thread to exit */
  inline void _WaitForThread();

  /** we have been signalled that a frame has arrived and we are now "inside"
   * the signalling object, so invoke the appropriate callback
   */
  virtual void _FrameArrived();

  // main loop for continuous operation.  called by start()
  void *Main(void *);
  void _CheckInitialised();


  // Make the next step, returning the success or not of the operation
  virtual bool _Step() = 0;
  
  int delay;
  unsigned int frame_number;
  bool cont,thread_running,_initialised;
  ThreadType parent;
  unsigned int number_of_channels;

  // Some data is protected by a mutex lock
  Mutex access_lock;

  bool user_finished; // user function returned

};


} // End namespace VW











// INLINES

inline unsigned int 
VW::SequencerBase::GetNumberOfChannels() const 
{
  return(number_of_channels);
}

inline unsigned int 
VW::SequencerBase::GetCurrentFrameNumber() const 
{
  return(frame_number);
}
inline void VW::SequencerBase::SetDelay(int d) 
{ 
  delay = d; 
}

inline int VW::SequencerBase::GetDelay() const 
{
  return(delay);
}

#endif // _sequencer_h_
