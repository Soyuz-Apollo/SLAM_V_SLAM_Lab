/** 
 * @file   sequencerfirewiregtk.h
 * @author BJ Tordoff
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
 * @class   VW::SequencerFirewireGTK
 * @ingroup Sequencers Interfaces
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 *
 * (c) Copyright University of Oxford
 *     All rights reserved
 *
 * @brief A GTK1-based interface to the firewire sequencer.
 *
 * This sequencer is a version of SequencerFirewire which also provides
 * a GTK interface. The sequencer functionality is identical to the parent
 * class, but with added functions to control the interface. As it is just a
 * GTK1 widget it can be embedded in any GTK1 form, either as a button and
 * pop-up window or as a full widget.
 */


// This class defines a GTK interface for the digital cameras
#ifndef _SequencerFirewireGTK_H
#define _SequencerFirewireGTK_H

// System stuff
#include <vector>
#include <gtk--/main.h>
#include <gtk--/window.h>
#include <gtk--/box.h>

// VW include
#include <VWFirewire/sequencerfirewire.h>

// Local stuff
#include "firewirecameragtk.h"
#include "interfacegtk.h"

namespace VW {

  class SequencerFirewireGTK;



  class SequencerFirewireGTKWidget : public VW::InterfaceGTKWidget {
  public:
    void InitForm(SequencerFirewireGTK* parent);
    // These widgets are required throughout the class
    Gtk::VBox* cameras_box;
  };



  class SequencerFirewireGTK : 
    public VW::InterfaceGTK<SequencerFirewireGTKWidget>,
    public VW::SequencerFirewire {
  public:
    SequencerFirewireGTK(VW::InterfaceType t=VW::INTERFACE_AS_BUTTONWINDOW,
			 unsigned int x_reduction=1,
			 unsigned int y_reduction=1);
    ~SequencerFirewireGTK();
    
    virtual bool Init(); // Overload the probe so that form can be changed too

  /** Start continuous capture from the sequence. */
  virtual void Continuous();
  /** Stop continuous capture from the sequence. */
  virtual void Stop();
  /** Capture the next single frame. */
  virtual void Next();


    // Interface specific
    void CreateForm();
    void UpdateForm() {};
    void ShowCamera(unsigned int which);
    // Must have a load and save function even if they do nothing
    void LoadParameterFile(std::string paramfile=".param") {};
    void SaveParameterFile(std::string paramfile=".param") {};

   // SigC-style signals to attach to (you can also use the VWEvents ones)
    SigC::Signal1<void,int> NewFrameSignal;

  protected:
    // Overloaded from parent Grabber class
    virtual void      AddNewCamera(std::vector<VW::FirewireCamera*>& c_list,
				   unsigned int which_element);

    // Overload the Frame arrived bit to also emit the sigc signal
    virtual void _FrameArrived() {
      user_finished=false;
      NewFrameEvent(0l); 
      NewFrameSignal.emit(0l);
      user_finished=true;
    }
  }; // end class SequencerFirewireGTK
}; // end namespace

#endif
