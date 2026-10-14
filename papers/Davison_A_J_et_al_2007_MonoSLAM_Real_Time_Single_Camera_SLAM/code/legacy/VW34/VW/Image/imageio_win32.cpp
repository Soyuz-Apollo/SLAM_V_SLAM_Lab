/** @file imageio_win32.cpp
 *
 *              (c) Copyright University of Cambridge
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



// DO NOT COMPILE THIS FILE DIRECTLY - IT IS INCLUDED BY IMAGEIO.CPP



#include "imageio.h"
#include "imageio_gdi.tpp"
#include "imageio_ppm.tpp"





// Simply call the GDI code

template <class ImType>
bool VW::ReadImage(const std::string& filename,
						ImType& image)
{
	// For ppm's use the native handler, else use GDI (string ops in imageio_gdi.tpp)
	std::string suffix = _FindSuffix(filename);
	if (_CaseInsensitiveCompare(suffix,"ppm")==0) {
		const bool success = VW::ReadPPMImage(filename, image);
		return success;
	}
	return VW::ReadGDIImage(filename, image);
}
  

template <class ImType>
bool VW::WriteImage(const std::string& filename,
						const ImType& image)
{
	// For ppm's use the native handler, else use GDI (string ops in imageio_gdi.tpp)
	std::string suffix = _FindSuffix(filename);
	if (_CaseInsensitiveCompare(suffix,"ppm")==0) {
		return VW::WritePPMImage(filename, image);
	}
	return VW::WriteGDIImage(filename, image);
}


