/** 
 * @file   displayxformsform.h
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


/** Header file generated with fdesign on Thu Sep 16 15:28:40 1999.**/

#ifndef FD_DISPLAYFORM_H_
#define FD_DISPLAYFORM_H_

#include <X11/forms.h>


/** Callback **/
extern int DisplayXFormsCanvasHandler(FL_OBJECT *ob, Window win, int width, int height, XEvent* xev, void* data);
/**** Forms and Objects ****/
typedef struct {
	FL_FORM *DisplayForm;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *canvas_graphic;
} FD_DisplayForm;

extern FD_DisplayForm * create_form_DisplayForm(int w,int h,long parent);
#endif
