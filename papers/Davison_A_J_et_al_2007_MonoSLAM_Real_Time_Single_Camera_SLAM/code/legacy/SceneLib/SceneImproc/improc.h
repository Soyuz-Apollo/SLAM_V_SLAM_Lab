/*  SceneLib: software for real-time SLAM

    improc.h
    Copyright (C) 2005 University of Oxford

    Author
    Andrew Davison
    ajd@doc.ic.ac.uk

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _scene_improc_h_
#define _scene_improc_h_

#include <vector>

#include <VNL/matrix.h>
#include <VNL/matrixfixed.h>
#include <VW/image.h>

/**
@addtogroup gSceneImproc
@{
*/

/// Threshold for stereo correlation using correlate2()
const double CORRTHRESH2 = 0.40;
/// Half-width of the strip we will search for stereo matches
const unsigned int LINESEARCHWIDTH = 8;
/// Value for the standard deviation of the intensity values in a patch
/// below which we deem it unsuitable for correlation
const double CORRELATION_SIGMA_THRESHOLD = 10.0;
/// Number of standard deviations to search within images
const double NO_SIGMA = 3.0;
/// Hackish value we add to correlation score to penalise if patch sigma
/// is low
const double LOW_SIGMA_PENALTY = 5.0;

void find_best_patch_inside_region(
  const VW::ImageMono<unsigned char> &image, 
  unsigned int *ubest, unsigned int *vbest,
  double *evbest,
  const unsigned int BOXSIZE,
  unsigned int ustart, unsigned int vstart,
  unsigned int ufinish, unsigned int vfinish);

void find_best_n_patches_inside_region(
  const VW::ImageMono<unsigned char> &image,
  const unsigned int n,
  unsigned int *ubest, unsigned int *vbest,
  double *evbest,
  const unsigned int BOXSIZE,
  unsigned int ustart, unsigned int vstart,
  unsigned int ufinish, unsigned int vfinish);

bool elliptical_search(
  const VW::ImageMono<unsigned char> &image, 
  const VW::ImageMono<unsigned char> &patch,
  const VNL::VectorFixed<2, double> centre,
  const VNL::MatrixFixed<2,2,double> &PuInv,
  unsigned int *u, unsigned int *v,
  const unsigned int BOXSIZE);

/**
Handles the correlation search for an image patch over a set of overlapping
ellipses. This class makes the search more efficient by only correlating at
each image location once.

The class should be constructed by passing the image and patch details, and
then each ellipse added by calling add_ellipse(). Calling search() performs the
correlation, and then the results can be extracted by iterating over the
SearchDatum data, for example
@code
SearchMultipleOverlappingEllipses ellipse_search(image, patch);
// Add the ellipses here
...
// Now do the search and extract the data
ellipse_search.search();
for(SearchMultipleOverlappingEllipses::SearchData::const_iterator i =
    ellipse_search.begin(); i != elliptical_search.end(); i++)
{
  if(i->result_flag) // If this ellipse found a match
  {
    // Do stuff with it
  }
}
@endcode
*/
class SearchMultipleOverlappingEllipses
{
public:
  /** Constructor.
  @param image The image to search
  @param patch The image patch to search for
  @param BOXSIZE The size of the image patch to use
  */
  SearchMultipleOverlappingEllipses(
      const VW::ImageMono<unsigned char> &image,
      const VW::ImageMono<unsigned char> &patch,
      const unsigned int BOXSIZE) :
    m_image(&image),
    m_patch(&patch),
    m_boxsize(BOXSIZE)
  {}

  virtual ~SearchMultipleOverlappingEllipses() {}
  
  void add_ellipse(
    const VNL::MatrixFixed<2,2,double> &PuInv,
    const VNL::VectorFixed<2,double>& search_centre);

  virtual void search();

  /**
  Structure to hold the data for a particular ellipse. */
  struct SearchDatum
  {
  friend class SearchMultipleOverlappingEllipses;
  private:
    SearchDatum(const VNL::MatrixFixed<2,2,double>& _PuInv,
                const VNL::VectorFixed<2,double>& _search_centre);

  public:
    int minx() {return int(search_centre[0] + 0.5) - halfwidth;}
    int maxx() {return int(search_centre[0] + 0.5) + halfwidth;}
    int miny() {return int(search_centre[1] + 0.5) - halfwidth;}
    int maxy() {return int(search_centre[1] + 0.5) + halfwidth;}

    bool inside_fast(int u, int v);
    bool inside(int u, int v);
    bool inside_relative(int u, int v);
    
  public:
    /// The inverse covariance. Set in the constructor via add_ellipse()
    VNL::MatrixFixed<2,2,double> PuInv;
    /// The search centre. Set in the constructor via add_ellipse()
    VNL::VectorFixed<2,double> search_centre;
    /// Set to <code>true</code> after search() if a successful match was found
    bool result_flag;
    /// The x-location of the successful match, if found
    unsigned int result_u;
    /// The y-location of the successful match, if found
    unsigned int result_v;
    /** Half the width of the search ellipse. Used to quickly decide where to
    search */
    unsigned int halfwidth;
    /** Half the height of the search ellipse. Used to quickly decide where to
    search */
    unsigned int halfheight;
  };

  /// Typdef the SearchDatum container for convenience
  typedef std::vector<SearchDatum> SearchData;

  /// How many ellipses are registered with the class?
  unsigned int size() const {return m_searchdata.size();}
  /// Iterator set to the start of the container of ellipse data
  SearchData::const_iterator begin() const {return m_searchdata.begin();}
  /// Iterator set to one past the end of the container of ellipse data
  SearchData::const_iterator end() const {return m_searchdata.end();}

  const VW::ImageMono<unsigned char>* image() const {return m_image;}
  const VW::ImageMono<unsigned char>* patch() const {return m_patch;}
  unsigned int boxsize() const {return m_boxsize;}
  SearchData& searchdata() {return m_searchdata;}
  
private:
  const VW::ImageMono<unsigned char>* m_image;
  const VW::ImageMono<unsigned char>* m_patch;
  unsigned int m_boxsize;
  SearchData m_searchdata;
};

///////////////////////
// These should be in VW!
// (Shamelessly copied from libcvd https://savannah.nongnu.org/projects/libcvd/)
/**
Image utilities shamelessly copied from the CVD library
(https://savannah.nongnu.org/projects/libcvd/). These should really be in VW.
*/
namespace ImageUtil
{
  template<class T> inline void memfill(T* data, int n, const T val)
  {
    T* de = data + n;
    for(;data < de; data++)
      *data=val;
  }

  template<> inline void memfill(unsigned char* data, int n, const unsigned char
val)
  {
    memset(data, val, n);
  }

  template<> inline void memfill(signed char* data, int n, const signed char
val)
  {
    memset(data, val, n);
  }

  template<> inline void memfill(char* data, int n, const char val)
  {
    memset(data, val, n);
  }
}

/**
Fill an image with a certain value. Shamelessly copied from the
CVD library (https://savannah.nongnu.org/projects/libcvd/). This should really
be in VW.
*/
template<typename T>
void fill_image(VW::ImageMono<T>& image, T val)
{
  ImageUtil::memfill(
    reinterpret_cast<T*>(image.GetRawBuffer()),
                         image.GetHeight()*image.GetWidth(), val);
}
/**
@}
*/
#endif
