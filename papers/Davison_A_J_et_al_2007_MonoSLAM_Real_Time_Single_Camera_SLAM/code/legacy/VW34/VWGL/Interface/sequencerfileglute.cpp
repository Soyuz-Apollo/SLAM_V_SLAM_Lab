#include "sequencerfileglute.h"
#include "button.h"

using namespace VW;

SequencerFileGlute::SequencerFileGlute()
  : SequencerFile()
{
  _BuildInterface();
}

SequencerFileGlute::SequencerFileGlute(const std::string& file_name)
  : SequencerFile(file_name)
{
  _BuildInterface();
}

void SequencerFileGlute::_BuildInterface()
{
  Glute2::Button* cbut = new Glute2::Button("Continuous");
  Glute2::Button* nbut = new Glute2::Button("Next");
  Glute2::Button* sbut = new Glute2::Button("Stop");
  this->AddToRight(cbut);
  this->AddToRight(nbut);
  this->AddToRight(sbut);
  cbut->PressedEvent.Attach(this, &SequencerFileGlute::_ContClicked);
  nbut->PressedEvent.Attach(this, &SequencerFileGlute::_NextClicked);
  sbut->PressedEvent.Attach(this, &SequencerFileGlute::_StopClicked);
}

void SequencerFileGlute::_ContClicked()
{
  Continuous();
}

void SequencerFileGlute::_NextClicked()
{
  Next();
}

void SequencerFileGlute::_StopClicked()
{
  Stop();
}
