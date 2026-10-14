/** @file   drawimage.cc 
 *  @author BJ Tordoff
 *  @date   02/08/02
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


#include <VW/image.h>
#include <VW/Improc/plesseycorner.h>
#include <VW/Improc/linesegfitted.h>

#include "drawimage.h"

using namespace std;

typedef unsigned int uint;

#ifdef _WIN32
#define min __min
#endif

#define PI 3.14159265358979323846


namespace VW {
  namespace DrawImage {

template <class ImageType>
void DrawRectangle(ImageType& image,
				      unsigned int x0, unsigned int y0,
				      unsigned int x1, unsigned int y1) 
{
  typename ImageType::PixelType colour = image.GetPenColour();

  for (unsigned int y=y0;y<y1;y++) {
    typename ImageType::PixelPtr ptr = image.GetRowStart(y);
    ptr += x0;
    for (unsigned int x=x0;x<x1;x++,ptr++) {
      *ptr = colour;
    }
  }
}
    template <class ImageType>
    void DrawRectangleOutline(ImageType& image,
			      unsigned int x0, unsigned int y0,
			      unsigned int x1, unsigned int y1,
			      unsigned int linewidth)
{
  typename ImageType::PixelType colour = image.GetPenColour();

  { // Top
    for (unsigned int y=y0;y<(y0+linewidth);y++) {
      typename ImageType::PixelPtr ptr = image.GetRowStart(y) + x0;
      for (unsigned int x=x0;x<=x1;x++,ptr++) {
	*ptr = colour;
      }
    }
  }

  { // Left+right
    for (unsigned int y=y0;y<=y1;y++) {
      typename ImageType::PixelPtr ptr = image.GetRowStart(y) + x0;
      for (unsigned int x=0;x<linewidth;x++,ptr++) {
	*ptr = colour;
      }
      ptr = image.GetRowStart(y) + x1 - linewidth + 1;
      for (unsigned int x=0;x<=linewidth;x++,ptr++) {
	*ptr = colour;
      }
    }
  }


  { // Bottom
    for (unsigned int y=(y1-linewidth+1);y<=y1;y++) {
      typename ImageType::PixelPtr ptr = image.GetRowStart(y) + x0;
      for (unsigned int x=x0;x<x1;x++,ptr++) {
	*ptr = colour;
      }
    }
  }
}

template <class ImageType> void 
DrawRandomRectangles(ImageType& image,
					unsigned int number_of_rectangles) 
{
    int min_size=10;
    VW::PixelRGB<unsigned char> c(0,0,0,0);
    typename ImageType::PixelType colour = image.GetPenColour();
    for (unsigned int i=0;i<number_of_rectangles;i++) {
      unsigned int x0 = uint (floor ((image.GetWidth()-min_size) * double (rand()) / (double(RAND_MAX) + 1.0)));
      unsigned int y0 = uint (floor ((image.GetHeight()-min_size) * double (rand()) / (double(RAND_MAX) + 1.0)));
      unsigned int w = uint (floor ((image.GetWidth()-min_size-x0) * double (rand()) / (double(RAND_MAX) + 1.0)));
      unsigned int h = uint (floor ((image.GetHeight()-min_size-y0) * double (rand()) / (double(RAND_MAX) + 1.0)));
      c.r = uint (floor (255 * double (rand()) / (double(RAND_MAX) + 1.0)));
      c.g = uint (floor (255 * double (rand()) / (double(RAND_MAX) + 1.0)));
      c.b = uint (floor (255 * double (rand()) / (double(RAND_MAX) + 1.0)));

      colour = c;
      image.SetPenColour(colour);
      DrawRectangle(image,
		    x0,y0,
		    x0+w+min_size,y0+h+min_size);

  }
}

template <class ImageType> void 
DrawCircle(ImageType& image, 
	   double x0, double y0, 
	   double radius)
{
  static const double NUM_CIRC_POINTS = 32.0;
  const double theta_step = 2.0 * PI / NUM_CIRC_POINTS;
  const double theta_limit = 6.283185 + theta_step; // Just over 2pi

  int last_col = int(x0+radius+0.5);
  int last_row = int(y0+0.5);
  const int max_col = image.GetWidth()-1;
  const int max_row = image.GetHeight()-1;
  for (double theta=theta_step; theta<theta_limit; theta+=theta_step) {
    const int col = int(x0+radius*cos(theta)+0.5);
    const int row = int(y0+radius*sin(theta)+0.5);
    if ((col>max_col) || (col<0) 
	|| (row>max_row) || (row<0)) {
      // skip this and next
      theta+=theta_step;
      last_col = int(x0+radius*cos(theta)+0.5);
      last_row = int(y0+radius*sin(theta)+0.5);
    } else if ((last_col>max_col) || (last_col<0) 
	|| (last_row>max_row) || (last_row<0)) {
      // skip this
      last_col = int(x0+radius*cos(theta)+0.5);
      last_row = int(y0+radius*sin(theta)+0.5);
    } else {
  //    cerr<<"Drawline ("<<last_col<<","<<last_row<<") - ("<<col<<","<<row<<")\n";
      image.DrawLine(last_col, last_row, col, row);
      last_col = col;
      last_row = row;
    }
  }

}

void Draw(const ImageRGB<unsigned char>& foreground,
	  ImageRGB<unsigned char>& background,
	  unsigned int x0, unsigned int y0,
	  double alpha)
{
  assert((alpha>=0) && (alpha<=1));

  // First work out how much of the foreground will fit
  unsigned int draw_width = min(background.GetWidth() - x0, foreground.GetWidth());
  unsigned int draw_height = min(background.GetHeight() - y0, foreground.GetHeight());

  unsigned int fy=0;
  for (unsigned int y = y0; y < (y0+draw_height); y++,fy++) {
    unsigned int fx=0;
    for (unsigned int x = x0; x < (x0+draw_width); x++,fx++) {
      VW::PixelRGB<unsigned char> b=background.GetPixel(x,y);
      VW::PixelRGB<unsigned char> f=foreground.GetPixel(fx,fy);
      b.ScaleIntensity(alpha);
      f.ScaleIntensity(1-alpha);

      background.SetPixel(x, y,  b + f );
    }
  }
}

void Draw(const ImageRGB<unsigned char>& foreground,
	  ImageRGB<unsigned char>& background,
	  unsigned int x0, unsigned int y0,
	  const VW::PixelRGB<unsigned char>& transparent_colour)
{
  // First work out how much of the foreground will fit
  unsigned int draw_width = min(background.GetWidth() - x0, foreground.GetWidth());
  unsigned int draw_height = min(background.GetHeight() - y0, foreground.GetHeight());

  unsigned int fy=0;
  for (unsigned int y = y0; y < (y0+draw_height); y++,fy++) {
    unsigned int fx=0;
    for (unsigned int x = x0; x < (x0+draw_width); x++,fx++) {
      VW::PixelRGB<unsigned char> f=foreground.GetPixel(fx,fy);
      // cout << "Compare ("<<int(f.r)<<","<<int(f.g)<<","<<int(f.b)<<","<<int(f.alpha)<<") with ("<<int(transparent_colour.r)<<","<<int(transparent_colour.g)<<","<<int(transparent_colour.b)<<","<<int(transparent_colour.alpha)<<")"<<endl;

      if (f!=transparent_colour) {
	background.SetPixel(x, y,  f );
      }
    }
  }  
}

template <class ImageType>
void Draw(ImageType& image, 
	  const std::vector<VW::PlesseyCorner>& corners,
	  VW::Normalisation C,
	  float cross_size)
{
  std::vector<VW::PlesseyCorner>::const_iterator i = corners.begin();
  std::vector<VW::PlesseyCorner>::const_iterator ie = corners.end();
  for ( ; i!=ie ; i++ )
    VW::DrawImage::Draw(image, VW::Point2D(*i), C, cross_size);
}
    
template <class ImageType>
void Draw(ImageType& image, 
	  const LineSeg2D& ls, 
	  Normalisation C)
{
  VW::LineSeg2D ls2 = C.UnNormalise (ls);
  ls2=ls2.Truncate(0,0,image.GetWidth()-1,image.GetHeight()-1);

  image.DrawLine(ls2.GetStartX(), ls2.GetStartY(),
		 ls2.GetEndX(), ls2.GetEndY());
		 

}

template <class ImageType>
void Draw(ImageType& image, 
			     const Point2D& p, 
			     Normalisation C, float cross_size)
{
  const VW::Point2D new_p = C.UnNormalise(p);
  const int x = int(new_p.GetX()+0.5);
  const int y = int(new_p.GetY()+0.5);
  image.DrawLine(x-cross_size,y,x+cross_size,y);
  image.DrawLine(x,y-cross_size,x,y+cross_size);

}
    
template <class ImageType>
void Draw(ImageType& image, 
	  const Line2D& lin, 
	  Normalisation C)
{
  VW::Line2D l = C.UnNormalise(lin);
  VW::LineSeg2D lineseg = l.Truncate(0,0,
				     image.GetWidth()-1,
				     image.GetHeight()-1);

  //cerr << "Draw line (" << lineseg << ")" << endl;

  image.DrawLine(lineseg.GetStartX(), lineseg.GetStartY(),
		 lineseg.GetEndX(), lineseg.GetEndY());

}



#ifdef INSTANTIATE
#  undef INSTANTIATE
#endif
# define INSTANTIATE(M) \
      template void Draw<M >(M&, const VW::Line2D&, VW::Normalisation); \
      template void Draw<M >(M&, const VW::LineSeg2D&, VW::Normalisation); \
      template void Draw<M >(M&, const VW::Point2D&, VW::Normalisation, float); \
      template void Draw<M >(M&, const std::vector<VW::PlesseyCorner>&, VW::Normalisation, float); \
      template void DrawCircle<M >(M&,double,double,double); \
      template void DrawRectangle<M >(M&,unsigned int,unsigned int,unsigned int,unsigned int); \
  template void DrawRectangleOutline<M >(M&,unsigned int,unsigned int,unsigned int,unsigned int,unsigned int); \
      template void DrawRandomRectangles<M >(M&,unsigned int);


INSTANTIATE(VW::ImageMono<unsigned char>);
INSTANTIATE(VW::ImageRGB<unsigned char>);
INSTANTIATE(VW::ImageYUV<unsigned char>);


#undef INSTANTIATE


  };// End namespace DrawImage
}; // End namespace VW
