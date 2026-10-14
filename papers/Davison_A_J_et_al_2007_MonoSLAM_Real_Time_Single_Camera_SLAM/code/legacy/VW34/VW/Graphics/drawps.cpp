/** 
 * @file   drawps.cpp
 * @author ID Reid
 * @date   01/09/2004
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

// System
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

// VW
#include <VW/image.h>
#include <VW/Improc/edgel.h>

// Local
#include "pscanvas.h"
#include "drawps.h"

using namespace std;
using namespace VW;


// {{{ void DrawFunctions::Draw(PSCanvas& psc, const ImageMono<unsigned char>& image)
void DrawFunctions::Draw(PSCanvas& psc, const ImageMono<unsigned char>& image) 
{
  if (!psc.IsInitialised()) {
    cerr << "Warning: PSCanvas not initialised\n";
    return;
  }
  const unsigned int w = image.GetWidth();
  const unsigned int h = image.GetHeight();
  // Store the current state for later
 psc.Stream() << "gsave\n";
  // Scale to make coords be pixels
 psc.Stream() << w << " " << h << " scale\n";
  // Now the image command
  psc.Stream() << w << " " << h << " 8 [" << w << " 0 0 -" << h << " 0 " << h << "]\n";
  psc.Stream() << "{ currentfile " << w << " string readhexstring pop } bind image\n";
  for (int i=0; i<int(image.GetHeight()); i++) {
    VW::PixelMono<unsigned char>* pix = image[i];
    for (int j=0; j<int(image.GetWidth()); j++, pix++) {
      psc.Stream() << setfill('0') << setw(2) << hex << int(pix->y);
    }
    psc.Stream() << setfill(' ') << dec << endl;
  }
  // Restore the previous state
 psc.Stream() << "grestore\n";
  psc.Stream() << endl;
  return;
}      


// {{{ void DrawFunctions::Draw(PSCanvas& psc, const ImageRGB<unsigned char>& image)
void DrawFunctions::Draw(PSCanvas& psc, const ImageRGB<unsigned char>& image) 
{
  if (!psc.IsInitialised()) {
    cerr << "Warning: PSCanvas not initialised\n";
    return;
  }
  const unsigned int w = image.GetWidth();
  const unsigned int h = image.GetHeight();

  // Store the current state for later
 psc.Stream() << "gsave\n";
  // Scale to make coords be pixels
 psc.Stream() << w << " " << h << " scale\n";
  // Now the image command
  psc.Stream() << w << " " << h << " 8 [" << w << " 0 0 -" << h << " 0 " << h << "]\n";
  psc.Stream() << "{ currentfile " << 3*w << " string readhexstring pop } bind false 3 colorimage\n";
  for (int i=0; i<int(image.GetHeight()); i++) {
    VW::PixelRGB<unsigned char>* rgb = image[i];
    for (int j=0; j<int(image.GetWidth()); j++, rgb++) {
      psc.Stream() << setfill('0') << setw(2) << hex << int(rgb->r);
      psc.Stream() << setfill('0') << setw(2) << hex << int(rgb->g);
      psc.Stream() << setfill('0') << setw(2) << hex << int(rgb->b);
    }
    psc.Stream() << setfill(' ') << dec << endl;
  }
  // Restore the previous state
 psc.Stream() << "grestore\n";
  psc.Stream() << endl;
  return;
}

void DrawFunctions::Draw(PSCanvas& psc, const ImageYUV<unsigned char>& image)
{
    if (!psc.IsInitialised()) {
    cerr << "Warning: PSCanvas not initialised\n";
    return;
  }
  const unsigned int w = image.GetWidth();
  const unsigned int h = image.GetHeight();

  // Store the current state for later
 psc.Stream() << "gsave\n";
  // Scale to make coords be pixels
 psc.Stream() << w << " " << h << " scale\n";
  // Now the image command
  psc.Stream() << w << " " << h << " 8 [" << w << " 0 0 -" << h << " 0 " << h << "]\n";
  psc.Stream() << "{ currentfile " << 3*w << " string readhexstring pop } bind false 3 colorimage\n";
  for (int i=0; i<int(image.GetHeight()); i++) {
    VW::PixelYUV<unsigned char>* yuv = image[i];
    for (int j=0; j<int(image.GetWidth()); j++,yuv++) {
      VW::PixelRGB<unsigned char> rgb = *yuv;
      psc.Stream() << setfill('0') << setw(2) << hex << int(rgb.r);
      psc.Stream() << setfill('0') << setw(2) << hex << int(rgb.g);
      psc.Stream() << setfill('0') << setw(2) << hex << int(rgb.b);
    }
    psc.Stream() << setfill(' ') << dec << endl;
  }
  // Restore the previous state
 psc.Stream() << "grestore\n";
  psc.Stream() << endl;
  return;
}

void DrawFunctions::Draw(PSCanvas& psc, const EdgeData& edges)
{
  const std::vector<VW::Edgel*>& edgels = edges.edgels;
  std::vector<VW::Edgel*>::const_iterator e;

  // The linked and unlinked edgels are drawn in turn to avoid constant
  // changing of the pen colour (which makes the ps file huge)
  // Linked first 
  psc.SetPenColour(0,0,127,0);
  for (e=edgels.begin(); e!=edgels.end(); e++) {
    if ((*e)->GetLinked()) {
      const double x = (*e)->GetX();
      const double y = (*e)->GetY();
      const double a = (*e)->GetA();
      const double sa=0.75*sin(a), ca=0.75*cos(a);
      psc.DrawLine(x-ca, y-sa, x+ca, y+sa);
    }
  }
  psc.SetPenColour(127,127,255,0);
  for (e=edgels.begin(); e!=edgels.end(); e++) {
    if (!(*e)->GetLinked()) {
      const double x = (*e)->GetX();
      const double y = (*e)->GetY();
      const double a = (*e)->GetA();
      const double sa=0.75*sin(a), ca=0.75*cos(a);
      psc.DrawLine(x-ca, y-sa, x+ca, y+sa);
    }
  }

}
