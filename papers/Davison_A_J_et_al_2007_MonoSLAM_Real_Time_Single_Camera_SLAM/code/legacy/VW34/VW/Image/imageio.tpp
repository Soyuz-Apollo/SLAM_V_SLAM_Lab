/** @file imageio.tpp
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

#ifndef _imageio_tpp_
#define _imageio_tpp_

#include "imageio.h"

// For internal use only
std::string _FindSuffix(const std::string& filename);

namespace VW {
  

  // Default is to print an error and return
  template <class ImType>
  bool ReadImage(const std::string& filename,
                 ImType& image)
  {
	std::string suffix = _FindSuffix(filename);
    std::cerr << "VW::ReadImage: Sorry filetype \""<<suffix<<"\" not supported.\n";
    return false;
  }
  
  template <class ImType>
  bool WriteImage(const std::string& filename,
                  const ImType& image)
  {
    std::cerr << "VW::WriteImage: Sorry filetype \""<<_FindSuffix(filename)<<"\" not supported.\n";
    return false;
  }
 
}; // end namespace VW

#endif /* _imageio_tpp_ */
