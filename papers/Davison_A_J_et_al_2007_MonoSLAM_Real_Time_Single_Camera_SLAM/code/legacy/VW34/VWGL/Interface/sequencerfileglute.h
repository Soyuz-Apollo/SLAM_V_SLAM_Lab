#ifndef _seqeuncerfileglute_h_
#define _seqeuncerfileglute_h_

#include "hbox.h"
#include <VW/events.h>
#include <VW/sequencers.h>

namespace VW {

  class SequencerFileGlute 
    : public VW::SequencerFile,
      public Glute2::HBox,
      public VWEvents::EventHandler { 
  public:
    SequencerFileGlute();
    SequencerFileGlute(const std::string& file_name);

  protected:
    void _BuildInterface();
    void _ContClicked();
    void _NextClicked();
    void _StopClicked();

  };

}; // end namespace VW


#endif
