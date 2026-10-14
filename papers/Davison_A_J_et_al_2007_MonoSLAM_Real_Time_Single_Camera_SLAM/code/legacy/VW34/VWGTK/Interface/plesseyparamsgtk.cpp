/** 
 * @file   plesseyparamsgtk.cc
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
#include <gtk--.h>

#include "VW/Improc/plessey.h"

#include "plesseyparamsgtk.h"


#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE true


using namespace std;



//******* Widget functions *******//


void VW::PlesseyParamsGTKWidget::InitForm(PlesseyParamsGTK* parent) {
  if (VERBOSE) cerr << "Create Plessey params form" << endl;

  this->set_label("Plessey Params");

  // Note that the form is constructed bottom-up, ie. the
  // lowest level elements are created then their parents
  // until the top level widget (main_table) is added 
  // directly to the frame.



  ////////////////
  // The params //
  ////////////////

  Gtk::Label*      thresh_label   = manage(new Gtk::Label("Strength threshold"));
  thresh_label->set_justify(GTK_JUSTIFY_RIGHT);
                   thresh_spinner = manage(new Gtk::SpinButton(1,2));
  tooltips.set_tip(*thresh_spinner, "Set the threshold for minimum corner strength.", "");
  Gtk::Label*      sigma_label    = manage(new Gtk::Label("Gaussian sigma"));
  sigma_label->set_justify(GTK_JUSTIFY_RIGHT);
                   sigma_spinner  = manage(new Gtk::SpinButton(1,2));
  tooltips.set_tip(*sigma_spinner, "Set the standard-deviation of the gaussian used in the smoothing convolution.", "");
  Gtk::Label*      convolution_size_label     = manage(new Gtk::Label("Convolution patch size"));
  convolution_size_label->set_justify(GTK_JUSTIFY_RIGHT);
                   convolution_size_spinner   = manage(new Gtk::SpinButton(1,0));
  tooltips.set_tip(*convolution_size_spinner, "Set the size of the (square) patch used for convolution.", "");
  Gtk::Label*      phobia_label   = manage(new Gtk::Label("Edge-phobia"));
  phobia_label->set_justify(GTK_JUSTIFY_RIGHT);
                   phobia_spinner = manage(new Gtk::SpinButton(1,2));
  tooltips.set_tip(*phobia_spinner, "Set the edge-phobia. This the level of suppression of features on edges.", "");
  Gtk::Label*      stored_size_label     = manage(new Gtk::Label("Stored patch size"));
  stored_size_label->set_justify(GTK_JUSTIFY_RIGHT);
                   stored_size_spinner   = manage(new Gtk::SpinButton(1,0));
  tooltips.set_tip(*stored_size_spinner, "Set the size of the (square) patch stored for matching and other comparisons.", "");


  // Set spinner limits
  Gtk::Adjustment* ad;
  //   - Thresh
  ad=thresh_spinner->get_adjustment();
  ad->set_lower(0);
  ad->set_upper(1.0);
  ad->set_value(parent->GetStrengthThreshold());
  ad->set_step_increment(0.01);
  ad->set_page_increment(0.1);
  //   - Sigma
  ad=sigma_spinner->get_adjustment();
  ad->set_lower(0.1);
  ad->set_upper(5.0);
  ad->set_value(parent->GetGaussianSigma());
  ad->set_step_increment(0.01);
  ad->set_page_increment(0.2);
  //   - Convolution Size
  ad=convolution_size_spinner->get_adjustment();
  ad->set_lower(1);
  ad->set_upper(9);
  ad->set_value(parent->GetConvolutionPatchSize());
  ad->set_step_increment(2);
  ad->set_page_increment(2);
  //   - Phobia
  ad=phobia_spinner->get_adjustment();
  ad->set_lower(0);
  ad->set_upper(1.0);
  ad->set_value(parent->GetEdgePhobia());
  ad->set_step_increment(0.01);
  ad->set_page_increment(0.1);
  //   - Convolution Size
  ad=stored_size_spinner->get_adjustment();
  ad->set_lower(1);
  ad->set_upper(9);
  ad->set_value(parent->GetStoredPatchSize());
  ad->set_step_increment(2);
  ad->set_page_increment(2);

  ad=thresh_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&PlesseyParamsGTK::UpdateFromForm));
  ad=sigma_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&PlesseyParamsGTK::UpdateFromForm));
  ad=convolution_size_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&PlesseyParamsGTK::UpdateFromForm));
  ad=phobia_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&PlesseyParamsGTK::UpdateFromForm));
  ad=stored_size_spinner->get_adjustment();
  ad->value_changed.connect(slot(parent,&PlesseyParamsGTK::UpdateFromForm));






  ///////////////////////////////
  // The param table and frame //
  ///////////////////////////////

  // Table
  Gtk::Table*      params_table   = manage(new Gtk::Table(5,2, false));
  params_table->set_spacings(3);
  params_table->set_border_width(3);

  params_table->attach(*thresh_spinner,  1,2,0,1);
  params_table->attach(*thresh_label,    0,1,0,1);
  params_table->attach(*sigma_spinner,   1,2,1,2);
  params_table->attach(*sigma_label,     0,1,1,2);
  params_table->attach(*convolution_size_spinner,    1,2,2,3);
  params_table->attach(*convolution_size_label,      0,1,2,3);
  params_table->attach(*phobia_spinner,  1,2,3,4);
  params_table->attach(*phobia_label,    0,1,3,4);
  params_table->attach(*stored_size_spinner,    1,2,4,5);
  params_table->attach(*stored_size_label,      0,1,4,5);

  // Frame
  Gtk::Frame*      params_frame   = manage(new Gtk::Frame());
  params_frame->set_border_width(3); 
  params_frame->add(*params_table);






  //////////////////////
  // Main table stuff //
  //////////////////////

  // Base class provides the toolbar
  CreateButtonBar<VW::PlesseyParamsGTKWidget>(parent);

  // Main table
  Gtk::VBox*      main_table     = manage(new Gtk::VBox());
  //  main_table->set_spacings(3);
  //  main_table->set_border_width(3); 
  main_table->pack_start(*button_bar); // Provided by base class
  main_table->pack_start(*params_frame);


  // Add to window
  this->add(*main_table);

}
void VW::PlesseyParamsGTK::UpdateForm() {
  // Stop the callbacks during the update
  allow_callbacks=false;


  // Set spinner values
  //   - Thresh
  my_widget->thresh_spinner->set_value(strength_threshold);
  //   - Sigma
  my_widget->sigma_spinner->set_value(gaussian_sigma);
  //   - Size
  my_widget->convolution_size_spinner->set_value(convolution_patch_size);
  //   - Phobia
  my_widget->phobia_spinner->set_value(edge_phobia);
  //   - Size
  my_widget->stored_size_spinner->set_value(stored_patch_size);

  // Finally check if a Plessey object has been atatched and 
  // update its data
  if (pointer_to_plessey_object!=0) {
    pointer_to_plessey_object->SetParams(*this);
  }

  // Enable the callbacks
  allow_callbacks=true;
}

