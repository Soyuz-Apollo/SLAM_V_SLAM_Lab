/** 
 * @file   line2d.h
 * @author ID Reid
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
 * @class   VW::Line2D
 * @ingroup GeometricObjects
 * @author  ID Reid
 * @date    02/08/02
 * @brief   Representation of a 2D (infinite) line.
 *
 * Note that this definition of a line is infinite - that is, no end-points
 * are stored. For a finite line, see VW::LineSeg2D. The data is stored as
 * a 3D vector [a b c] defining the line ax + by + c = 0.
 *
 * Computation routines are conatined in VW::Line2DCompute and some other 
 * operations in operations.h
 */


#ifndef _line2d_h_
#define _line2d_h_

// System
#include <cassert>
#include <VNL/vectorfixed.h>

// Local
#include "VW/iobasics.h"

namespace VW
{

  // Forward declare some classes
  class LineSeg2D;
  class Point2D;
  typedef Point2D Vector2D;


class Line2D : public VNL::VectorFixed<3,double>
{
 public:
  Line2D () {};
  Line2D (double a, double b, double c);
  Line2D (const VNL::VectorFixed<3,double>& line);
  Line2D (const VNL::VectorFixed<3,float>& line);
  Line2D (const VNL::Vector<double>& line);
  Line2D (const VNL::Vector<float>& line);

  virtual ~Line2D() {}

  Line2D &operator=(const Line2D &p);

  void Set(double a, double b, double c);
  void Set(const VNL::Vector<double>& line);


  double GetA() const;
  double GetB() const;
  double GetC() const;

  /** Find the Y value corresponding to the given X value */
  double LookupY(const double x) const;

  /** Find the X value corresponding to the given Y value */
  double LookupX(const double y) const;

  /** The direction of the line (unit vector) */
  VW::Vector2D GetDirection() const;

  /** The direction perpendicular to the line (unit vector) */
  VW::Vector2D GetPerpendicular() const;

  /** Get the distance of a point from this line. */
  double GetDistance(const double x,
		     const double y) const;

  /** Get the distance of a point from this line. */
  double GetDistance(const Point2D& p) const;

  /** Get the squared distance of a point from this line. */
  double GetDistance2(const double x,
		     const double y) const;

  /** Get the squared distance of a point from this line. */
  double GetDistance2(const Point2D& p) const;

  /** Truncate using bounds on X (result is a line-segment) */
  LineSeg2D TruncateX(const double x0,
		     const double x1) const;

  /** Truncate using bounds on Y (result is a line-segment) */
  LineSeg2D TruncateY(const double y0,
		     const double y1) const;

  /** Truncate to lie within a rectangle (x0,y0)->(x1,y1) 
   * (eg for displaying) 
   */
  LineSeg2D Truncate(const double x0, const double y0,
		     const double x1, const double y1) const;

  // Streaming methods

  virtual 
  bool WriteASCII(std::ostream& stream) const;
  virtual 
  bool WriteXML(std::ostream& stream,
		const std::string& name="") const;
  virtual 
  bool WriteBinary(std::ostream& stream) const;


  virtual 
  bool ReadASCII(std::istream& stream);
  virtual 
  bool ReadXML(std::istream& stream, 
	       const std::string& name="");
  virtual 
  bool ReadBinary(std::istream& stream);

};

  class Covariance3D;

  /** Normalise the line and its covariance so that distances can be measured
   * using the dot product.
   */
  void NormaliseLine(VW::Line2D& line,
		     VW::Covariance3D& line_covariance);


};  // End of namespace VW







// Inlines

inline VW::Line2D::Line2D (double a, double b, double c) :
  VNL::VectorFixed<3,double> (a, b, c)
{}

inline VW::Line2D::Line2D (const VNL::VectorFixed<3,double>& line) :
  VNL::VectorFixed<3,double> (line)
{}

inline VW::Line2D::Line2D (const VNL::VectorFixed<3,float>& line)
{
  data[0] = line[0];
  data[1] = line[1];
  data[2] = line[2];
}

inline VW::Line2D::Line2D (const VNL::Vector<double>& line) :
  VNL::VectorFixed<3,double> (line)
{}

inline VW::Line2D::Line2D (const VNL::Vector<float>& line)
{
  assert (line.size () == 3);
  data[0] = line[0];
  data[1] = line[1];
  data[2] = line[2];
}

inline void VW::Line2D::Set (double a, double b, double c)
{
  data[0] = a;
  data[1] = b;
  data[2] = c;
}

inline void VW::Line2D::Set(const VNL::Vector<double>& line)
{
  data[0] = line[0];
  data[1] = line[1];
  data[2] = line[2];
}

inline double 
VW::Line2D::GetA() const
{
  return data[0];
}

inline double 
VW::Line2D::GetB() const
{
  return data[1];
}

inline double 
VW::Line2D::GetC() const
{
  return data[2];
}

inline VW::Line2D& VW::Line2D::operator= (const VW::Line2D& line)
{
  data[0] = line[0];
  data[1] = line[1];
  data[2] = line[2];

  return *this;
}

inline double 
VW::Line2D::LookupY(const double x) const
{
  // y = -(ax+c)/b

  return -( data[2] + data[0]*x) / data[1];
}

inline double 
VW::Line2D::LookupX(const double y) const
{
  // x = -(by+c)/a

  return -( data[2] + data[1]*y) / data[0];
  
}

inline
bool  VW::Line2D::WriteASCII(std::ostream& stream) const
{
  stream << data[0] << "\t" << data[1] << "\t" << data[2] << "\t";
  return(true);
}

inline
bool  VW::Line2D::WriteXML(std::ostream& stream, 
				 const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<Line2D NAME=\"" << name << "\"> ";
  else
    stream << "<Line2D> ";
  
  // Contents
  stream << "<A>" << data[0] << "</A> ";
  stream << "<B>" << data[1] << "</B> ";
  stream << "<C>" << data[2] << "</C> ";
  
    // End tag
  stream << "</Line2D>";
  
  return(true);
}

inline
bool  VW::Line2D::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,data[0]);
  VW::WriteBinary(stream,data[1]);
  VW::WriteBinary(stream,data[2]);
  return(true);
}

inline
bool  VW::Line2D::ReadASCII(std::istream& stream)
{
  stream >> data[0] >> data[1] >> data[2];
  return(true);
}

inline
bool VW::Line2D::ReadXML(std::istream& stream, 
			  const std::string&)
{
  std::string tagname;
  std::istringstream contents(" ");
  if (VW::FindXMLBlock("Line2D",stream,tagname)) {
    if (VW::GetXMLBlock("A",stream,contents,tagname))
      contents >> data[0];
    else
      std::cerr << "VW::Line2D::ReadXML - error reading A-component.\n";
    if (VW::GetXMLBlock("B",stream,contents,tagname))
      contents >> data[1];
    else
      std::cerr << "VW::Line2D::ReadXML - error reading B-component.\n";
    if (VW::GetXMLBlock("C",stream,contents,tagname))
      contents >> data[2];
    else
      std::cerr << "VW::Line2D::ReadXML - error reading C-component.\n";
  } else {
    return(false);
  }
  return(true);
}

inline
bool VW::Line2D::ReadBinary(std::istream& stream)
{
  VW::ReadBinary(stream,data[0]);
  VW::ReadBinary(stream,data[1]);
  VW::ReadBinary(stream,data[2]);
  return(true);
}


#endif // #ifndef _line2d_h_
