/** 
 * @file   displaygtkwidget.h
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
 * @class  VW::DisplayGTKWidget
 * @ingroup Displays
 * @author BJ Tordoff
 * @date   29/01/2002
 * @brief  An embeddable GTK widget for displaying images.
 */


#ifndef _DISPLAY_GTK_WIDGET_H
#define _DISPLAY_GTK_WIDGET_H

// System includes
#include <gtk--.h>

/* Need to know about those lurverly images.. */
#include "VW/image.h"
#include "VW/graphics.h"
#include "displaygtkcommon.h"

#define Z_ADJ 1
#define IMAGE_SIZE 100

namespace VW {

class DisplayGTKWidget : public Gtk::DrawingArea, public DisplayGTKCommon {
public:
  // Genesis and revelations...
  DisplayGTKWidget(const std::string title = "");
  DisplayGTKWidget(const unsigned int width, 
		     const unsigned int height, 
		     const std::string title = "");
  ~DisplayGTKWidget(void) {};


  // Resize an existing display
  void SetSize(const unsigned int new_width, 
	       const unsigned int new_height);
  unsigned int GetWidth();
  unsigned int GetHeight();

  // Primitive drawing operations
  void SetPixel(const int x, const int y);
  void SetPixel(const int x, const int y, 
		const unsigned char red, 
		const unsigned char green, 
		const unsigned char blue, 
		const unsigned char alpha=0);
  void DrawPoint(const float x, const float y);
  void DrawLine(const float x0, const float y0, 
		const float x1, const float y1);
  void SetPenColour(const unsigned char red, const unsigned char green, 
		    const unsigned char blue, const unsigned char alpha);
  void GetPenColour(unsigned char& red, unsigned char& green, 
		    unsigned char& blue, unsigned char& alpha) const;

  void Flush() {};
  void Reset();

  // Particular to GTK displays
  DisplayGTKWidget* GetDrawingArea() {return(this);};

  // SigC-style signals to attach to (you can also use the VWEvents ones)
  SigC::Signal3<void,int,int,int> ClickSignal;
  SigC::Signal5<void,int,int,int,int,int> DragSignal;
  SigC::Signal5<void,int,int,int,int,int> DragFinishedSignal;

protected:
  // Last click pos
  int last_x;
  int last_y;
  // Whether we're mid-drag
  bool dragging;

  // The current pen
  Gdk_Color pen_colour;


  // take over some impl methods
  gint expose_event_impl(GdkEventExpose*); 
  //  void size_allocate_impl(GtkAllocation* p0);
  gint motion_notify_event_impl(GdkEventMotion*); 
  gint button_press_event_impl(GdkEventButton*); 
  gint button_release_event_impl(GdkEventButton*); 

};


} // end namespace VW

// Drawing primitves
inline void VW::DisplayGTKWidget::SetPixel(const int x, const int y) 
{

  gdk_draw_point (this->get_window(),
                  this->get_style()->get_fg_gc(GTK_STATE_NORMAL), 
                  x,y);

}


inline void VW::DisplayGTKWidget::SetPixel(const int x, const int y,
					   const unsigned char red, 
					   const unsigned char green,
					   const unsigned char blue, 
					   const unsigned char alpha) 
{

  Gdk_Color pixcol;
  pixcol.red = red*255;
  pixcol.green = green*255;
  pixcol.blue = blue*255;
  get_colormap().alloc(pixcol);


  this->get_style()->get_fg_gc(GTK_STATE_NORMAL).set_foreground(pixcol);
  
  gdk_draw_point (this->get_window(),
                  this->get_style()->get_fg_gc(GTK_STATE_NORMAL), 
                  x,y);

}


inline void VW::DisplayGTKWidget::DrawPoint(const float x, const float y) {
  gdk_draw_point (this->get_window(),
                  this->get_style()->get_fg_gc(GTK_STATE_NORMAL), 
                  int(x),int(y));
}


inline void VW::DisplayGTKWidget::DrawLine(const float x0, const float y0,
					     const float x1, const float y1) {
  gdk_draw_line (this->get_window(),
		 this->get_style()->get_fg_gc(GTK_STATE_NORMAL), 
		 int(x0), int(y0), int(x1), int(y1));
}


inline unsigned int 
VW::DisplayGTKWidget::GetWidth() 
{
  return(width());
};

inline unsigned int 
VW::DisplayGTKWidget::GetHeight() 
{
  return(height());
};


inline void  
VW::DisplayGTKWidget::SetPenColour(const unsigned char red, 
				   const unsigned char green, 
				   const unsigned char blue, 
				   const unsigned char alpha) 
{
  pen_colour.red = red*255;
  pen_colour.green = green*255;
  pen_colour.blue = blue*255;
  get_colormap().alloc(pen_colour);

  this->get_style()->get_fg_gc(GTK_STATE_NORMAL).set_foreground(pen_colour);
}

inline void  
VW::DisplayGTKWidget::GetPenColour(unsigned char& red, 
				   unsigned char& green, 
				   unsigned char& blue, 
				   unsigned char& alpha) const 
{
  red = pen_colour.red/255;
  green = pen_colour.green/255;
  blue = pen_colour.blue/255;
  alpha=0;
}
 

inline void 
VW::DisplayGTKWidget::Reset() 
{
  gdk_draw_rectangle(this->get_window(),
		     this->get_style()->get_fg_gc(GTK_STATE_NORMAL),
		     1, // Filled
		     0,0,GetWidth(),GetHeight());
};

#endif
