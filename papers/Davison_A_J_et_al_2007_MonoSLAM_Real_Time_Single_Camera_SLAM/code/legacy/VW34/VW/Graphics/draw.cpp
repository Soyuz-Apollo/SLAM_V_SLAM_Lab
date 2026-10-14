/** @file draw.cpp
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
#include <vector>

// VW
#include <VW/MathBits/covariance2d.h>

#include <VW/Image/imagemono.h>
#include <VW/Image/imagergb.h>
#include <VW/Image/imagehsv.h>
#include <VW/Image/imageinterleave.h>

#include <VW/Improc/plesseycorner.h>
#include <VW/Improc/edgel.h>
#include <VW/Improc/linesegfitted.h>
#include <VW/Improc/siftfeature.h>
#include <VW/Improc/patchfeature.h>

// Local
#include "draw.h"

using namespace std;
using namespace VW;

// Images
// {{{ void DrawFunctions::Draw(DisplayBase& display, const ImageMono<unsigned char>& image)

void DrawFunctions::Draw(DisplayBase& display, const ImageMono<unsigned char>& image) {
  //cout << "Draw Monoimage using generic code" << endl;
  // Only have base image, so must draw using (slow) primitives
  int im_w=image.GetWidth();
  int im_h=image.GetHeight();
  int di_w=display.GetWidth();
  int di_h=display.GetHeight();
  int width=0,height=0;
  if (im_w<di_w) width=im_w; else width=di_w;
  if (im_h<di_h) height=im_h; else height=di_h;
  for (int y=0;y<height;y++)
    for (int x=0;x<width;x++) {
      int l=image.GetPixel(x,y).y;
      display.SetPixel(x,y,l,l,l,0);
    }
}

// }}}
// {{{ void DrawFunctions::Draw(DisplayBase& display, const ImageInterleave<unsigned char,4>& image)
#if 0
void DrawFunctions::Draw(DisplayBase& display, const ImageInterleave<unsigned char,4>& image) {
  //cout << "Draw Monoimage using generic code" << endl;
  // Only have base image, so must draw using (slow) primitives
  int im_w=image.GetWidth();
  int im_h=image.GetHeight();
  int di_w=display.GetWidth();
  int di_h=display.GetHeight();
  int width=0,height=0;
  if (im_w<di_w) width=im_w; else width=di_w;
  if (im_h<di_h) height=im_h; else height=di_h;
  for (int y=0;y<height;y++)
    for (int x=0;x<width;x++) {
      int l=image.GetPixel(x,y);
      display.SetPixel(x,y,l,l,l,0);
    }
}

// }}}
// {{{ void DrawFunctions::Draw(DisplayBase& display, const ImageInterleave<unsigned char,2>& image)

void DrawFunctions::Draw(DisplayBase& display, const ImageInterleave<unsigned char,2>& image) {
  //cout << "Draw Monoimage using generic code" << endl;
  // Only have base image, so must draw using (slow) primitives
  int im_w=image.GetWidth();
  int im_h=image.GetHeight();
  int di_w=display.GetWidth();
  int di_h=display.GetHeight();
  int width=0,height=0;
  if (im_w<di_w) width=im_w; else width=di_w;
  if (im_h<di_h) height=im_h; else height=di_h;
  for (int y=0;y<height;y++)
    for (int x=0;x<width;x++) {
      int l=image.GetPixel(x,y);
      display.SetPixel(x,y,l,l,l,0);
    }
}
#endif
// }}}
// {{{ void DrawFunctions::Draw(DisplayBase& display, const ImageRGB<unsigned char>& image)

void DrawFunctions::Draw(DisplayBase& display, const ImageRGB<unsigned char>& image) {
  //cout << "Draw RGBimage using generic code" << endl;
  // Only have base image, so must draw using (slow) primitives
  int im_w=image.GetWidth();
  int im_h=image.GetHeight();
  int di_w=display.GetWidth();
  int di_h=display.GetHeight();
  int width=0,height=0;
  if (im_w<di_w) width=im_w; else width=di_w;
  if (im_h<di_h) height=im_h; else height=di_h;
  for (int y=0;y<height;y++)
    for (int x=0;x<width;x++) {
      PixelRGB<unsigned char> p=image.GetPixel(x,y);
      display.SetPixel(x,y,p.r,p.g,p.b,0);
    }
}

// }}}


// Points
// {{{ void DrawFunctions::Draw(DisplayBase& display, const Point& p, Normalisation, size = 2, shape = PLUS)

void DrawFunctions::Draw(DisplayBase& display, const Point2D& pin, 
			 Normalisation C, float size, CornerShape shape)
{
  //cout << "Draw 2d-point using generic code" << endl;
  Point2D p = C.UnNormalise (pin,pin);

  const float x=float(p.GetX());
  const float xm=float(x-size);
  const float xp=float(x+size);
  const float y=float(p.GetY());
  const float ym=float(y-size);
  const float yp=float(y+size);

  switch(shape) {
  default:
  case CROSS:
    // Draw a point as a cross symbol
    display.DrawLine (xm, ym, xp, yp);
    display.DrawLine (xm, yp, xp, ym);
    break;
  case PLUS:
    // Draw a point as a plus symbol
    display.DrawLine(xm,y,xp,y);
    display.DrawLine(x,ym,x,yp);
    break;
  case SQUARE:
    // Draw a point as a plus symbol
    display.DrawLine(xm,ym,xp,ym);
    display.DrawLine(xp,ym,xp,yp);
    display.DrawLine(xp,yp,xm,yp);
    display.DrawLine(xm,yp,xm,ym);
    break;
  }
}
void DrawFunctions::Draw(DisplayBase& display, const std::vector<Point2D>& pl,
			 Normalisation C,
			 float size, CornerShape shape)
{
  std::vector<VW::Point2D>::const_iterator i = pl.begin(), ie = pl.end();
  for ( ; i != ie ; i++ ) Draw(display,*i,C,size,shape);
}

// }}}
// {{{ void DrawFunctions::Draw(DisplayBase& display, const Point& p, size, shape = PLUS)

void DrawFunctions::Draw(DisplayBase& display, const Point2D& pin, 
			 float size, CornerShape shape)
{
  Draw (display, pin, Normalisation(), size, shape);
}

// }}}
// {{{ void DrawFunctions::Draw(display, const CornerFeature& p, Normalisation, size = 2, shape = PLUS)

void DrawFunctions::Draw(DisplayBase& display, const CornerFeature& pin,
			 Normalisation C, float size, CornerShape shape)
{
  //cout << "Draw 2d-point using generic code" << endl;
  Point2D p = C.UnNormalise (pin);

  const float x=float(p.GetX());
  const float xm=x-(size*(pin.GetFeatureScale()));
  const float xp=x+(size*(pin.GetFeatureScale()));
  const float y=float(p.GetY());
  const float ym=y-(size*(pin.GetFeatureScale()));
  const float yp=y+(size*(pin.GetFeatureScale()));

  switch(shape) {
  default:
  case CROSS:
    // Draw a point as a cross symbol
    display.DrawLine (xm, ym, xp, yp);
    display.DrawLine (xm, yp, xp, ym);
    break;
  case PLUS:
    // Draw a point as a plus symbol
    display.DrawLine(xm,y,xp,y);
    display.DrawLine(x,ym,x,yp);
    break;
  case SQUARE:
    // Draw a point as a square
    display.DrawLine(xm,ym,xp,ym);
    display.DrawLine(xp,ym,xp,yp);
    display.DrawLine(xp,yp,xm,yp);
    display.DrawLine(xm,yp,xm,ym);
    break;
  }
}

// }}}
// {{{ void DrawFunctions::Draw(display, const SIFTFeature& p, Normalisation)

void DrawFunctions::Draw(DisplayBase& display, const SIFTFeature& pin,
			 Normalisation C)
{
  //cout << "Draw 2d-point using generic code" << endl;
  Point2D p = C.UnNormalise (pin);

  const float x=float(p.GetX());
  const float y=float(p.GetY());
  const float radius=pin.GetFeatureScale()*8;
  const float tx = x + radius*cos(pin.GetOrientation());
  const float ty = y + radius*sin(pin.GetOrientation());

  display.DrawCircle(x,y,radius);
  display.DrawLine(x,y,tx,ty);
  

}

void DrawFunctions::Draw(DisplayBase& display, 
			 const std::vector<VW::SIFTFeature>& pl,
			 Normalisation C)
{
  std::vector<VW::SIFTFeature>::const_iterator i = pl.begin(), ie = pl.end();
  for ( ; i != ie ; i++ ) Draw(display,*i,C);
}


// }}}
// {{{ void DrawFunctions::Draw(display, const PatchFeature& p, Normalisation)

void DrawFunctions::Draw(DisplayBase& display, const PatchFeature& pin,
			 Normalisation C)
{
  //cout << "Draw 2d-point using generic code" << endl;
  Point2D p = C.UnNormalise (pin);

  const float x=float(p.GetX());
  const float y=float(p.GetY());
  const float dimension = float(pin.GetPatchSize())*pin.GetFeatureScale();
  const float c = 0.5f*dimension*cos(pin.GetOrientation());
  const float s = 0.5f*dimension*sin(pin.GetOrientation());

  // radial
  display.DrawLine(x,y,x+s,y-c);
  // Box
  display.DrawLine(x+s-c, y-s-c,    x-s-c, y-s+c);
  display.DrawLine(x-s-c, y-s+c,    x-s+c, y+s+c);
  display.DrawLine(x-s+c, y+s+c,    x+s+c, y+s-c);
  display.DrawLine(x+s+c, y+s-c,    x+s-c, y-s-c);
  

}
void DrawFunctions::Draw(DisplayBase& display, 
			 const std::vector<VW::PatchFeature>& pl,
			 Normalisation C)
{
  std::vector<VW::PatchFeature>::const_iterator i = pl.begin(), ie = pl.end();
  for ( ; i != ie ; i++ ) Draw(display,*i,C);
}

// }}}

// {{{ void DrawFunctions::Draw(display, const CornerFeature& p, size, shape = PLUS)

void DrawFunctions::Draw(DisplayBase& display, const CornerFeature& pin,
			 float size, CornerShape shape)
{
  Draw (display, pin, Normalisation(), size, shape);
}

// }}}
// {{{ void DrawFunctions::Draw(display, corners, Normalisation, size = 2, shape = PLUS, *colours = 0)
void DrawFunctions::Draw(DisplayBase& display, const vector<CornerFeature>& corners,
			 Normalisation C,
			 float size, CornerShape shape, const vector<Colour> *colours)
{
  unsigned char orig_red, orig_green, orig_blue, orig_alpha;
  display.GetPenColour (orig_red, orig_green, orig_blue, orig_alpha);

  vector<Colour>::const_iterator col_it = colours->begin ();
  //if (colours != 0) col_it = colours->begin ();

  bool done = false;
  for (vector<CornerFeature>::const_iterator it = corners.begin ();
       it != corners.end (); it++) {
    const CornerFeature p = C.UnNormalise (*it, *it);

    const float x=float(p.GetX());
    const float xm=x-(size*(it->GetFeatureScale()));
    const float xp=x+(size*(it->GetFeatureScale()));
    const float y=float(p.GetY());
    const float ym=y-(size*(it->GetFeatureScale()));
    const float yp=y+(size*(it->GetFeatureScale()));

    if (colours != 0 && !done) {
      // If colours aren't provided, pen colour will just be unchanged
      // If provided array isn't long enough, remaining corners will be
      // original pen colour
      if (col_it == colours->end ()) {
	done = true;
	display.SetPenColour (orig_red, orig_green, orig_blue, orig_alpha);
      }
      else {
	display.SetPenColour (col_it->_r, col_it->_g, col_it->_b, col_it->_a);
	col_it++;
      }
    }

    switch(shape) {
    default:
    case CROSS:
      // Draw a point as a cross symbol
      display.DrawLine (xm, ym, xp, yp);
      display.DrawLine (xm, yp, xp, ym);
      break;
    case PLUS:
      // Draw a point as a plus symbol
      display.DrawLine(xm,y,xp,y);
      display.DrawLine(x,ym,x,yp);
      break;
    case SQUARE:
      // Draw a point as a plus symbol
      display.DrawLine(xm,ym,xp,ym);
      display.DrawLine(xp,ym,xp,yp);
      display.DrawLine(xp,yp,xm,yp);
      display.DrawLine(xm,yp,xm,ym);
      break;
    }
  }
}

// }}}

  // Lines
// {{{ void DrawFunctions::Draw(DisplayBase& display, const LineSeg2D& ls, Normalisation)

void DrawFunctions::Draw(DisplayBase& display, const LineSeg2D& ls, Normalisation C)
{
  //cout << "Draw 2d-line-segment using generic code" << endl;
  Point2D sp = C.UnNormalise (ls.sp);
  Point2D ep = C.UnNormalise (ls.ep);
  display.DrawLine(float(sp.GetX()), float(sp.GetY()), float(ep.GetX()), float(ep.GetY()));
}

// }}}
// {{{ void DrawFunctions::Draw(DisplayBase& display, const Line2D& l, Normalisation)

void DrawFunctions::Draw (DisplayBase& display, const Line2D& lin, Normalisation C)
{
  //cout << "Draw 2d general line feature using generic code" << endl;
  Line2D l = C.UnNormalise (lin, lin);
  VW::LineSeg2D lineseg = l.Truncate(0,0,
				     display.GetWidth(),
				     display.GetHeight());
  Draw(display,lineseg);
}

// }}}
// {{{ void DrawFunctions::Draw(DisplayBase& display, const EdgeString& estring)

void DrawFunctions::Draw(DisplayBase& display,const  EdgeString& estring) 
{
  deque<Edgel *>::const_iterator e;

  for (e=estring.begin(); e!=estring.end(); e++) {
    display.SetPixel((*e)->GetIX(), (*e)->GetIY());
  }
}

// }}}


// {{{ void DrawFunctions::Draw(DisplayBase& display, const EdgeData& edat)

void DrawFunctions::Draw(DisplayBase& display,const  EdgeData& edat) 
{
  vector<Edgel *>  edgels = edat.edgels;
  vector<Edgel *>::const_iterator e;

  for (e=edgels.begin(); e!=edgels.end(); e++) {
    if ((*e)->GetLinked())
      display.SetPixel((*e)->GetIX(), (*e)->GetIY(), 0, 255, 0, 0);
    else
      display.SetPixel((*e)->GetIX(), (*e)->GetIY(), 255, 0, 0, 0);
  }
}

// }}}


// Trajectories
// {{{ void DrawFunctions::Draw(DisplayBase& display, const Trajectory<Point2D>& t)

void DrawFunctions::Draw(DisplayBase& display, const Trajectory<Point2D>& t,
			 Normalisation C, bool draw_backwards) {
  
  unsigned char r,g,b,a;

  display.GetPenColour(r,g,b,a);

  display.SetPenColour(r/2,g/2,b/2,a);

  // Draw the tail (half-brightness)
  if (t.size()>1) {
    if (draw_backwards) {
      Point2D last_p;
      last_p=C.UnNormalise (t.GetOldestFeature());
      for (int i=t.size()-1;i>=0;i--) {
	Point2D p = C.UnNormalise (t.GetOldFeature(i));
	display.DrawLine(float(p.GetX()),float(p.GetY()),
			 float(last_p.GetX()),float(last_p.GetY()));
	last_p=p;
      }
    }
    else {
      Point2D last_p;
      last_p = C.UnNormalise (t.GetNewestFeature());
       for (unsigned int i=1;i<t.size();i++) {
	Point2D p = C.UnNormalise (t.GetOldFeature(i));
	display.DrawLine(float(p.GetX()),float(p.GetY()),
			 float(last_p.GetX()),float(last_p.GetY()));
	last_p=p;
      }
    }
  }

  display.SetPenColour(r,g,b,a);

  // Draw the end-point as a plus symbol
  //  Point2D p = draw_backwards ? t.GetEnd() : t.GetStart();
  //  Draw(display,p);
}

// }}}
// {{{ void DrawFunctions::Draw(DisplayBase& display, const Trajectory<Point2D>& t)

void DrawFunctions::Draw(DisplayBase& display, const Trajectory<Point2D>& t,
			 bool draw_backwards)
{
  Draw (display, t, Normalisation(), draw_backwards);
}

// }}}
// {{{ void DrawFunctions::Draw(DisplayBase& display, const Trajectory<PlesseyCorner>& t)

void DrawFunctions::Draw(DisplayBase& display, const Trajectory<PlesseyCorner>& t,
			 Normalisation C, bool draw_backwards) {
  
  unsigned char r,g,b,a;
  unsigned char tr,tg,tb;

  display.GetPenColour(r,g,b,a);
  tr=r;tg=g;tb=b;


  if (t.size()>1) {
    unsigned int s=t.size()-1;
    unsigned int ts=1;
    if (draw_backwards) {
      Point2D last_p;
      last_p=C.UnNormalise (t.GetOldestFeature());
      for (int i=int(t.size()-1);i>=0;i--) {
	 // Scale the colour
	float col_scale=float(ts)/float(s);
	ts++;
	tr=static_cast<unsigned char>(col_scale*float(r));
	tg=static_cast<unsigned char>(col_scale*float(g));
	tb=static_cast<unsigned char>(col_scale*float(b));
	display.SetPenColour(tr,tg,tb,a);

	// Do the work
	Point2D p = C.UnNormalise (t.GetOldFeature(i));
	display.DrawLine(float(p.GetX()),float(p.GetY()),
			 float(last_p.GetX()),float(last_p.GetY()));
	last_p=p;
      }
    }
    else {
      Point2D last_p;
      last_p = C.UnNormalise (t.GetNewestFeature());
       for (unsigned int i=1;i<t.size();i++) {
	 // Scale the colour
	float col_scale=float(ts)/float(s);
	ts++;
	tr=static_cast<unsigned char>(col_scale*float(r));
	tg=static_cast<unsigned char>(col_scale*float(g));
	tb=static_cast<unsigned char>(col_scale*float(b));
	display.SetPenColour(tr,tg,tb,a);

	// Do the work
	Point2D p = C.UnNormalise (t.GetOldFeature(i));
	display.DrawLine(float(p.GetX()),float(p.GetY()),
			 float(last_p.GetX()),float(last_p.GetY()));
	last_p=p;
      }
    }
  }


  display.SetPenColour(r,g,b,a);

  // Draw the end-point as a plus symbol
  //  PlesseyCorner p = draw_backwards ? t.GetEnd() : t.GetStart();
  //  Draw(display,p);
}

// }}}
// {{{ void DrawFunctions::Draw(DisplayBase& display, const Trajectory<PlesseyCorner>& t)

void DrawFunctions::Draw(DisplayBase& display, const Trajectory<PlesseyCorner>& t,
			 bool draw_backwards)
{
  Draw (display, t, Normalisation(), draw_backwards);
}

// }}}

void DrawFunctions::Draw(VW::DisplayBase& display, 
			 const VW::Point2D& location,
			 const VW::Covariance2D& covar)
{
  VNL::MatrixFixed<2,2,double> lower_triangle;
  if (covar.CholeskyDecompose(lower_triangle)) {
    display.DrawEllipse(float(location.GetX()), 
			float(location.GetY()),
			lower_triangle[0][0], 
			lower_triangle[1][1], 
			lower_triangle[1][0]);
  }
  
}

void DrawFunctions::DrawCovariance(DisplayBase& display, 
											  const Point2D& location,
											  const VNL::MatrixFixed<2,2,double>& covar)
{
  // Take account of small deviations from symmetry
  const float c01 = 0.5f*float(covar[0][1]+covar[1][0]);
  float rx=0.0f;
  float ry=0.0f;
  float skew=0.0f;
  bool error = false;
  
  // Now do a mini Cholesky decomposition
  // result is  U = [rx skew
  //                  0   ry]
  // so that covar = U'*U
  if (covar[0][0]<=0.0) {
    error = true;
  } else {
    rx = sqrt(float(covar[0][0]));
    skew = c01 / rx;
    const float skew2 = skew*skew;
    
    if (skew2>covar[1][1]) {
      error = true;
    } else {
      ry = sqrt(float(covar[1][1]) - skew2);
    }
  }
  if (error) {
    cerr << "DrawFunctions::DrawCovariance: Covariance is not positivie definite!" << endl;
    cerr << "   C=[\n" << covar << "]" << endl;
  } else {
    display.DrawEllipse(float(location.GetX()), float(location.GetY()),
			rx, ry, skew);
  }
  
}
void 
DrawFunctions::Draw(DisplayBase& display, 
     const std::list<LineSeg2D>& ls)
{
  //  std::cerr << "Drawing " << ls.size() << " line segs"<<std::endl;
  std::list<LineSeg2D>::const_iterator i = ls.begin();
  std::list<LineSeg2D>::const_iterator e = ls.end();
  for ( ; i != e ; i++) {
    Draw(display, *i);
  }
}

void 
DrawFunctions::Draw(DisplayBase& display, 
     const std::list<LineSegFitted>& ls)
{
  //  std::cerr << "Drawing " << ls.size() << " line segs"<<std::endl;
  std::list<LineSegFitted>::const_iterator i = ls.begin();
  std::list<LineSegFitted>::const_iterator e = ls.end();
  for ( ; i != e ; i++) {
    Draw(display, VW::LineSeg2D(*i));
  }
}

void VW::DrawFunctions::
DrawRectangleOutline(DisplayBase& display,
		     unsigned int x0, unsigned int y0,
		     unsigned int x1, unsigned int y1,
		     unsigned int linewidth)
{
  for (unsigned int i=0;i<linewidth;i++) {
    // Top
    display.DrawLine(x0,y0+i,x1,y0+i);
    // Left
    display.DrawLine(x0+i,y0,x0+i,y1);
    // Right
    display.DrawLine(x1-i,y0,x1-i,y1);
    // Bottom
    display.DrawLine(x0,y1-i,x1,y1-i);
  }

}
