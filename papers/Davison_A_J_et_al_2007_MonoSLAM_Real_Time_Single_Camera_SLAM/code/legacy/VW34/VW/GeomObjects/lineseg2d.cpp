/** 
 * @file   lineseg2d.cc
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


//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <math.h>

#include <string>
#include <iostream>

// Local includes
#include "point2d.h"
#include "lineseg2d.h"

// NB Inline functions given in .h, not in the .cc file

using namespace std;
using namespace VW;

////////////////////////////
// Creation / Destruction //
////////////////////////////

LineSeg2D::LineSeg2D() : 
  sp(0,0), ep(0,0)
{
  // Nothing much to create...
}


LineSeg2D::LineSeg2D(const double x0, const double y0, 
		     const double x1, const double y1) :
  sp(x0, y0), ep(x1, y1)
{
  // Nothing much to create...
}

LineSeg2D::LineSeg2D(const Point2D& p0, const Point2D& p1) :
  sp(p0), ep(p1)
{
  // Nothing much to create...
}

LineSeg2D::LineSeg2D(const Point2D *p0, const Point2D *p1) :
  sp(*p0), ep(*p1)
{
  // Nothing much to create...
}

LineSeg2D &LineSeg2D::operator=(const LineSeg2D &ls) 
{
  if (this != &ls) {
    sp=ls.sp;
    ep=ls.ep;
  } else {
    cerr << "Error: copying LineSeg2D into itself!" << endl;
  }
  return *this;
}


LineSeg2D::~LineSeg2D() 
{
  // Nothing useful to delete, so a bit empty really.
}

LineSeg2D LineSeg2D::Truncate(const double x0, const double y0,
			      const double x1, const double y1) const
{
  // Check the easy solutions (completely outside window)
  const bool start_to_left = sp.GetX() < x0;
  const bool end_to_left = ep.GetX() < x0;
  if (start_to_left && end_to_left) return LineSeg2D(0,0,0,0);

  const bool start_to_right = sp.GetX() > x1;
  const bool end_to_right = ep.GetX() > x1;
  if (start_to_right && end_to_right) return LineSeg2D(0,0,0,0);

  const bool start_above = sp.GetY() < y0;
  const bool end_above = ep.GetY() < y0;
  if (start_above && end_above) return LineSeg2D(0,0,0,0);

  const bool start_below = sp.GetY() > y1;
  const bool end_below = ep.GetY() > y1;
  if (start_below && end_below) return LineSeg2D(0,0,0,0);


  // Check if completely in image
  if (!start_to_left && !start_to_right 
      && !start_above && !start_below 
      && !end_to_left && !end_to_right 
      && !end_above && !end_below) return *this;

  // Now the hard bit - it's partially in view
  const VW::Vector2D direc = GetDirection();
  VW::Point2D new_sp(sp), new_ep(ep);
  if (start_to_left) {
    double delta = x0 - new_sp.GetX();
    new_sp += delta*direc/direc.GetX();
  } else if (start_to_right) {
    double delta = new_sp.GetX() - x1;
    new_sp -= delta*direc/direc.GetX();
  }
  if (end_to_left) {
    double delta = x0 - new_ep.GetX();
    new_ep += delta*direc/direc.GetX();
  } else if (end_to_right) {
    double delta = new_ep.GetX() - x1;
    new_ep -= delta*direc/direc.GetX();
  }
  // Now examine new ends
  const bool new_start_above = new_sp.GetY() < y0;
  const bool new_end_above = new_ep.GetY() < y0;
  if (new_start_above && new_end_above) return LineSeg2D(0,0,0,0);

  const bool new_start_below = new_sp.GetY() > y1;
  const bool new_end_below = new_ep.GetY() > y1;
  if (new_start_below && new_end_below) return LineSeg2D(0,0,0,0);
  if (new_start_above) {
    double delta = y0 - new_sp.GetY();
    new_sp += delta*direc/direc.GetY();
  } else if (new_start_below) {
    double delta = new_sp.GetY() - y1;
    new_sp -= delta*direc/direc.GetY();
  }
  if (new_end_above) {
    double delta = y0 - new_ep.GetY();
    new_ep += delta*direc/direc.GetY();
  } else if (new_end_below) {
    double delta = new_ep.GetY() - y1;
    new_ep -= delta*direc/direc.GetY();
  }



  return LineSeg2D(new_sp, new_ep);
  
}

double 
LineSeg2D::GetAngle() const
{
  const VW::Vector2D dir(ep-sp);
  return atan2(dir.GetY(),
	       dir.GetX());
}

double 
LineSeg2D::GetDistanceToOrigin() const
{
  return GetDistanceToLine(VW::Point2D(0,0));
}

bool LineSeg2D::operator==(const LineSeg2D &ls) 
{
  return (sp==ls.sp && ep==ls.ep);
}


double operator*(const LineSeg2D &ls1, const LineSeg2D &ls2) 
{
  VW::Vector2D d1(ls1.ep - ls1.sp);
  VW::Vector2D d2(ls2.ep - ls2.sp);

  return (d1.GetX() * d2.GetX() +
	  d1.GetY() * d2.GetY());
}






double LineSeg2D::GetDistanceToLine(const VW::Point2D& testpoint) const
{
  Point2D perp(ep._y-sp._y, sp._x-ep._x);
  return fabs((testpoint-sp)*perp/GetLength());
}


double LineSeg2D::GetDistanceToLineSeg(const VW::Point2D& testpoint) const
{
  const double dot = (ep-sp)*(testpoint-sp)/(GetLength()*GetLength());

  if (dot < 0.0) {
    // closest point on lineseg is sp
    //cerr << "sp closest\n";
    return testpoint.GetDistance(sp);
  } else if (dot > 1.0) {
    // closest point on lineseg is ep
    //cerr << "ep closest\n";
    return testpoint.GetDistance(ep);
  } else {    
    Vector2D perp(ep._y-sp._y, sp._x-ep._x);
    //cerr << "middle closest\n";
    //cerr << "perp * vec = " << perp * (ep-sp) << endl;
    return fabs((testpoint-sp)*perp/GetLength());
  }
}

bool LineSeg2D::GetIntersection(const VW::LineSeg2D& lineseg, VW::Point2D& result) const
{
  VW::Point2D delta = ep - sp; 
  VW::Point2D vect_sp = lineseg.sp - sp;
  VW::Point2D vect_ep = lineseg.ep - sp;

  // The Z component of the cross product is negative for left-handed set
  const double zs = delta.GetX()*vect_sp.GetY() 
                  - delta.GetY()*vect_sp.GetX();
  const double ze = delta.GetX()*vect_ep.GetY() 
                  - delta.GetY()*vect_ep.GetX();

  // Check if both on same side (zs,ze same sign)
  if (zs*ze > 0) {
    result.Set(0,0);
    return false;
  }

  // OK on different sides, so use similar triangles to get intercept
  delta.Normalise();
  const double dist_sp = GetDistanceToLine(lineseg.sp);
  const double dist_ep = GetDistanceToLine(lineseg.ep);
  const double tot = dist_sp+dist_ep;


  result = (dist_sp*lineseg.ep + dist_ep*lineseg.sp)/tot;

  const double l = GetLength();

  if ((result.GetDistance(sp) > l) || (result.GetDistance(ep) > l)) {
    // Is beyond one end
    return false;
  }


  return true;
}

ostream& VW::operator<<(ostream& s, const LineSeg2D& ls) {
  ls.WriteASCII(s);
  return s;
}


istream& VW::operator>>(istream& s, LineSeg2D& ls) {
  ls.ReadASCII(s);
  return s;
}

bool VW::LineSeg2D::WriteASCII(std::ostream& stream) const
{
  stream << sp << ep;
  return stream.good();
}

bool VW::LineSeg2D::WriteXML(std::ostream& stream,
			     const std::string& name) const
{
  // Start tag
  if (name.size()>0)
    stream << "<LineSeg2D NAME=\"" << name << "\"> ";
  else
    stream << "<LineSeg2D> ";
  
  // Contents
  sp.WriteXML(stream,"Start");
  ep.WriteXML(stream,"End");
  
    // End tag
  stream << "</LineSeg2D>";
  
  return(true);
}

bool VW::LineSeg2D::WriteBinary(std::ostream& stream) const
{
  bool success = sp.WriteBinary(stream);
  success = success && ep.WriteBinary(stream);
  return success;
}

bool VW::LineSeg2D::ReadASCII(std::istream& stream)
{
  stream >> sp >> ep;
  return stream.good();
}

bool VW::LineSeg2D::ReadXML(std::istream& stream, 
			    const std::string& name)
{
  std::string tagname;
  std::istringstream contents(" ");
  if (VW::FindXMLBlock("LineSeg2D",stream,tagname)) {
    if (!sp.ReadXML(contents)) {
      std::cerr << "VW::LineSeg2D::ReadXML - error reading start point.\n";
      return false;
    }
    if (!ep.ReadXML(contents)) {
      std::cerr << "VW::LineSeg2D::ReadXML - error reading end point.\n";
      return false;
    }
  } else {
    return(false);
  }
  return(true);
}

bool VW::LineSeg2D::ReadBinary(std::istream& stream)
{
  bool success = sp.ReadBinary(stream);
  success = success && ep.ReadBinary(stream);
  return success;
}
