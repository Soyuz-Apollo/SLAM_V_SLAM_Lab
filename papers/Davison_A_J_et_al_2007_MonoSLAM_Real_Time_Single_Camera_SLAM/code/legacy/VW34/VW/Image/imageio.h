/** @file imageio.h
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

#ifndef _imageio_h_
#define _imageio_h_

#include <string>

namespace VW {
  

  // Now the funcs
  template <class ImType>
  bool ReadImage(FILE *file,
                 ImType& image);

  template <class ImType>
  bool ReadImage(const std::string& filename,
                 ImType& image);
  
  template <class ImType>
  bool WriteImage(FILE *file,
                  const ImType& image);

  template <class ImType>
  bool WriteImage(const std::string& filename,
                  const ImType& image);
 
}; // end namespace VW


#endif /* _imageio_h_ */
