/** 
 * @file     calibrationmatrix.h
 * @author   BJ Tordoff
 * @date     21/08/2003
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
 * @class   VW::CalibrationMatrix
 * @ingroup GeometricObjects
 * @date    21/08/2003
 * @author  BJ Tordoff
 * @brief A class to store the camera internal calibration.
 *
 * Whilst this is mostly just and ordinary VW::MatrixFixed, it has some added functions
 * for faster computation taking advantage of the known zeros.
 */

#ifndef _calibrationmatrix_h_
#define _calibrationmatrix_h_

// VW
#include <VNL/matrixfixed.h>

// Local
#include "point3d.h"
#include "point2d.h"

namespace VW {




  class CalibrationMatrix : public VW::IOObject {

  public:
    
    /** Construct and set to identity. */
    CalibrationMatrix();

    /** Construct from another rotation matrix. */
    CalibrationMatrix(const CalibrationMatrix& C);

    /** Set from list of params. */
    CalibrationMatrix(const double focal_length,
		      const double aspect_ratio,
		      const double skew,
		      const double principle_point_x,
		      const double principle_point_y);

    virtual ~CalibrationMatrix() {}

    /** Set from list of params. */
    void Set(const double focal_length,
	     const double aspect_ratio,
	     const double skew,
	     const double principle_point_x,
	     const double principle_point_y);

    /** Set from 3x3 matrix (must be upper triangular). */
    void Set(const VNL::MatrixFixed<3,3,double>& M);

    /** Set to the identity matrix. */
    void SetIdentity();

    double      GetFocalLength() const;
    /** Aspect ratio is f_y/f_x. */
    double      GetAspectRatio() const;
    double      GetSkew() const;
    double      GetPrincipalPointX() const;
    double      GetPrincipalPointY() const;
    VW::Point2D GetPrincipalPoint() const;

    void SetFocalLength(const double f);
    /** Aspect ratio is f_y/f_x and must be positive. */
    void SetAspectRatio(const double a);
    void SetSkew(const double s);
    void SetPrincipalPointX(const double px);
    void SetPrincipalPointY(const double py);
    void SetPrincipalPoint(const VW::Point2D& p);

    /** Return the inverse of the calibration (which is also a 
     *valid calibration!). 
     */
    VW::CalibrationMatrix Inverse() const;

    /** Return as a 3x3 (upper triangular) matrix. */
    VNL::MatrixFixed<3,3,double> AsMatrix() const;

    /** Return the inverse as a 3x3 (upper triangular) matrix. 
     * Note that this is much faster than getting the matrix then inverting it.
     */
    VNL::MatrixFixed<3,3,double> InverseMatrix() const;

    /** Return the transpose as a 3x3 matrix. */
    VNL::MatrixFixed<3,3,double> TransposeMatrix() const;


    VNL::MatrixFixed<3,3,double> operator*(const VNL::MatrixFixed<3,3,double>& M) const;
    /** Multiply as a non-homogeneous 3-vector. */
    VW::Vector3D operator*(const VW::Vector3D& v) const;
    /** Multiply a 2-vector homogeneously. */
    VW::Vector2D operator*(const VW::Vector2D& v) const;

    /** Map a 2x2 matrix homogeneously into pixel coords as K*M*K' (2x2 part only). */
    VNL::MatrixFixed<2,2,double> Map(const VNL::MatrixFixed<2,2,double>& M) const;


    // Streaming methods
    
    bool WriteASCII(std::ostream& stream) const;
    bool WriteXML(std::ostream& stream,
		  const std::string& name="") const;
    bool WriteBinary(std::ostream& stream) const;
    

    bool ReadASCII(std::istream& stream);
    bool ReadXML(std::istream& stream, 
		 const std::string& name="");
    bool ReadBinary(std::istream& stream);

protected:
  double _f, _a, _s, _px, _py;

  };


}; // End namespace VW





















// Some inlines
inline
VW::CalibrationMatrix::CalibrationMatrix()
  : _f(1), _a(1), _s(0), _px(0), _py(0)
{
}

inline
VW::CalibrationMatrix::CalibrationMatrix(const VW::CalibrationMatrix& C)
  : VW::IOObject(), _f(C._f), _a(C._a), _s(C._s), _px(C._px), _py(C._py)
{
}

inline
VW::CalibrationMatrix::CalibrationMatrix(const double focal_length,
					 const double aspect_ratio,
					 const double skew,
					 const double principle_point_x,
					 const double principle_point_y)
  : _f(focal_length), 
    _a(aspect_ratio),
    _s(skew),
    _px(principle_point_x),
    _py(principle_point_y)    
{
}



