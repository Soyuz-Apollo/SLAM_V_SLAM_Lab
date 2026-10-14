/** 
 * @file   classifieruv.cc
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


// System
#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>

// VW
#include "VW/iobasics.h"

// Local
#include "classifieruv.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false



using namespace std;
using namespace VW;

VW::ClassifierUV::ClassifierUV() :
  classifier_matrix(0),
  num_classes(0),
  rows(0),
  columns(0),
  my_bin_size(0),
  u_scale(1.0),
  v_scale(1.0)
{
  cerr << "New skin detector created" << endl;
}
VW::ClassifierUV::~ClassifierUV() {
  // Clear any existing array
  if (classifier_matrix!=0) {
    for (unsigned int i=0;i<rows;i++) {
      delete[] classifier_matrix[i];
    }
    delete[] classifier_matrix;
  }
  if (num_classes>0) {
    for (unsigned int i=0;i<num_classes;i++) {
      delete[] prob_matrix[i];
    }
  }
}

void VW::ClassifierUV::Reset(unsigned int new_bin_size,
			     unsigned int new_num_classes) 
{
  if (new_bin_size!=my_bin_size) {

    my_bin_size=new_bin_size;
    
    // Clear any existing array
    if (classifier_matrix!=0) {
      for (unsigned int i=0;i<rows;i++) {
	delete[] classifier_matrix[i];
      }
      delete[] classifier_matrix;
    }
    for (unsigned int i=0;i<prob_matrix.size();i++) {
      for (unsigned int j=0;j<rows;j++) {
	delete[] prob_matrix[i][j];
      }
      delete[] prob_matrix[i];
    }
    prob_matrix.resize(new_num_classes);
    num_classes = new_num_classes;
    // Set the U and V scale factors
    u_scale=1.0f/my_bin_size;
    v_scale=1.0f/my_bin_size;

    // Number of rows assuming u,v in {0,255}
    rows=columns=256/my_bin_size;

    // Allocating the Classification Matrix:
    classifier_matrix  = new unsigned int*[rows]; // Classification Matrix
    for (unsigned int i=0;i<rows;i++) {
      classifier_matrix[i] = new unsigned int[columns];
      for (unsigned int j=0;j<columns;j++) {
	classifier_matrix[i][j]=0;
      }
    }

    // And allocate the prob matrices
    cout << "Reserve space for "<<num_classes<<" prob matrices." << endl;
    for (unsigned int c=0;c<num_classes;c++) {
      prob_matrix[c] = new float*[rows];
      for (unsigned int i=0;i<rows;i++) {
	prob_matrix[c][i] = new float[columns];
	for (unsigned int j=0;j<columns;j++) {
	  prob_matrix[c][i][j]=0.0f;
	}
      }
    }
 
  }
}



void VW::ClassifierUV::Classify(const VW::ImageYUV<unsigned char>& input_image,
			    VW::ImageMono<unsigned char>& classification) const {
  if (classification.IsAlloced()) {
  if ((classification.GetWidth()!=input_image.GetWidth()) 
      || (classification.GetHeight()!=input_image.GetHeight())) {
    cerr << "VW::ClassifierUV::Classify(image): input and output image dimensions are different!" << endl;
    exit(0);
  }
  } else {
    classification.AllocImageData(input_image.GetWidth(), 
				  input_image.GetHeight());
  }

  const unsigned int num_pix = input_image.GetWidth()*input_image.GetHeight();
  unsigned char* input_ptr=static_cast<unsigned char*>(input_image.GetRawBuffer());
  unsigned char* output_ptr=static_cast<unsigned char*>(classification.GetRawBuffer());
  unsigned char y,u,v;
  for (unsigned int i=0;i<num_pix;i++) {
    y=*(input_ptr++);
    u=*(input_ptr++);
    v=*(input_ptr++);
    input_ptr++;
    

    *output_ptr=Classify(u,v);
    output_ptr++;

  }

}

void VW::ClassifierUV::GetProbImage(const VW::ImageYUV<unsigned char>& input_image,
				    unsigned int which_class,
				    VW::ImageMono<float>& prob) const
{
  if (prob.IsAlloced()) {
    if ((prob.GetWidth()!=input_image.GetWidth()) 
	|| (prob.GetHeight()!=input_image.GetHeight())) {
      cerr << "VW::ClassifierUV::GetProbImage(image): input and output image dimensions are different!" << endl;
      exit(0);
    }
  } else {
    prob.AllocImageData(input_image.GetWidth(), input_image.GetHeight());
  }
  const unsigned int num_pix = input_image.GetWidth()*input_image.GetHeight();
  unsigned char* input_ptr=static_cast<unsigned char*>(input_image.GetRawBuffer());
  float* output_ptr=static_cast<float*>(prob.GetRawBuffer());
  unsigned char y,u,v;
  for (unsigned int i=0;i<num_pix;i++) {
    y=*(input_ptr++);
    u=*(input_ptr++);
    v=*(input_ptr++);
    input_ptr++;
    

    *output_ptr=GetProb(u,v,which_class);
    output_ptr++;

  }
}

void VW::ClassifierUV::GetProbImage(const VW::ImageYUV<unsigned char>& input_image,
				    unsigned int which_class,
				    VW::ImageMono<unsigned char>& prob) const
{
  if (prob.IsAlloced()) {
    if ((prob.GetWidth()!=input_image.GetWidth()) 
	|| (prob.GetHeight()!=input_image.GetHeight())) {
      cerr << "VW::ClassifierUV::GetProbImage(image): input and output image dimensions are different!" << endl;
      exit(0);
    }
  } else {
    prob.AllocImageData(input_image.GetWidth(), input_image.GetHeight());
  }

  const unsigned int num_pix = input_image.GetWidth()*input_image.GetHeight();
  unsigned char* input_ptr=static_cast<unsigned char*>(input_image.GetRawBuffer());
  unsigned char* output_ptr=static_cast<unsigned char*>(prob.GetRawBuffer());
  unsigned char y,u,v;
  for (unsigned int i=0;i<num_pix;i++) {
    y=*(input_ptr++);
    u=*(input_ptr++);
    v=*(input_ptr++);
    input_ptr++;
    
    // Want full range to be 0<=p<256 so that each of the 256 values covers
    // an equal range, hence the 255.9!
    *output_ptr=static_cast<unsigned char>(255.9*GetProb(u,v,which_class));
    output_ptr++;

  }
}

void VW::ClassifierUV::
ClassifyAndColour(const VW::ImageYUV<unsigned char>& input_image,
		  VW::ImageRGB<unsigned char>& classification) const 
{
  if (classification.IsAlloced()) {
    if ((classification.GetWidth()!=input_image.GetWidth()) 
	|| (classification.GetHeight()!=input_image.GetHeight())) {
      cerr << "VW::ClassifierUV::ClassifyAndColour(image): input and output image dimensions are different!" << endl;
      exit(0);
    }
  } else {
    classification.AllocImageData(input_image.GetWidth(), input_image.GetHeight());
  }

  const unsigned int num_pix = input_image.GetWidth()*input_image.GetHeight();
  unsigned char* input_ptr=static_cast<unsigned char*>(input_image.GetRawBuffer());
  unsigned char* output_ptr=static_cast<unsigned char*>(classification.GetRawBuffer());
  unsigned char y,u,v,r,g,b;
  for (unsigned int i=0;i<num_pix;i++) {
    y=*(input_ptr++);
    u=*(input_ptr++);
    v=*(input_ptr++);
    input_ptr++;
    
    const unsigned int c=Classify(u,v);

    // Convert the classification number into an RGB colour
    Colourify(c,r,g,b);

    *output_ptr=b;output_ptr++;
    *output_ptr=g;output_ptr++;
    *output_ptr=r;output_ptr++;
    output_ptr++;

  }

  
}


void VW::ClassifierUV::
GetHistogramAsImage(VW::ImageRGB<unsigned char>& histogram) const
{
  if (!histogram.IsAlloced()) {
    histogram.AllocImageData(columns,rows);
  } else {
    if ((histogram.GetWidth()!=rows) 
	|| (histogram.GetHeight()!=columns)) {
      cerr << "VW::ClassifierUV::GetHistogramAsImage: image argument is wrong size ("<<histogram.GetWidth()<<"x"<<histogram.GetHeight()<<" should be "<<rows<<"x"<<columns<<")." << endl;
      exit(0);
    }
  }
  unsigned char r,g,b;
  for (unsigned int y=0;y<rows;y++) {
    for (unsigned int x=0;x<columns;x++) {
      unsigned int c=classifier_matrix[y][x];
      Colourify(c,r,g,b);
      histogram.SetPixel(x,y,VW::PixelRGB<unsigned char>(r,g,b,0));
    }
  }

}



void VW::PixelList2ClassifierUVMatrix(const std::string& pixellist_filename,
				      const std::string& matrix_filename,
				      unsigned int binsize)
{
   cerr << "VW::PixelList2ClassifierUVMatrix." << endl;
  string tagname;
  istringstream contents(" "),contents2(" ");


  // Open the input file
  ifstream inputstream(pixellist_filename.c_str());
  if (!inputstream) {
    cerr << "VW::PixelList2ClassifierUVMatrix - could not open input file \"" << pixellist_filename << "\"." << endl;
    exit(0);
  } else {
    // Find the relevant chunk of XML in this file
    if (!VW::FindXMLBlock("ClassificationData",inputstream,tagname)) {
      cerr << "VW::PixelList2ClassifierUVMatrix - could not find ClassificationData in \"" << pixellist_filename << "\"." << endl;
    exit(0);
    } else {
      cout << "   found it!" << endl;
      // Recover the number of classes
      unsigned int numclasses=0;
      unsigned int n=256/binsize;
      double otherprob=0;
      unsigned int otherid=0;
      if (VW::GetXMLBlock("NumberOfClasses",inputstream,contents,tagname)) {
	contents >> numclasses;
      } else {
	cout << "VW::PixelList2ClassifierUVMatrix - Failed to find <NumberOfClasses>\n";
	exit(0);
      }
      if (VERBOSE) cerr << "   Num classes="<<numclasses<<endl;

      if (VW::GetXMLBlock("OtherClassProbability",inputstream,contents,tagname)) {
	contents >> otherprob;
      } else {
	cout << "VW::PixelList2ClassifierUVMatrix - Failed to find <OtherClassProbability>\n";
	exit(0);
      }
      if (VERBOSE) cerr << "   Other class prob="<<otherprob<<endl;
  
      if (VW::GetXMLBlock("OtherClassification",inputstream,contents,tagname)) {
	contents >> otherid;
      } else {
	cout << "VW::PixelList2ClassifierUVMatrix - Failed to find <OtherClassification>\n";
	exit(0);
      }
      if (VERBOSE) cerr << "   Other class id="<<otherid<<endl;
 
      // Allocate the accumulation arrays (numclasses x n x n)
      if (VERBOSE) cerr << "   Create accumulation buffers..."<<endl;
      int*** accbuff = new int**[numclasses];
      for (unsigned int i=0;i<numclasses;i++) {
	accbuff[i] = new int*[n];
	for (unsigned int j=0;j<n;j++) {
	  accbuff[i][j] = new int[n];
	  for (unsigned int k=0;k<n;k++)
	    accbuff[i][j][k] = 0;
	}
      }
      if (VERBOSE) cerr << "                              ...done"<<endl;

      // Allocate the other data
      vector<string> classnames(numclasses);
      vector<unsigned int> classids(numclasses);
      vector<unsigned int> totalnumberofpixels(numclasses);

      // Now start to read the data
      for (unsigned int c=0;c<numclasses;c++) {
	totalnumberofpixels[c] = 0;
	// Skip to next class definition
	if (!VW::FindXMLBlock("Class",inputstream,tagname)) {
	  cout << "VW::PixelList2ClassifierUVMatrix - Failed to find class number "<<c+1<<"\n";
	  exit(0);
	}
	
	if (VW::GetXMLBlock("Name",inputstream,contents,tagname)) {
	  contents >> classnames[c];
	} else {
	  cout << "VW::PixelList2ClassifierUVMatrix - Failed to find <Name> for class "<<c+1<<".\n";
	  exit(0);
	}
	if (VERBOSE) cerr << "   Class "<<c<<" name="<<classnames[c]<<endl;

	if (VW::GetXMLBlock("Classification",inputstream,contents,tagname)) {
	  contents >> classids[c];
	} else {
	  cout << "VW::PixelList2ClassifierUVMatrix - Failed to find <Classification> for class "<<c+1<<".\n";
	  exit(0);
	}
  	
	unsigned int numpix=0;
	if (VW::GetXMLBlock("NumberOfPixels",inputstream,contents,tagname)) {
	  contents >> numpix;
	} else {
	  cout << "VW::PixelList2ClassifierUVMatrix - Failed to find <NumberOfPixels> for class "<<c+1<<".\n";
	  exit(0);
	}
  	
	// Now read all pixel data
	if (VW::FindXMLBlock("Data",inputstream,tagname)) {
	  for (unsigned int p=0;p<numpix;p++) {
	    if (VW::GetXMLBlock("Pixel",inputstream,contents,tagname)) {
	      unsigned char y,u,v;
	      unsigned int t,m;
	      VW::GetXMLBlock("Y",contents,contents2,tagname);
	      contents2 >> t;y=static_cast<unsigned char>(t);
	      VW::GetXMLBlock("U",contents,contents2,tagname);
	      contents2 >> t;u=static_cast<unsigned char>(t);
	      VW::GetXMLBlock("V",contents,contents2,tagname);
	      contents2 >> t;v=static_cast<unsigned char>(t);
	      VW::GetXMLBlock("Multiplicity",contents,contents2,tagname);
	      contents2 >> m;

	      // Accumulate
	      if (VERBOSE) cerr << "Add "<<m<<" to bin ("<<u/binsize<<","<<v/binsize<<")"<<endl;
	      accbuff[c][u/binsize][v/binsize] += m;
	      totalnumberofpixels[c] += m;

	    } else {
	      cout << "VW::PixelList2ClassifierUVMatrix - Failed to find <Pixel> number "<<p<<" for class "<<c+1<<".\n";
	      exit(0);
	    }
	  }
	} else {
	  cout << "VW::PixelList2ClassifierUVMatrix - Failed to find <Data> for class "<<c+1<<".\n";
	  exit(0);
	}


      } // End for (unsigned int c=0;c<numclasses;c++)


      // Now we have all the data, so close the input file
      inputstream.close();

      // Open the output stream
      ofstream outputstream(matrix_filename.c_str());

      if (!outputstream) {
	cerr << "VW::PixelList2ClassifierUVMatrix - could not open output file \"" << matrix_filename << "\"." << endl;
	exit(0);
      }
	cerr << "VW::PixelList2ClassifierUVMatrix - writing to output file \"" << matrix_filename << "\"." << endl;

      // Write some header stuff
      outputstream << "<ClassifierUVData>" << endl;
      outputstream << "  <NumClasses>" << numclasses << "</NumClasses>"<<endl;
      outputstream << "  <BinSize>" << binsize << "</BinSize>"<<endl;
      outputstream << "  <ClassifierMatrix>";

      // For each pixel, work out the probabilities and stick in the best
      for (unsigned int v=0;v<n;v++) {
	for (unsigned int u=0;u<n;u++) {
	  unsigned int best_id = otherid;
	  double best_prob = otherprob;
	  for (unsigned int c=0;c<numclasses;c++) {
	    const double prob = double(accbuff[c][u][v])/double(totalnumberofpixels[c]);
	    if (prob>best_prob) {
	      best_prob = prob;
	      best_id = classids[c];
	    }
	  }
	  // Write the id
	  outputstream << best_id << " ";

	}
	  outputstream << endl;
      }
      outputstream << "</ClassifierMatrix>" << endl;

      // Now write each class's probability matrix
      for (unsigned int c=0;c<numclasses;c++) {
	outputstream << "  <ProbMatrix Class="<<c<<">";
	for (unsigned int v=0;v<n;v++) {
	  for (unsigned int u=0;u<n;u++) {
	  double total_prob = otherprob;
	  for (unsigned int c2=0;c2<numclasses;c2++) {
	    total_prob += float(accbuff[c2][u][v])/float(totalnumberofpixels[c2]);
	  }
	  const float prob = float(accbuff[c][u][v])/float(totalnumberofpixels[c]);
	    outputstream << prob/total_prob << " ";
	  }
	  outputstream << endl;
	}
	outputstream << "  </ProbMatrix>\n";
      }
      outputstream << "</ClassifierUVData>" << endl;
      outputstream.close();

    }
  }
}
inline 
bool VW::ClassifierUV::WriteASCII(std::ostream& stream) const
{
  std::cerr << "VW::ClassifierUV::WriteASCII - not implemented yet.\n";
  return(false);
}

inline 
bool VW::ClassifierUV::WriteXML(std::ostream& stream, 
				 const std::string& name) const
{  
  std::cerr << "VW::ClassifierUV::WriteXML - not implemented yet.\n";
  return(false);
}

inline 
bool VW::ClassifierUV::WriteBinary(std::ostream& stream) const
{
  std::cerr << "VW::ClassifierUV::WriteBinary - not implemented yet.\n";
  return(false);
}

inline 
bool VW::ClassifierUV::ReadASCII(std::istream& stream)
{
  std::cerr << "VW::ClassifierUV::ReadASCII - not implemented yet.\n";
  return(false);
}

inline 
bool VW::ClassifierUV::ReadXML(std::istream& stream, 
					 const std::string& name)
{
  // Loads the classification file into the classif matrix
  // Should return false if loading fails, but doesn't yet

  unsigned int bin_size=1, nclasses=1;
  std::istringstream contents(" "); // NB: GCC<3 can't cope with uninited sstreams
   std::string tagname;

  if (!VW::FindXMLBlock("ClassifierUVData",stream,tagname)) {
    std::cerr << "VW::ClassifierUV::ReadXML - could not find <ClassifierUVData> tag." << std::endl;
    return(false);
  }
  if (!VW::GetXMLBlockAsNumber("NumClasses",stream,nclasses,tagname)) {
    std::cerr << "VW::ClassifierUV::ReadXML - "
	      << "could not find <NumClasses> tag." << std::endl;
    return(false);
  }
  if (!VW::GetXMLBlockAsNumber("BinSize",stream,bin_size,tagname)) {
    std::cerr << "VW::ClassifierUV::ReadXML - "
	      << "could not find <BinSize> tag." << std::endl;
    return(false);
  }
  
  cout << "ClassifierUV::ReadXML: Going to load " << nclasses << " classes with matrix size " << 256/bin_size << "x" << 256/bin_size << endl;

  // Resize the data array
  Reset(bin_size, nclasses);

  // Now find the data
  if (!VW::FindXMLBlock("ClassifierMatrix",stream,tagname)) {
    std::cerr << "VW::ClassifierUV::ReadXML - "
	      << "could not find <ClassifierMatrix> tag." << std::endl;
    return(false);
  }


  for(unsigned int i=0;i<rows;i++)
    for(unsigned int j=0;j<columns;j++)
      stream >> classifier_matrix[i][j];

  // Now read each prob matrix
  for(unsigned int c=0;c<nclasses;c++) {
    cout << "   Load prob matrix "<<c<<endl;
    if (!VW::FindXMLBlock("ProbMatrix",stream,tagname)) {
      std::cerr << "VW::ClassifierUV::ReadXML - "
		<< "could not find <ProbMatrix> tag for class "<<c
		<<"." << std::endl;
      return(false);
    }
    for(unsigned int i=0;i<rows;i++)
      for(unsigned int j=0;j<columns;j++)
	stream >> prob_matrix[c][i][j];
  }



  return(true);
}

inline 
bool VW::ClassifierUV::ReadBinary(std::istream& stream)
{
  std::cerr << "VW::ClassifierUV::ReadBinary - not implemented yet.\n";
  exit(0);
  return(true);
}
void VW::ClassifierUV::LoadParameterFile(string filename)
{
  ReadXMLFile(filename);
}
void VW::ClassifierUV::SaveParameterFile(string filename)
{
  WriteXMLFile(filename);
}