// {{{ VW::PlesseyParamsGTK::PlesseyParamsGTK(InterfaceType t)
VW::PlesseyParamsGTK::PlesseyParamsGTK(InterfaceType t) : 
  pointer_to_plessey_object(0) {
  my_type=t;
  my_name="Plessey Params";
  if (t==INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
  my_widget->InitForm(this);
}
// }}}

VW::PlesseyParamsGTK::PlesseyParamsGTK(float        _strength_threshold,
				       float        _gaussian_sigma,
				       unsigned int _convolution_patch_size,
				       float        _edge_phobia,
				       unsigned int _stored_patch_size,
				       InterfaceType t) :
  PlesseyParams(_strength_threshold,_gaussian_sigma,_convolution_patch_size,_edge_phobia, _stored_patch_size),
  pointer_to_plessey_object(0) {
  my_type=t;
  my_name="Plessey Params";
  if (t==INTERFACE_AS_BUTTONWINDOW) {
    InitButtonWindow();
  } else {
    InitWidget();
  }
}


void VW::PlesseyParamsGTK::UpdateFromForm() {
  // Only update if allowed
  if (allow_callbacks) {
    // Read the spinner values from the form
    strength_threshold     = my_widget->thresh_spinner->get_value_as_float();
    gaussian_sigma         = my_widget->sigma_spinner->get_value_as_float();
    convolution_patch_size = static_cast<unsigned int>(my_widget->convolution_size_spinner->get_value_as_int());
    edge_phobia            = my_widget->phobia_spinner->get_value_as_float();
    stored_patch_size      = static_cast<unsigned int>(my_widget->stored_size_spinner->get_value_as_int());
    if (pointer_to_plessey_object!=0) {
      pointer_to_plessey_object->SetParams(*this);
    }
  }
}
// INLINES

void VW::PlesseyParamsGTK::SetStrengthThreshold(const float new_value)
{
  strength_threshold=new_value;UpdateForm();
};
    
void VW::PlesseyParamsGTK::SetGaussianSigma(const float new_value)
{
  gaussian_sigma=new_value;UpdateForm();
};

void VW::PlesseyParamsGTK::SetConvolutionPatchSize(const unsigned int new_value)
{convolution_patch_size=new_value;UpdateForm();};

void VW::PlesseyParamsGTK::SetEdgePhobia(const float new_value)
{
  edge_phobia=new_value;UpdateForm();
};

void VW::PlesseyParamsGTK::SetStoredPatchSize(const unsigned int new_value)
{
  stored_patch_size=new_value;UpdateForm();
};

void VW::PlesseyParamsGTK::LoadParameterFile(std::string paramfile)
{
  VW::PlesseyParams::LoadParameterFile(paramfile);
  UpdateForm();
}

void VW::PlesseyParamsGTK::SaveParameterFile(std::string paramfile) 
{
  VW::PlesseyParams::SaveParameterFile(paramfile);
  UpdateForm();
}
void VW::PlesseyParamsGTK::Set(const float         new_strengththreshold,
			       const float         new_gaussiansigma,
			       const unsigned int  new_convolutionpatchsize,
			       const float         new_edgephobia,
			       const unsigned int  new_storedpatchsize) 
{
  std::cerr << "Setting..." <<std::endl;
  strength_threshold     = new_strengththreshold;
  gaussian_sigma         = new_gaussiansigma;
  convolution_patch_size = new_convolutionpatchsize;
  edge_phobia            = new_edgephobia;
  stored_patch_size      = new_storedpatchsize;
  std::cerr << "Update..." <<std::endl;
  UpdateForm();
};
void VW::PlesseyParamsGTK::AttachPlesseyObject(VW::Plessey& plessey_object)
{
  pointer_to_plessey_object=&plessey_object;
}
