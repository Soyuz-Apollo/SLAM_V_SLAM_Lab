/** 
 * @file   sequencerfirewireglow.h
 * @author AJ Davison
 * @date   20/08/2005
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

#include "sequencerfirewireglow.h"
using namespace std;
using namespace VW;

SequencerFirewireGlow::SequencerFirewireGlow(unsigned int x_reduction,
					     unsigned int y_reduction)
  : SequencerFirewire(x_reduction,y_reduction),
    GlowQuickPaletteWindow("Firewire Sequencer")
{
  ArrangeButtons();
}

void SequencerFirewireGlow::ArrangeButtons()
{
  GlowQuickPanelWidget* panel = AddPanel(
		GlowQuickPanelWidget::etchedStyle, "Main Controls");
  panel = panel->AddArrangingPanel(GlowQuickPanelWidget::horizontal);
  continuous_button = panel->AddPushButton("Continuous", this);
  next_button = panel->AddPushButton("Next", this);
  stop_button = panel->AddPushButton("Stop", this);
  Pack();
}

void SequencerFirewireGlow::OnMessage(const GlowPushButtonMessage& message)
{
  if (message.widget == continuous_button)
    Continuous();
  else if (message.widget == next_button)
    SequencerBase::Next();
  else if (message.widget == stop_button)
    Stop();
}
