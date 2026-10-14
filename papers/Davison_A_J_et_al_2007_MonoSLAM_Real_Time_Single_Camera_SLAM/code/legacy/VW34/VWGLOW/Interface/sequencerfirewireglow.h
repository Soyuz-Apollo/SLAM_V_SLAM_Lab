/** 
 * @file   sequencerfileglow.h
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

#ifndef _sequencerfirewireglow_h_
#define _sequencerfirewireglow_h_

#include <VWFirewire/sequencerfirewire.h>
#include <glow.h>
#include <glowQuickPalette.h>

GLOW_NAMESPACE_USING

namespace VW {

  class SequencerFirewireGlow : public VW::SequencerFirewire,
    public GlowQuickPaletteWindow,
    public GlowPushButtonReceiver
  { 
  public:
    SequencerFirewireGlow(unsigned int x_reduction=1,
			  unsigned int y_reduction=1);
    

  protected:

    void ArrangeButtons();

    // Implement this method to receive pushbutton events
    void OnMessage(const GlowPushButtonMessage& message);

    GlowPushButtonWidget* continuous_button;
    GlowPushButtonWidget* next_button;
    GlowPushButtonWidget* stop_button;
  };

}; // end namespace VW



#endif
