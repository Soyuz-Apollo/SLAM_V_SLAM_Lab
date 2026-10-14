/** 
 * @file   plesseyparamsgtk.h
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


// This class defines a GTK interface for the digital cameras
#ifndef _PlesseyParamsGTK_H
#define _PlesseyParamsGTK_H

#include <gtk--.h>
#include <VW/Improc/plesseydetector.h>
#include "interfacegtk.h"

namespace VW {
  class PlesseyParamsGTK;
  /**
   * The widget which forms the heart of the interface.
   * This widget contains all controls except the toolbar,
   * and should be embedded in a form.
   */
  class PlesseyParamsGTKWidget : public VW::InterfaceGTKWidget {
  public:
    // Interface specific
    void InitForm(PlesseyParamsGTK* parent);
    Gtk::SpinButton*    thresh_spinner; 
    Gtk::SpinButton*    sigma_spinner; 
    Gtk::SpinButton*    convolution_size_spinner; 
    Gtk::SpinButton*    phobia_spinner; 
    Gtk::SpinButton*    stored_size_spinner; 
  };

  class PlesseyParamsGTK : public VW::InterfaceGTK<PlesseyParamsGTKWidget>
			 , public VW::PlesseyParams {
  public:
    PlesseyParamsGTK(VW::InterfaceType t=VW::INTERFACE_AS_BUTTONWINDOW);
    PlesseyParamsGTK(float        _strength_threshold,
		     float        _gaussian_sigma,
		     unsigned int _convolution_patch_size,
		     float        _edge_phobia,
		     unsigned int _stored_patch_size,
		     VW::InterfaceType t=VW::INTERFACE_AS_BUTTONWINDOW);
   
    // Attaching a plessey object (allows auto-update of params)
    void AttachPlesseyObject(VW::Plessey& plessey_object);

    // Manipulator functions
    virtual void Set(const float         new_strengththreshold,
		     const float         new_gaussiansigma,
		     const unsigned int  new_convolutionpatchsize,
		     const float         new_edgephobia,
		     const unsigned int  new_storedpatchsize);
    
    void UpdateForm();
    virtual void SetStrengthThreshold(const float new_value);
    virtual void SetGaussianSigma(const float new_value);
    virtual void SetConvolutionPatchSize(const unsigned int new_value);
    virtual void SetEdgePhobia(const float new_value);
    virtual void SetStoredPatchSize(const unsigned int new_value);
    virtual void LoadParameterFile(std::string paramfile = ".params");
    virtual void SaveParameterFile(std::string paramfile = ".params");
    
    
    void UpdateFromForm();
  private:
    VW::Plessey*            pointer_to_plessey_object; 
    bool allow_callbacks;
  };
  
}; // end namespace






#endif
