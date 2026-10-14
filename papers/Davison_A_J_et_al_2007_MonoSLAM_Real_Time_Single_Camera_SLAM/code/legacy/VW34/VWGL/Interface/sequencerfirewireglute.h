#ifndef _seqeuncerfirewireglute_h_
#define _seqeuncerfirewireglute_h_

#include "hbox.h"
#include <VW/events.h>
#include <VWFirewire/sequencerfirewire.h>

namespace VW {

  class SequencerFirewireGlute 
    : public VW::SequencerFirewire,
      public Glute2::HBox,
      public VWEvents::EventHandler { 
  public:
    SequencerFirewireGlute(unsigned int x_reduction=1,
			   unsigned int y_reduction=1);

  protected:
    void _BuildInterface();
    void _ContClicked();
    void _NextClicked();
    void _StopClicked();

  };

}; // end namespace VW


#endif
