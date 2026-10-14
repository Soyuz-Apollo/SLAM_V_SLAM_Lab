#include "sequencerfirewireglute.h"
#include "button.h"

using namespace VW;

SequencerFirewireGlute::SequencerFirewireGlute(unsigned int x_reduction,
					       unsigned int y_reduction)
  : SequencerFirewire(x_reduction,y_reduction)
{
  _BuildInterface();
}

void SequencerFirewireGlute::_BuildInterface()
{
  Glute2::Button* cbut = new Glute2::Button("Continuous");
  Glute2::Button* nbut = new Glute2::Button("Next");
  Glute2::Button* sbut = new Glute2::Button("Stop");
  this->AddToRight(cbut);
  this->AddToRight(nbut);
  this->AddToRight(sbut);
  cbut->PressedEvent.Attach(this, &SequencerFirewireGlute::_ContClicked);
  nbut->PressedEvent.Attach(this, &SequencerFirewireGlute::_NextClicked);
  sbut->PressedEvent.Attach(this, &SequencerFirewireGlute::_StopClicked);
}

void SequencerFirewireGlute::_ContClicked()
{
  Continuous();
}

void SequencerFirewireGlute::_NextClicked()
{
  Next();
}

void SequencerFirewireGlute::_StopClicked()
{
  Stop();
}
