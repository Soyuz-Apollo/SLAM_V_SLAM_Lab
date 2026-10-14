/** 
 * @file   yorickgtk.h
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
/**
 * @class   YorickGTK
 * @ingroup Interfaces Hardware
 * @author  BJ Tordoff
 * @date    02/08/2002
 * @brief   A GTK based interface for controlling Yorick.
 * This derrives from Yorick, and can be used as a drop-in replacement.
 */


// This class defines a GTK interface for the digital cameras
#ifndef _YorickGTK_H
#define _YorickGTK_H

#include <VWYorick/yorickmain.h>

#include "interfacegtk.h"
#include <vector>
#include <gtk--.h>

namespace VW {

  class YorickGTK;

  /** The Yorick drag-pad.
   * This class provides a drawingarea on which mouse-drags
   * send commands to Yorick. Drags with button1 control
   * pan and tilt, and button2 controls vergence. The current axis
   * positions are represented visually by a cross-hair.
   */
    class YorickDragPad : public Gtk::DrawingArea {
      /* Backing pixmap for drawing area */
      
      Gdk_Pixmap pixmap;
      // Default parameters
      static const double _DEFAULT_SENSITIVITY           = 2.0;    // In pixel/deg
      Gdk_GC gc;
      Gdk_GC brush_gc;
      Gdk_Window win;
      Gdk_Visual visual;
      VW::Yorick *_yorick_p;
      double _yo_sensitivity;        // Pixels (of mouse motion) per degree
      
      virtual gint configure_event_impl (GdkEventConfigure *event);
      virtual gint expose_event_impl (GdkEventExpose *event);
      virtual gint button_press_event_impl (GdkEventButton *event);
      virtual gint motion_notify_event_impl (GdkEventMotion *event);
      
    public:
      void Erase();
      void Redraw();
      
      YorickDragPad (VW::Yorick *yorick_pointer);
      ~YorickDragPad ();
      
    protected:
      // Some control data
      float _pan;
      float _tilt;
      float _vergel, _verger;
    private:
      // Record where button was pressed
      int _pressed_at_x;
      int _pressed_at_y;
      float _pressed_at_pan;
      float _pressed_at_tilt;
      float _pressed_at_vergel;
      float _pressed_at_verger;
};




  /** The Yorick widget.
   * This class provides a widget with all the necessary controls
   * for Yorick - manual entry spin-buttons for each axis and a
   * drag-pad (see VW::YorickDragPad). It should not be used directly
   * and is merely contained by the YorickGTK class.
   */
  class YorickGTKWidget : public VW::InterfaceGTKWidget {
  public:
    void InitForm(YorickGTK* parent);
    void EnableForm();
    void DisableForm();
    Gtk::SpinButton* vergeleft_spinner;
    Gtk::SpinButton* vergeright_spinner;
    Gtk::SpinButton* elevation_spinner;
    Gtk::SpinButton* pan_spinner;
    VW::YorickDragPad* drag_pad;
    
  protected:
    Gtk::Button*       activate_button;
    Gtk::Button*       recentre_button;
    Gtk::Button*       origin_button;
  static const int _DEFAULT_CONTROL_WIDGET_SIZE_X    = 100;
  static const int _DEFAULT_CONTROL_WIDGET_SIZE_Y    = 100;
  };




  /* Main Yorick interface class.
   * This derrives from Yorick, and can be used as a drop-in replacement.
   */
  class YorickGTK : 
    public VW::InterfaceGTK<YorickGTKWidget>,
    public VW::Yorick {
  public:
    YorickGTK(VW::InterfaceType t=VW::INTERFACE_AS_BUTTONWINDOW);
    ~YorickGTK();
    
    // Interface specific
    void CreateForm();
    void UpdateForm();

    /** Switch on/off yorick. */
    virtual void i_Activate();
    /** Slowly move back to all axes at zero. */
    virtual void i_Recentre();
    /** Set the current position as zero. */
    virtual void i_SetOrigin();

    // Interface initiated demands
    virtual void i_VergeLeft();
    virtual void i_VergeRight();
    virtual void i_Elevate();
    virtual void i_Pan();

    // Overloaded functions from parent
    virtual int Demand(float p, float t, float vl, float vr);
    virtual int DemandPan(float p);
    virtual int DemandTilt(float t);
    virtual int DemandPanTilt(float p, float t);
    virtual int DemandVergeLeft(float vl);
    virtual int DemandVergeRight(float vr);
    virtual int DemandVerge(float vl, float vr);


    // Must have a load and save function even if they do nothing
    void LoadParameterFile(std::string paramfile=".param") {};
    void SaveParameterFile(std::string paramfile=".param") {};
 
  protected:
    bool allow_callbacks;
  }; // end class YorickGTK
}; // end namespace

#endif
