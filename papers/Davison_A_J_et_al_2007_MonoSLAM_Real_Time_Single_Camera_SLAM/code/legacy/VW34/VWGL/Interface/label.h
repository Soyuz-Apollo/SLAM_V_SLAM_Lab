/** 
 * @file   label.h
 * @author BJ Tordoff & KR Connor
 * @date   03/12/2003
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

#ifndef GLUTE_LABEL_H
#define GLUTE_LABEL_H

#include <vector>
#include <string>
#include "widget.h"


namespace Glute2 {
  

  class Label : public Widget {
  public:
    // Note that the text can be multi-row (just include \n in the string)
    Label(const std::string& label);
    ~Label();

    virtual void Draw();
    virtual void SetSize(unsigned int width, unsigned int height);

    void ChangeLabel(const std::string& newlabel);

  protected:

    std::vector<std::string> _text;
    // Button dimensions
    unsigned int _max_text_w;
  };
  

} // namespace Glute2


#endif // GLUTE_LABEL_H
