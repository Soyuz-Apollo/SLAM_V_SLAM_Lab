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

#ifndef _sequencerfileglow_h_
#define _sequencerfileglow_h_

#include <VW/sequencers.h>
#include <glow.h>
#include <glowQuickPalette.h>

GLOW_NAMESPACE_USING

namespace VW {

  class SequencerFileGlow : public VW::SequencerFile,
    public GlowQuickPaletteWindow,
    public GlowPushButtonReceiver
  { 
  public:
    SequencerFileGlow();
    SequencerFileGlow(const std::string& file_name);

    

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
