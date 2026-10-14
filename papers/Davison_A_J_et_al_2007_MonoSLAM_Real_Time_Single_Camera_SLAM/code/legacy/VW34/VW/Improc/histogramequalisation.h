/** 
 * @file    histogramequalisation.h
 * @author  BJ Tordoff
 * @date    05/04/2005
 *
 *              (c) Copyright University of Cambridge
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
 * @class   VW::HistogramEqualisation
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    05/04/2005
 * @brief   Standard histogram equalisation of image intensities
 */


#ifndef _histogramequalise_h_
#define _histogramequalise_h_

#include <vector>
#include <VW/Image/imagemono.h>
#include <VW/Image/imageyuv.h>

namespace VW {

  class HistogramEqualisation {
  public:
    /** Construct without initialising. */
    HistogramEqualisation() {}

    /** Calculate te histogram and apply immediately to this image. */
    HistogramEqualisation(VW::ImageMono<unsigned char>& image);

    /** Calculate te histogram and apply immediately to this image. */
    HistogramEqualisation(VW::ImageYUV<unsigned char>& image);

    /** Calculate from inputimage and store equalised result in
     * resultimge.
     */
    HistogramEqualisation(const VW::ImageMono<unsigned char>& inputimage,
			  VW::ImageMono<unsigned char>& resultimage);

    /** Calculate from inputimage and store equalised result in
     * resultimge.
     */
    HistogramEqualisation(const VW::ImageYUV<unsigned char>& inputimage,
			  VW::ImageYUV<unsigned char>& resultimage);

    /** Recompute the histogram from an image. */
    void ComputeFromImage(const VW::ImageMono<unsigned char>& inputimage);

    /** Recompute the histogram from an image. */
    void ComputeFromImage(const VW::ImageYUV<unsigned char>& inputimage);

    /** Add a new image to the histogram. */
    void AddImage(const VW::ImageMono<unsigned char>& inputimage);

    /** Add a new image to the histogram. */
    void AddImage(const VW::ImageYUV<unsigned char>& inputimage);

    /** Apply a previosuly computed histogram to a new image. */
    void ApplyToImage(VW::ImageMono<unsigned char>& resultimage);

    /** Apply a previosuly computed histogram to a new image. */
    void ApplyToImage(VW::ImageYUV<unsigned char>& resultimage);

    /** Apply a previosuly computed histogram to a new image. */
    void ApplyToImage(const VW::ImageMono<unsigned char>& inputimage,
		      VW::ImageMono<unsigned char>& resultimage);

    /** Apply a previosuly computed histogram to a new image. */
    void ApplyToImage(const VW::ImageYUV<unsigned char>& inputimage,
		      VW::ImageYUV<unsigned char>& resultimage);

    

  protected: // Funcs
    void ResetArrays();

  protected: // Data
    unsigned int _totalnum;
    std::vector<unsigned int> _bin_contents;
    std::vector<unsigned char> _hash;
  };

}; // End namespace VW









inline VW::HistogramEqualisation::
HistogramEqualisation(VW::ImageMono<unsigned char>& image)
{
  ComputeFromImage(image);
  ApplyToImage(image);
}

inline VW::HistogramEqualisation::
HistogramEqualisation(VW::ImageYUV<unsigned char>& image)
{
  ComputeFromImage(image);
  ApplyToImage(image);
}

inline VW::HistogramEqualisation::
HistogramEqualisation(const VW::ImageMono<unsigned char>& inputimage,
		      VW::ImageMono<unsigned char>& resultimage)
{
  ComputeFromImage(inputimage);
  ApplyToImage(inputimage, resultimage);
}

inline VW::HistogramEqualisation::
HistogramEqualisation(const VW::ImageYUV<unsigned char>& inputimage,
		      VW::ImageYUV<unsigned char>& resultimage)
{
  ComputeFromImage(inputimage);
  ApplyToImage(inputimage, resultimage);
}

inline void VW::HistogramEqualisation::
ComputeFromImage(const VW::ImageMono<unsigned char>& inputimage)
{
  ResetArrays();

  AddImage(inputimage);

}

inline void VW::HistogramEqualisation::
ComputeFromImage(const VW::ImageYUV<unsigned char>& inputimage)
{
  ResetArrays();

  AddImage(inputimage);

}

#endif
