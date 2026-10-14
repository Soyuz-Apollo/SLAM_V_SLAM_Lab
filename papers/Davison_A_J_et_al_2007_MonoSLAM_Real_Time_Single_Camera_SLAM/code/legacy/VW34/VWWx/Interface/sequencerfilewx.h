/** 
 * @file   sequencerfilewx.h
 * @author BJ Tordoff
 * @date   27/05/2004
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
 * @class   VW::SequencerFileWx
 * @ingroup Sequencers Interfaces
 * @author  BJ Tordoff
 * @date    27/05/2004
 * @brief A Wx-based interface to the file sequencer.
 *
 * This sequencer is a version of SequencerFile which also provides
 * a WxWidgets interface. The sequencer functionality is identical to the parent
 * class, but with added functions to control the interface. As it is just a
 * Wx widget it can be embedded in any WxWidgets form in the usual manner.
 */


#ifndef _SequencerFileWx_h_
#define _SequencerFileWx_h_

#include <string>
#include <vector>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/event.h>

#include "evtsequencernewframe.h"
#include <VW/Sequencers/sequencerfile.h>

namespace VW {


class SequencerFileWx 
  : public wxPanel,
    public VW::SequencerFile
    
{
public:
  SequencerFileWx(wxWindow *parent,
		  int wx_id);
  SequencerFileWx(wxWindow *parent,
		  int wx_id,
		  const std::string& first_file_name);
  SequencerFileWx(wxWindow *parent,
		  int wx_id,
		  const std::vector<std::string>& first_file_names);
  ~SequencerFileWx();
    
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

  // Interface specific
  void CreateForm(wxWindow *parent);
  void UpdateForm();
  void UpdateFromForm();

 protected: // Funcs
  // Overload the Frame arrived bit to also emit the sigc signal
  virtual void _FrameArrived();
  void OnLoopChanged(int which);
  void OnStopClicked(wxCommandEvent & event);
  void OnNextClicked(wxCommandEvent & event);
  void OnRewindClicked(wxCommandEvent & event);
  void OnContinuousClicked(wxCommandEvent & event);

  // We will be handling some GUI events, so need to declare it
  DECLARE_EVENT_TABLE();
  DECLARE_CLASS(SequencerFileWx);


 protected: // Data
  int _wx_id;
  wxButton *_cont_button, *_next_button, *_rewind_button, *_stop_button; 
  wxStaticText *_framenumber_label;
}; // end class SequencerFileWx

}; // End namespace VW



#endif