inline void 
VW::CalibrationMatrix::Set(const double focal_length,
			   const double aspect_ratio,
			   const double skew,
			   const double principle_point_x,
			   const double principle_point_y)
{
  _f = focal_length; 
  _a = aspect_ratio;
  _s = skew;
  _px = principle_point_x;
  _py = principle_point_y;
}

inline void 
VW::CalibrationMatrix::Set(const VNL::MatrixFixed<3,3,double>& M)
{
  assert((M[1][0]==0) && (M[2][0]==0) && (M[2][1]==0));

  const double scale = 1.0/M[2][2];
  _f  = scale * M[0][0];
  _a  = scale * M[1][1] / _f;
  _s  = scale * M[0][1] / _f;
  _px = scale * M[0][2];
  _py = scale * M[1][2];
}

inline void 
VW::CalibrationMatrix::SetIdentity()
{
  _f  = 1.0;
  _a  = 1.0;
  _s  = 0.0;
  _px = 0.0;
  _py = 0.0;
}

inline double 
VW::CalibrationMatrix::GetFocalLength() const
{
  return _f;
}

inline double 
VW::CalibrationMatrix::GetAspectRatio() const
{
  return _a;
}

inline double 
VW::CalibrationMatrix::GetSkew() const
{
  return _s;
}

inline double 
VW::CalibrationMatrix::GetPrincipalPointX() const
{
  return _px;
}

inline double 
VW::CalibrationMatrix::GetPrincipalPointY() const
{
  return _py;
}

inline VW::Point2D 
VW::CalibrationMatrix::GetPrincipalPoint() const
{
  return VW::Point2D(_px,_py);
}

inline void 
VW::CalibrationMatrix::SetFocalLength(const double f)
{
  _f = f;
}

inline void 
VW::CalibrationMatrix::SetAspectRatio(const double a)
{
  _a = a;
}

inline void 
VW::CalibrationMatrix::SetSkew(const double s)
{
  _s = s;
}

inline void 
VW::CalibrationMatrix::SetPrincipalPointX(const double px)
{
  _px = px;
}

inline void 
VW::CalibrationMatrix::SetPrincipalPointY(const double py)
{
  _py = py;
}

inline void 
VW::CalibrationMatrix::SetPrincipalPoint(const VW::Point2D& p)
{
  _px = p.GetX();
  _py = p.GetY();
}


inline VW::CalibrationMatrix 
VW::CalibrationMatrix::Inverse() const
{
  const double f2 = 1.0 / _f;
  const double a2 = 1.0 / _a;
  return CalibrationMatrix(f2, 
			   a2, 
			   -_s*a2, 
			   f2* (_s*_py*a2 - _px), 
			   -f2*a2*_py);
}

inline VNL::MatrixFixed<3,3,double> 
VW::CalibrationMatrix::AsMatrix() const
{
  VNL::MatrixFixed<3,3,double> M(0.0);
  M(0,0) = _f;
  M(1,1) = _f*_a;
  M(0,1) = _f*_s;
  M(0,2) = _px;
  M(1,2) = _py;
  M(2,2) = 1.0;
  return M;
}

inline VNL::MatrixFixed<3,3,double> 
VW::CalibrationMatrix::InverseMatrix() const
{
  const double f2 = 1.0 / _f;
  const double a2 = 1.0 / _a;
  const double s2 = -_s * a2;
  const double px2 = f2* (_s*_py*a2 - _px);
  const double py2 = -f2 * a2 * _py;
  VNL::MatrixFixed<3,3,double> M(0.0);
  M(0,0) = f2;
  M(1,1) = f2 * a2;
  M(0,1) = f2 * s2;
  M(0,2) = px2;
  M(1,2) = py2;
  M(2,2) = 1.0;
  return M;
}

inline VNL::MatrixFixed<3,3,double> 
VW::CalibrationMatrix::TransposeMatrix() const
{
  VNL::MatrixFixed<3,3,double> M(0.0);
  M(0,0) = _f;
  M(1,1) = _f*_a;
  M(1,0) = _f*_s;
  M(2,0) = _px;
  M(2,1) = _py;
  M(2,2) = 1.0;
  return M;
}

inline VNL::MatrixFixed<3,3,double> 
VW::CalibrationMatrix::operator*(const VNL::MatrixFixed<3,3,double>& M) const
{
  VNL::MatrixFixed<3,3,double> result(M);
  const double fy = _f*_a;
  const double s = _f*_s;

  result[0][0] = _f*result[0][0] + s*result[1][0] + _px*result[2][0];
  result[0][1] = _f*result[0][1] + s*result[1][1] + _px*result[2][1];
  result[0][2] = _f*result[0][2] + s*result[1][2] + _px*result[2][2];

  result[1][0] =                  fy*result[1][0] + _py*result[2][0];
  result[1][1] =                  fy*result[1][1] + _py*result[2][1];
  result[1][2] =                  fy*result[1][2] + _py*result[2][2];

  return result;
}

