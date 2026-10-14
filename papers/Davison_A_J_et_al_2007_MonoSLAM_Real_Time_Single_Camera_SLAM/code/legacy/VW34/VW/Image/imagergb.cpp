/** @file imagergb.cpp
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

#include "imagergb.h"
#include "imageio.h"

using namespace std;
using namespace VW;


template <class Type>
bool ImageRGB<Type>::ReadImage(FILE *file)
{
  return VW::ReadImage(file,*this);
}

template <class Type>
bool ImageRGB<Type>::ReadImage(const string& filename)
{
  return VW::ReadImage(filename,*this);
}

template <class Type>
bool ImageRGB<Type>::WriteImage(FILE *file) const
{
  return VW::WriteImage(file,*this);
}

template <class Type>
bool ImageRGB<Type>::WriteImage(const string& filename) const
{
  return VW::WriteImage(filename,*this);
}




//////////////////////////////////////////////////////
// Instantiate some common versions

template class VW::ImageRGB <unsigned char>;
template class VW::ImageRGB <double>;
template class VW::ImageRGB <float>;
//template class ImageRGB <int>;
//template class ImageRGB <short int>;
