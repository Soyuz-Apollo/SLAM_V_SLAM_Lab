/*  SceneLib: software for real-time SLAM

    correlate.h
    Copyright (C) 2005 University of Oxford

    Author
    Andrew Davison
    ajd@doc.ic.ac.uk

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _scene_correlate_h_
#define _scene_correlate_h_

#include <VW/image.h>

double correlate2_warning(const int x0, const int y0, 
			  const int x0lim, const int y0lim, 
			  const int x1, const int y1,
			  const VW::ImageMono<unsigned char> &p0,
			  const VW::ImageMono<unsigned char> &p1,
			  double *sd0ptr, double *sd1ptr);

#endif
