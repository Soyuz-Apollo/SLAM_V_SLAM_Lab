/** @file imagergbplanes.cpp
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

#include <string>
#include <iostream>

#include "imagergbplanes.h"
#include "imageio.h"

using namespace std;
using namespace VW;

/////////////////////////////////////////////////////////

template <class Type>
void ImageRGBPlanes<Type>::AllocImageData(const int w, const int h)
{
  r.AllocImageData(w,h);
  g.AllocImageData(w,h);
  b.AllocImageData(w,h);
  _alloced = true;
  return;
}


template <class Type>
ImageRGBPlanes<Type>::ImageRGBPlanes(const int w, const int h)
: _alloced(false)
{
  AllocImageData(w, h);
}

template <class Type>
ImageRGBPlanes<Type>::ImageRGBPlanes(Type *red, Type *green, Type *blue, 
			       const int w, const int h)
: _alloced(true)
{
	// Since we know that r,g,b are mono images, it's safe to do a
	// reinterpret cast to PixelMono
  r.SetImageData(reinterpret_cast<PixelMono<Type>* >(red), w, h);
  g.SetImageData(reinterpret_cast<PixelMono<Type>* >(green), w, h);
  b.SetImageData(reinterpret_cast<PixelMono<Type>* >(blue), w, h);
}

template <class Type>
bool ImageRGBPlanes<Type>::ReadImage(const std::string& filename)
{
  return VW::ReadImage(filename,*this);
}

template <class Type>
bool ImageRGBPlanes<Type>::ReadImage(FILE *file)
{
  return VW::ReadImage(file,*this);
}

template <class Type>
bool ImageRGBPlanes<Type>::WriteImage(const std::string& filename) const
{
  return VW::WriteImage(filename,*this);
}

template <class Type>
bool ImageRGBPlanes<Type>::WriteImage(FILE *file) const
{
  return VW::WriteImage(file,*this);
}



//////////////////////////////////////////////////////

template class VW::ImageRGBPlanes <unsigned char>;
template class VW::ImageRGBPlanes <double>;
template class VW::ImageRGBPlanes <float>;
//template class VW::ImageRGBplanes <int>;
//template class VW::ImageRGBplanes <short int>;
