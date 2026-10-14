/** 
 * @file   displaygtkcommon.h
 * @author BJ Tordoff
 * @date   29/01/2002
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
 * @class  VW::DisplayGTKCommon
 * @author BJ Tordoff
 * @date   29/01/2002
 *
 * This abstract class provides the functions that are common between
 * the GTK display which opens a window, and the one that doesn't
 */

#ifndef _DISPLAY_GTKMM_COMMON_H
#define _DISPLAY_GTKMM_COMMON_H


#include <string>
#include "VW/graphics.h"
#include "gdk--.h"

namespace VW {

  class DisplayGTKWidget;

class DisplayGTKCommon : public DisplayBase {
 public:
  DisplayGTKCommon (const std::string title = "") : 
    DisplayBase (title) {gdk_rgb_init();}
  DisplayGTKCommon (const unsigned int width, const unsigned int height,
		    const std::string title = "") :
    DisplayBase (width, height, title) {gdk_rgb_init();}

  virtual void Reset () = 0;
  virtual void Flush () = 0;

  // Particular to GTK displays
  virtual DisplayGTKWidget* GetDrawingArea() = 0;

 protected:

};

} // end namespace VW

#endif
