/** 
 * @file   sequencermoviegtk.h
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


// This class defines a GTK interface for the digital cameras
#ifndef _SequencerMovieGTK_H
#define _SequencerMovieGTK_H

#include <string>
#include <vector>
#include <gtk--.h>

#include "VW/Sequencers/sequencermovie.h"

#include "interfacegtk.h"

namespace VW {

  class SequencerMovieGTKWidget;



  class SequencerMovieGTK : 
    public VW::InterfaceGTK<SequencerMovieGTKWidget>,
    public VW::SequencerMovie {
  public:
    SequencerMovieGTK(const std::string& moviename,
		      VW::InterfaceType t=VW::INTERFACE_AS_BUTTONWINDOW);
    SequencerMovieGTK(const std::vector<std::string>& movienames,
		      VW::InterfaceType t=VW::INTERFACE_AS_BUTTONWINDOW);
    ~SequencerMovieGTK();
    
    virtual bool Init(); // Overload the probe so that form can be changed too

  /** Start continuous capture from the sequence. */
  virtual void Continuous();
  /** Stop continuous capture from the sequence. */
  virtual void Stop();
  /** Capture the next single frame. */
  virtual void Next();
  /** Re-open this file and start from the beginning. */
  virtual void Rewind();



   // Interface specific
    void CreateForm();
    void UpdateForm();
    void UpdateFromForm();

    // Must have a load and save function even if they do nothing
    void LoadParameterFile(std::string paramfile=".param") {};
    void SaveParameterFile(std::string paramfile=".param") {};
 
  protected:
  }; // end class SequencerMovieGTK



  class SequencerMovieChannelGTKWidget : public Gtk::HBox {
  public:
    void InitForm(SequencerMovieGTK* main_form);
    // These widgets are required throughout the class
    std::string GetMovieName();
    void SetMovieName(const std::string s);
    void SetLED(bool valid);
  protected:
    // for the file selector
    void LoadStart();
    void LoadOK();
    void LoadCancel();
    Gtk::FileSelection* file_selector;
   
    SequencerMovieGTK*  _main_form;
    Gtk::Entry*         file_name;
    Gtk::Button*        browse_button;
    Gtk::Pixmap*        status_led;
  };


  class SequencerMovieGTKWidget : public VW::InterfaceGTKWidget {
  public:
    void InitForm(SequencerMovieGTK* parent);
    std::string GetMovieName(unsigned int which_channel);
    void SetLED(unsigned int which_channel, bool valid);

    // These widgets are required throughout the class
    Gtk::VBox       *files_box;
    Gtk::Button     *go_button,*next_button,*stop_button,*rewind_button;
  protected:
    std::vector<SequencerMovieChannelGTKWidget*> file_widgets;
  };



}; // end namespace






#endif
