/** 
 * @file   displayxformsform.c
 * @author ID Reid
 * @date   02/08/2002
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

#include "forms.h"
#include "displayxformsform.h"

extern int DisplayXFormsCanvasHandler(FL_OBJECT *ob, Window win, int width, int height, XEvent* xev, void* data);

FD_DisplayForm *create_form_DisplayForm(int w, int h, long parent)
{
  /* w,h are width / height, parent is a pointer to the owning object */
  FL_OBJECT *obj;
  FD_DisplayForm *fdui = (FD_DisplayForm *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();
  
  fl_set_border_width(2);
  fdui->DisplayForm = fl_bgn_form(FL_NO_BOX, w, h);
  obj = fl_add_box(FL_UP_BOX,0,0,w,h,"");
  fl_set_object_color(obj,FL_MCOL,FL_WHITE);
  fdui->canvas_graphic = obj = fl_add_canvas(FL_NORMAL_CANVAS,0,0,w,h,"DisplayForm");
  fl_set_object_resize(obj, FL_RESIZE_NONE);
  fl_add_canvas_handler(fdui->canvas_graphic, Expose, 
			&DisplayXFormsCanvasHandler, (void *)parent);
  fl_add_canvas_handler(fdui->canvas_graphic, ButtonPress, 
			&DisplayXFormsCanvasHandler, (void *)parent);
  fl_add_canvas_handler(fdui->canvas_graphic, ButtonRelease, 
			&DisplayXFormsCanvasHandler, (void *)parent);
  fl_add_canvas_handler(fdui->canvas_graphic, MotionNotify, 
			&DisplayXFormsCanvasHandler, (void *)parent);
  fl_end_form();
  
  fdui->DisplayForm->fdui = fdui;
  fl_set_border_width(old_bw);
  
  return fdui;
}   
