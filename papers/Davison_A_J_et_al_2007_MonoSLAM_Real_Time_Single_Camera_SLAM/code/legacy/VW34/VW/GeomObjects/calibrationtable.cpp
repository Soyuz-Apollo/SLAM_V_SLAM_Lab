/** 
 * @file   calibrationtable.cpp 
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
#include <sstream>

#include "calibrationtable.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false


using namespace std;


VW::CalibrationTable::CalibrationTable() :
  is_fixed_aspect(false),
  is_fixed_skew(false),
  is_fixed_pp(false),
  is_known_aspect(false),
  is_known_skew(false),
  is_known_pp(false),
  known_aspect(1.0), 
  known_skew(0.0), 
  known_px(0.0), 
  known_py(0.0),
  f_poly(K_POLYFITORDER),
  f_reverse_poly(K_POLYFITORDER),
  aspect_poly(K_POLYFITORDER),
  skew_poly(K_POLYFITORDER),
  px_poly(K_POLYFITORDER),
  py_poly(K_POLYFITORDER),
  kappa_poly(K_POLYFITORDER)
{
  
}


VW::CalibrationTable::~CalibrationTable()
{
  if(VERBOSE) cerr << "VW::CalibrationTable: destructor called!" << endl;
}

bool  VW::CalibrationTable::WriteASCII(std::ostream& stream) const
{
  const unsigned int numcal = _GetNumberOfCalibrationData();
  const unsigned int numdis = _GetNumberOfDistortionData();



  if(VERBOSE) cerr << "VW::CalibrationTable::WriteASCII - Not yet" << endl;

  stream << "% motor_position | focal_length | aspect_ratio | skew | PPX | PPY \n";

  for (unsigned int j=0;j<numcal;j++) {
    double m=0,f=0,a=0,s=0,px=0,py=0;
    _GetRawCalibrationData(j,m,f,a,s,px,py);
    stream << m << "\t" << f << "\t" << a << "\t" << s << "\t" << px << "\t" << py << "\n";
  }

  stream << "\n";

  stream << "% motor_position | kappa\n";
  for (unsigned int j=0;j<numdis;j++) {
    double m=0,kappa=0;
    this->_GetRawDistortionData(j,m,kappa);
    stream << m << "\t" << kappa << "\n";
  }

  return(true);
}

bool  VW::CalibrationTable::WriteXML(std::ostream& stream, 
				 const std::string& name) const
{
  const unsigned int numcal = _GetNumberOfCalibrationData();
  const unsigned int numdis = _GetNumberOfDistortionData();

  
  // Contents

    // Start tag
    if (name.size()>0)
      stream << "<CalibrationTable NAME=\"" << name << "\"> \n";
    else
      stream << "<CalibrationTable> \n";
    
    stream << "  <NumberOfCalibrationData> "<< numcal
	   << " </NumberOfCalibrationData>\n";
    stream << "  <NumberOfDistortionData> "<< numdis
	   << " </NumberOfDistortionData>\n";
    
    // Contents
    if (numcal>0) {

    for (unsigned int j=0;j<numcal;j++) {
      double m=0,f=0,a=0,s=0,px=0,py=0;
      _GetRawCalibrationData(j,m,f,a,s,px,py);
      stream << "  <CalibrationDatum> <Motor>" << m
	     << "</Motor> <Zoom>" << f
	     << "</Zoom> <Aspect>" << a
	     << "</Aspect> <Skew>" << s
	     << "</Skew> <PPx>" << px
	     << "</PPx> <PPy>" << py
	     << "</PPy> </CalibrationDatum>\n";
    }
  }
  if (numdis>0) {
    for (unsigned int j=0;j<numdis;j++) {
      double m=0,kappa=0;
      this->_GetRawDistortionData(j,m,kappa);
      stream << "  <DistortionDatum> <Motor>" << m
	     << "</Motor> <Kappa>" << kappa
	     << "</Kappa> </DistortionDatum>\n";
    }
  }

  // End tag
  stream << "</CalibrationTable>";
  
  return(true);
}

bool  VW::CalibrationTable::WriteBinary(std::ostream& stream) const
{
  const unsigned int numcal = _GetNumberOfCalibrationData();
  const unsigned int numdis = _GetNumberOfDistortionData();
  
  // Write sizes
  VW::WriteBinary(stream, numcal);
  VW::WriteBinary(stream, numdis);
  // Data points
  if (numcal>0) {
    for (unsigned int j=0;j<numcal;j++) {
      double m=0,f=0,a=0,s=0,px=0,py=0;
      this->_GetRawCalibrationData(j,m,f,a,s,px,py);
      VW::WriteBinary(stream, m);
      VW::WriteBinary(stream, f);
      VW::WriteBinary(stream, a);
      VW::WriteBinary(stream, s);
      VW::WriteBinary(stream, px);
      VW::WriteBinary(stream, py);
    }
  }
  if (numdis>0) {
    for (unsigned int j=0;j<numdis;j++) {
      double m=0,kappa=0;
      this->_GetRawDistortionData(j,m,kappa);
      VW::WriteBinary(stream, m);
      VW::WriteBinary(stream, kappa);
    }
  }
  return(true);
}

bool  VW::CalibrationTable::ReadASCII(std::istream& stream)
{
  if(VERBOSE) cerr << "VW::CalibrationTable::ReadASCII - Not yet" << endl;
  exit(0);
  return(true);
}

bool VW::CalibrationTable::ReadXML(std::istream& stream, 
			  const std::string& name)
{
  if(VERBOSE) std::cerr << "VW::CalibrationTable::ReadXML\n";
  std::string tagname;
  std::istringstream contents(" ");
  unsigned int num_cal = 0;
  unsigned int num_dis = 0;

  // Empty any existing data
  Clear();

  // Empty the storage
  if (VERBOSE) std::cout << "VW::CalibrationTable::ReadXML - clear existing data.\n";
  Clear();


  if (VW::FindXMLBlock("CalibrationTable",stream,tagname)) {


    if (VW::GetXMLBlock("NumberOfCalibrationData",stream,contents,tagname))
      contents >> num_cal;
    else {
      std::cerr << "VW::CalibrationTable::ReadXML - error reading NumberOfCalibrationData.\n";
      return(false);
    }
    if (VW::GetXMLBlock("NumberOfDistortionData",stream,contents,tagname))
      contents >> num_dis;
    else {
      std::cerr << "VW::CalibrationTable::ReadXML - error reading NumberOfDistortionData.\n";
      return(false);
    }
    
    if (VERBOSE) std::cout << "VW::CalibrationTable::ReadXML - going to read "
			   << num_cal << " sets of calibration data.\n";
     
   // Read each claibration datum
    for (unsigned int i=0;i<num_cal;i++) {
      if (VW::FindXMLBlock("CalibrationDatum",stream,tagname)) {
	double m,z,a,s,ppx,ppy;
	GetXMLBlockAsNumber("Motor",stream,m,tagname); 
	GetXMLBlockAsNumber("Zoom",stream,z,tagname); 
	GetXMLBlockAsNumber("Aspect",stream,a,tagname);
	GetXMLBlockAsNumber("Skew",stream,s,tagname);
	GetXMLBlockAsNumber("PPx",stream,ppx,tagname);
	GetXMLBlockAsNumber("PPy",stream,ppy,tagname);
	if (VERBOSE) 
	  std::cout << "Read ("<<m<<", "<<z<<", "<<a<<", "<<s<<", "<<ppx<<", "<<ppy<<")\n";
	_AddRawCalibrationData(m,z,a,s,ppx,ppy);
      } else {
	std::cerr << "VW::CalibrationTable::ReadXML - could not find calibration datum "<<i<<".\n";
	return(false);
      }
    }
 
    if (VERBOSE) std::cout << "VW::CalibrationTable::ReadXML - going to read "
			   << num_dis << " sets of distortion data.\n";
   // Read each distortion datum
    for (unsigned int i=0;i<num_dis;i++) {
      if (VW::FindXMLBlock("DistortionDatum",stream,tagname)) {
	double m,k;
	GetXMLBlock("Motor",stream,contents,tagname); 
	contents >> m;
	GetXMLBlock("Kappa",stream,contents,tagname); 
	contents >> k;
	_AddRawDistortionData(m,k);
      } else {
	std::cerr << "VW::CalibrationTable::ReadXML - could not find distortion datum "<<i<<".\n";
	return(false);
      }
    }
    if (VERBOSE) std::cerr << "VW::CalibrationTable::ReadXML: Read "
			   <<_GetNumberOfCalibrationData()
			   <<" calibration data points and "
			   <<_GetNumberOfDistortionData()
			   <<" distortion data points.\n";

  } else {
    std::cerr<<"VW::CalibrationTable::ReadXML - could not find <CalibrationTable> tag.\n";
   return(false);
  }
  return(true);
}

bool VW::CalibrationTable::ReadBinary(std::istream& stream)
{
  // Empty the storage
  Clear();

  // Read sizes
  unsigned int nc=0,nd=0;
  VW::ReadBinary(stream, nc);
  VW::ReadBinary(stream, nd);
  // Data points (calibration)
  if (nc>0) {
    for (unsigned int j=0;j<nc;j++) {
      double m=0,f=0,a=0,s=0,px=0,py=0;
      VW::ReadBinary(stream, m);
      VW::ReadBinary(stream, f);
      VW::ReadBinary(stream, a);
      VW::ReadBinary(stream, s);
      VW::ReadBinary(stream, px);
      VW::ReadBinary(stream, py);
      _AddRawCalibrationData(m,f,a,s,px,py);
    }
  }
  // Data points (distortion)
  if (nd>0) {
    for (unsigned int j=0;j<nd;j++) {
      double m=0,kappa=0;
      VW::ReadBinary(stream, m);
      VW::ReadBinary(stream, kappa);
      _AddRawDistortionData(m,kappa);
    }
  }
  return(true);
}



