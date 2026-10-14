/** 
 * @file   paramfiles_general.h  
 * @author JGH Knight
 * @date   02/08/02
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


#ifndef _paramfiles_general_h_
#define _paramfiles_general_h_

#include <string>

/* ParameterFiles namespace
 *
 * Any info pertaining to how parameter files are formed goes here.
 * See paramfiles.README.
 */

namespace ParameterFiles {
  const std::string PARAMETER_LIST_LABEL = "parameter list";
  const std::string PARAMETER_LIST_NAME_LABEL = "name";
  const std::string PARAMETER_LABEL = "parameter";
  const std::string PARAMETER_NAME_LABEL = "name";
  const std::string PARAMETER_DATA_LABEL = "data";
}

#endif // #ifndef _paramfiles_general_h_
