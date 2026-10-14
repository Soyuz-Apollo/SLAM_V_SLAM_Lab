/** 
 * @file   delaunay.h
 * @author DW Murray, F Champ and ID Reid
 * @date   22/03/1989		   	
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
 * @class    VW::Delaunay2D
 * @ingroup  GeometricAlgorithms
 * @author   ID Reid
 * @date     02/11/2002
 * @brief    Voronoi tessellation and Delaunay triangulation.
 *
 *  Modified from "fcdt.c", by IDR, 04/2001
 *  Still need to C++ify data structures and add access methdods
 *  probably should use stl for storing dlists
 */

#ifndef _delaunay_h_
#define _delaunay_h_

#include <vector>
#include "VW/GeomObjects/point2d.h"

namespace VW {

  // forward declare some other stuff
  class LineSeg2D;


  /**
   * namespace for Delaunay data (keeps it from cluttering the VW namespace).
   */
  namespace Delaunay {

    // point    = POINT    = Point
    // npointer = NPOINTER = NPointer
    // ve       = VE       = VEdge
    // de       = DE       = DEdge
    // dp       = DP       = DEdgePointer
    // dt       = DT       = Triangle
    struct Point { /* point */
      double		x,y;
      struct NPointer      *first_neighbour,
	                   *last_neighbour;
      struct Point	   *next,
 	                   *previous;
      int		    label, n_neighbour, status;
    };
    
    struct NPointer { /* neighbour */
      struct Point *point;
      struct NPointer *next, *previous;
      struct VEdge *ve;
      struct DEdge *de;
      int    de_traversed;
    };
    
    struct VEdge { /* Voronoi edge */
      int  a_inf,b_inf;
      double xa,ya,xb,yb;
      struct Point *p[2];
      struct VEdge *next,*previous;
      int label;
    };
    
    struct DEdge { /* Delaunay edge */
      struct Point *p[2];
      struct Triangle		*dt[2];
      struct DEdge *next,*previous;
      int label;
    };
    
    struct DEdgePointer { /* Edge pointer */
      struct DEdge		*de;
      struct DEdgePointer	*next;
    };
    
    
    struct Triangle { /* Delaunay triangle */
      struct DEdge *de[3];
      struct Triangle *next, *previous;
      double x, y; /* center of circumcircle */
      double sr;  /* square of the circle radius */
      int label;
    };
    
  };
  
  class Delaunay2D {
    
  public:
    
    Delaunay2D();
    Delaunay2D(int tlx, int tly, int brx, int bry);
    ~Delaunay2D();
    
    void ComputeDelaunay(std::vector<Point2D> &pointlist);
    void UpdateTessellation(Point2D *p);
    void UpdateTessellation(Point2D p);
    void UpdateTessellation(double x, double y);
    void set_window_limits(int tlx, int tly, int brx, int bry);
    void show_tess();
    void read_tess();
    void write_tess();
    // need to add more access methods
    std::vector<LineSeg2D> *GetDelaunayEdges();
    
  private:
    
    // private methods
    int ispoint_in_list(double x, double y);
    void add_point(double x, double y, int status);
    void init_tess();
    void increment_voronoi();
    void new_triangle_list();
    int isincircle(Delaunay::Triangle *triangle);
    void make_dp(Delaunay::DEdge *edge);
    void delete_triangle(Delaunay::Triangle *triangle);
    void new_edge_list();
    int ispoint_on_edge(Delaunay::DEdge *edge);
    void make_neighbours(Delaunay::Point *point1, 
			 Delaunay::Point *point2);
    void make_edge(Delaunay::Point *point1, 
		   Delaunay::Point *point2);
    Delaunay::DEdge *make_new_edge(Delaunay::Point *point1, 
				   Delaunay::Point *point2);
    void make_triangle(Delaunay::DEdge *edge1, 
		       Delaunay::DEdge *edge2, 
		       Delaunay::DEdge *edge3);
    void center_radius(Delaunay::Triangle *triangle);
    void delete_edge(Delaunay::DEdge *edge);
    void erase_neighbours(Delaunay::Point *point1, 
			  Delaunay::Point *point2);
    void read_neighbours(Delaunay::Point *point, 
			 Delaunay::Point **pt_point, 
			 Delaunay::DEdge **pt_de);
    
    // private data
    double gx[2], gy[2];  // window limits
    
    int n_point, n_ve, n_dt, n_de, 
      l_point, l_ve, l_de, l_dt;
    
    Delaunay::Point             *first_point, *last_point;
    Delaunay::VEdge	        *first_ve, *last_ve;
    Delaunay::DEdge	        *first_de, *last_de;
    Delaunay::Triangle	        *first_dt, *last_dt;
    Delaunay::DEdgePointer	*first_dp, *last_dp;
    
    int n_neighbour;
    
  };
}; // end namespace VW

#endif /* _delaunay_h_ */
