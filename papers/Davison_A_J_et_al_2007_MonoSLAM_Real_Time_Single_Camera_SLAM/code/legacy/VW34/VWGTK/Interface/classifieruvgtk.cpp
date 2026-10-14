/** 
 * @file   classifieruvgtk.cc
 * @author BJ Tordoff
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

#include <unistd.h>
#include "classifieruvgtk.h"
#include <gtk--.h>

#define VERBOSE true
//******* Widget functions *******//


void VW::ClassifierUVGTKWidget::InitForm(ClassifierUVGTK* parent) {
  set_label("ClassifierUV control");
  set_border_width(10);

  Gtk::VBox* layout_table = manage(new Gtk::VBox());

  // Main buttonbar
  CreateButtonBar<ClassifierUVGTKWidget>(parent);
  layout_table->pack_start(*button_bar);

  filename = manage(new Gtk::Entry());
  filename->set_text("<no file>");
  layout_table->pack_start(*filename);

  add(*layout_table);

}
void VW::ClassifierUVGTK::UpdateForm() {
  // Needs some thought (finger classes return quaternions at present)
  my_widget->filename->set_text(_param_file);

}

// {{{ VW::ClassifierUVGTK::ClassifierUVGTK(VW::InterfaceType t)

VW::ClassifierUVGTK::ClassifierUVGTK(VW::InterfaceType t) {
  my_type=t;
  my_name="ClassifierUV Params";
  if (t==VW::INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
  my_widget->InitForm(this);
}

// }}}


void VW::ClassifierUVGTK::UpdateFromForm() {
  // Read values from the form
 

}

void VW::ClassifierUVGTK::LoadParameterFile(std::string paramfile)
{
  //bool result = 
  ClassifierUV::LoadParameterFile(paramfile);
  _param_file=paramfile;
  UpdateForm();
  //  return (result);
}
void VW::ClassifierUVGTK::SaveParameterFile(std::string paramfile)
{
  //       ClassifierUV::ReadData(paramfile);
  //       _param_file=paramfile;
  UpdateForm();
}