inline VW::Vector3D 
VW::CalibrationMatrix::operator*(const VW::Vector3D& v) const
{
  VW::Vector3D result;

  result.SetX(_f*v.GetX() + _f*_s*v.GetY() + _px*v.GetZ());
  result.SetY(              _f*_a*v.GetY() + _py*v.GetZ());
  result.SetZ(                                   v.GetZ());

  return result;
}

inline VW::Vector2D 
VW::CalibrationMatrix::operator*(const VW::Vector2D& v) const
{
  VW::Vector2D result(0,0);

  result.SetX(_f*v.GetX() + _f*_s*v.GetY() + _px);
  result.SetY(              _f*_a*v.GetY() + _py);

  return result;
}
inline VNL::MatrixFixed<2,2,double> 
VW::CalibrationMatrix::Map(const VNL::MatrixFixed<2,2,double>& M) const
{
  VNL::MatrixFixed<2,2,double> result;
  const double f2=_f*_f;
  result[0][0] = f2*( M[0][0] + _s*(M[0][1] + M[1][0] + _s*M[1][1]) );
  result[0][1] = f2*_a*( M[0][1] + _s*M[1][1] );
  result[1][0] = f2*_a*( M[1][0] + _s*M[1][1] );
  result[1][1] = f2*_a*_a*M[1][1];
  return result;
}

inline
bool  VW::CalibrationMatrix::WriteASCII(std::ostream& stream) const
{
  stream << _f << "\t" << _a << "\t" << _s << "\t" << _px << "\t" << _py << "\t";
  return(true);
}

inline
bool  VW::CalibrationMatrix::WriteXML(std::ostream& stream, 
				 const std::string& name) const
{
  
  // Start tag
  if (name.size()>0)
    stream << "<CalibrationMatrix NAME=\"" << name << "\"> ";
  else
    stream << "<CalibrationMatrix> ";
  
  // Contents
  stream << "<FocalLength>" << _f << "</FocalLength> ";
  stream << "<AspectRatio>" << _a << "</AspectRatio> ";
  stream << "<Skew>" << _s << "</Skew> ";
  stream << "<PrincipalPointX>" << _px << "</PrincipalPointX> ";
  stream << "<PrincipalPointY>" << _py << "</PrincipalPointY> ";
  
    // End tag
  stream << "</CalibrationMatrix>";
  
  return(true);
}

inline
bool  VW::CalibrationMatrix::WriteBinary(std::ostream& stream) const
{
  VW::WriteBinary(stream,_f);
  VW::WriteBinary(stream,_a);
  VW::WriteBinary(stream,_s);
  VW::WriteBinary(stream,_px);
  VW::WriteBinary(stream,_py);
  return(true);
}

inline
bool  VW::CalibrationMatrix::ReadASCII(std::istream& stream)
{
  stream >> _f >> _a >> _s >> _px >> _py;
  return(true);
}

inline
bool VW::CalibrationMatrix::ReadXML(std::istream& stream, 
			  const std::string& )
{
  std::string tagname;
  std::istringstream contents(" ");
  if (VW::FindXMLBlock("CalibrationMatrix",stream,tagname)) {

    if (VW::GetXMLBlock("FocalLength",stream,contents,tagname))
      contents >> _f;
    else {
      std::cerr << "VW::CalibrationMatrix::ReadXML - error reading focal length.\n";
      return false;
    }

    if (VW::GetXMLBlock("AspectRatio",stream,contents,tagname))
      contents >> _a;
    else {
      std::cerr << "VW::CalibrationMatrix::ReadXML - error reading aspect ratio.\n";
      return false;
    }

    if (VW::GetXMLBlock("Skew",stream,contents,tagname))
      contents >> _s;
    else {
      std::cerr << "VW::CalibrationMatrix::ReadXML - error reading skew.\n";
      return false;
    }

    if (VW::GetXMLBlock("PrincipalPointX",stream,contents,tagname))
      contents >> _px;
    else {
      std::cerr << "VW::CalibrationMatrix::ReadXML - error reading principal point X-coord.\n";
      return false;
    }

    if (VW::GetXMLBlock("PrincipalPointY",stream,contents,tagname))
      contents >> _py;
    else {
      std::cerr << "VW::CalibrationMatrix::ReadXML - error reading principal point Y-coord.\n";
      return false;
    }

  } else {
    return(false);
  }
  return(true);
}

inline
bool VW::CalibrationMatrix::ReadBinary(std::istream& stream)
{
  VW::ReadBinary(stream,_f);
  VW::ReadBinary(stream,_a);
  VW::ReadBinary(stream,_s);
  VW::ReadBinary(stream,_px);
  VW::ReadBinary(stream,_py);
  return(true);
}

#endif // _calibrationmatrix_h_
