/** 
 * @file   classifieruv.h
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
 * @class   VW::ClassifierUV 
 * @ingroup ImageProcessing
 * @author  BJ Tordoff
 * @date    02/08/02
 * @brief   A class to store a classification histogram, and providing
 *          classification of pixels or whole images.
 */


#ifndef _classifieruv_h_
#define _classifieruv_h_

#include <string>
#include <sstream>
#include "VW/image.h"
#include "VW/iobasics.h"

namespace VW {

class ClassifierUV : public VW::IOObject {
public:

  ClassifierUV();
  /** Construct and load from a file. */
  ClassifierUV(const std::string& filename);
  virtual ~ClassifierUV();

  /** Set to a new size (and fill with CLASSIFIER_OTHER). */
  void Reset(unsigned int new_bin_size=1,
	     unsigned int new_num_classes=0);


  // Basic classifiers/probs


  /** Classify a given UV pair. */
  unsigned int 
  Classify(unsigned char u, unsigned char v) const;

  void          
  Classify(const VW::ImageYUV<unsigned char>& input_image,
	   VW::ImageMono<unsigned char>& classification) const;


  /** Get the probability that a given UV pair belongs to @which_class. */
  float GetProb(unsigned char u, 
		unsigned char v, 
		unsigned int which_class) const;

  /** Get the probability that each pixel belongs to @which_class. 
   * resulting pixels have values between 0 and 1 inclusive.
   */
  void GetProbImage(const VW::ImageYUV<unsigned char>& input_image,
		    unsigned int which_class,
		    VW::ImageMono<float>& prob) const;
  /** Get the probability that each pixel belongs to @which_class. 
   * resulting pixels have values between 0 and 255 inclusive.
   */
  void GetProbImage(const VW::ImageYUV<unsigned char>& input_image,
		    unsigned int which_class,
		    VW::ImageMono<unsigned char>& prob) const;


  // More fancy versions

  // Sometimes we want to draw the result for human interpretation, so 
  // a colourful output is required.
  void          
  ClassifyAndColour(const VW::ImageYUV<unsigned char>& input_image,
		    VW::ImageRGB<unsigned char>& classification) const;

  void
  GetHistogramAsImage(VW::ImageRGB<unsigned char>& histogram) const;


  // Sometimes we might want to tweak the array directly
  unsigned int** GetClassifierMatrix();

  // Streaming methods

  /** Stream out as plain ASCII */
  virtual 
  bool WriteASCII(std::ostream& stream) const; // Also provides '<<'
  /** Stream out as XML style ASCII */
  virtual 
  bool WriteXML(std::ostream& stream,
			 const std::string& name="") const;
  /** Stream out as binary */
  virtual 
  bool WriteBinary(std::ostream& stream) const;


  /** Stream in as plain ASCII */
  virtual 
  bool ReadASCII(std::istream& stream); // Also provides '<<'
  /** Stream in as XML style ASCII */
  virtual 
  bool ReadXML(std::istream& stream, 
			const std::string& name="");
  /** Stream in as binary */
  virtual 
  bool ReadBinary(std::istream& stream);

  /** Load an existing data-file (same as ReadXMLFile()). */
  virtual void LoadParameterFile(std::string filename);
  /** Save into data-file (same as WriteXMLFile()). */
  virtual void SaveParameterFile(std::string filename);
protected:

  // FUNCTIONS
  void Colourify(unsigned int class_index,
		 unsigned char& r,
		 unsigned char& g,
		 unsigned char& b) const; 
  // DATA
  unsigned int** classifier_matrix;
  std::vector<float**> prob_matrix;
  unsigned int num_classes;
  unsigned int rows, columns;
  unsigned int my_bin_size;
  float u_scale,v_scale; // multiplier for u,v to get bin location
};



  /** A function to load a list of pixel data and output a 
   * classification matrix.
   */
  void PixelList2ClassifierUVMatrix(const std::string& pixellist_filename,
				    const std::string& matrix_filename,
				    unsigned int binsize = 2);
  
}; // end namespace VW


// INLINERS

inline VW::ClassifierUV::ClassifierUV(const std::string& filename)
{
  ReadXMLFile(filename);
}


inline
unsigned int** VW::ClassifierUV::GetClassifierMatrix()
{
  return (classifier_matrix);
}
inline void VW::ClassifierUV::Colourify(unsigned int class_index,
					unsigned char& r,
					unsigned char& g,
					unsigned char& b) const
{
    switch(class_index) {
    default:
    case 0:
      r=255;  g=255;  b=255;
      break;
    case 1:
      r=0;    g=0;    b=0;
      break;
    case 2:
      r=255;  g=0;    b=0;
      break;
    case 3:
      r=0;    g=255;  b=0;
      break;
    case 4:
      r=0;    g=0;    b=255;
      break;
    case 5:
      r=255;  g=255;  b=0;
      break;
    case 6:
      r=255;  g=0;    b=255;
      break;
    case 7:
      r=0;    g=255;  b=255;
      break;
    }
}

inline unsigned int VW::ClassifierUV::
Classify(unsigned char u, unsigned char v) const 
{
  const int binu=int(floor(u*u_scale));
  const int binv=int(floor(v*v_scale));
  return classifier_matrix[binv][binu];
}

inline float VW::ClassifierUV::
GetProb(unsigned char u, unsigned char v, unsigned int which_class) const 
{
  const int binu=int(floor(u*u_scale));
  const int binv=int(floor(v*v_scale));
  return prob_matrix[which_class][binv][binu];
}

#endif
