/** @file imageinterleave.cpp
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

#include "imageinterleave.h"
#include "imageio.h"

using namespace std;
using namespace VW;



//////////////////////////////////////////////////////////

template <class Type, int Skip>
bool ImageInterleave<Type,Skip>::ReadImage(const std::string& filename)
{
  return VW::ReadImage(filename,*this);
}

template <class Type, int Skip>
bool ImageInterleave<Type,Skip>::WriteImage(const std::string& filename) const
{
  return VW::WriteImage(filename,*this);
}


//////////////////////////////////////////////////////////

template class VW::ImageInterleave <unsigned char, 4>;
template class VW::ImageInterleave <unsigned char, 2>;
