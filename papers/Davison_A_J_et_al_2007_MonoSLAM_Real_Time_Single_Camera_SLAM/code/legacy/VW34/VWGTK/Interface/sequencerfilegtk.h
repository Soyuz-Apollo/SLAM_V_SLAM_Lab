/** 
 * @file   sequencerfilegtk.h
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
 * @class   VW::SequencerFileGTK
 * @ingroup Sequencers Interfaces
 * @author  ID Reid, BJ Tordoff
 * @date    02/08/02
 * @brief A GTK1-based interface to the file sequencer.
 *
 * This sequencer is a version of SequencerFile which also provides
 * a GTK interface. The sequencer functionality is identical to the parent
 * class, but with added functions to control the interface. As it is just a
 * GTK1 widget it can be embedded in any GTK1 form, either as a button and
 * pop-up window or as a full widget.
 */


// This class defines a GTK interface for the digital cameras
#ifndef _SequencerFileGTK_H
#define _SequencerFileGTK_H

#include <string>
#include <vector>
#include <gtk--.h>

#include "VW/Sequencers/sequencerfile.h"

#include "interfacegtk.h"

namespace VW {

class SequencerFileGTKWidget;



class SequencerFileGTK : 
    public VW::InterfaceGTK<SequencerFileGTKWidget>,
    public VW::SequencerFile
{
public:
  SequencerFileGTK(VW::InterfaceType t=VW::INTERFACE_AS_BUTTONWINDOW);
  SequencerFileGTK(const std::string& first_file_name,
		   VW::InterfaceType t=VW::INTERFACE_AS_BUTTONWINDOW);
  SequencerFileGTK(const std::vector<std::string>& first_file_names,
		   VW::InterfaceType t=VW::INTERFACE_AS_BUTTONWINDOW);
  ~SequencerFileGTK();
    
  virtual bool Init(); // Overload the probe so that form can be changed too

  void AddChannel(const std::string& file_name);

  /** Start continuous capture from the sequence. */
  virtual void Continuous();
  /** Stop continuous capture from the sequence. */
  virtual void Stop();
  /** Capture the next single frame. */
  virtual void Next();
  /** Re-open this file and start from the beginning. */
  virtual void Rewind();

  virtual void SetLooping(SequencerLoopMode mode=LOOP);


  // SigC-style signals to attach to (you can also use the VWEvents ones)
  SigC::Signal1<void,int> NewFrameSignal;

  // Interface specific
  void CreateForm();
  void UpdateForm();
  void UpdateFromForm();

  // Must have a load and save function even if they do nothing
  void LoadParameterFile(std::string paramfile=".param") {};
  void SaveParameterFile(std::string paramfile=".param") {};
 
  void OnLoopChanged(int which);
protected:

    // Overload the Frame arrived bit to also emit the sigc signal
    virtual void _FrameArrived() {
      user_finished=false;
      NewFrameEvent(0l); 
      NewFrameSignal.emit(0l);
      user_finished=true;
    }


}; // end class SequencerFileGTK



class SequencerFileChannelGTKWidget : public Gtk::HBox {
public:
  void InitForm(SequencerFileGTK* main_form);
  // These widgets are required throughout the class
  std::string GetFileName()   {return(file_name->get_text());}

  void SetFileName(const std::string s)   {file_name->set_text(s);}
  void SetLED(bool valid);
protected:
  // for the file selector
  void LoadStart();
  void LoadOK();
  void LoadCancel();
  Gtk::FileSelection* file_selector;
   
  SequencerFileGTK*  _main_form;
  Gtk::Entry*         file_name;
  Gtk::Button*        browse_button;
  Gtk::Pixmap*        status_led;
};


class SequencerFileGTKWidget : public VW::InterfaceGTKWidget {
public:
  void        InitForm (SequencerFileGTK* parent);
  std::string GetFileName (unsigned int which_channel)   
    {return(file_widgets[which_channel]->GetFileName());}
  void        SetLED (unsigned int which_channel, bool valid);
  void        AddFile (SequencerFileGTK* parent);

  // These widgets are required throughout the class
  Gtk::VBox       *files_box;
  Gtk::Button     *go_button,*next_button,*stop_button,*rewind_button;
  Gtk::Label      *frame_no_label;
  Gtk::OptionMenu *loop_menu;
protected:
  std::vector<SequencerFileChannelGTKWidget*> file_widgets;
};



}; // end namespace



#endif
