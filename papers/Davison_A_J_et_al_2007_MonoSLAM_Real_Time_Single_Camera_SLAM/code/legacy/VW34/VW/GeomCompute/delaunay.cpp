/** 
 * @file   delaunay.cpp
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

// System
//#include <math.h>
#include <malloc.h>
#include <iostream>
#include <vector>

// VW
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/lineseg2d.h>

// Local
#include "delaunay.h"

using namespace std;
using namespace VW;
using namespace VW::Delaunay;

// constructor
Delaunay2D::Delaunay2D() :
  n_point(0), n_ve(0), n_dt(0), n_de(0), 
  l_point(0), l_ve(0), l_de(0), l_dt(0),
  first_point(NULL), last_point(NULL),
  first_ve(NULL), last_ve(NULL),
  first_de(NULL), last_de(NULL),
  first_dt(NULL), last_dt(NULL),
  first_dp(NULL), last_dp(NULL)
{
  // do nothing
}

// constructor
Delaunay2D::Delaunay2D(int tlx, int tly, int brx, int bry) :
  n_point(0), n_ve(0), n_dt(0), n_de(0), 
  l_point(0), l_ve(0), l_de(0), l_dt(0),
  first_point(NULL), last_point(NULL),
  first_ve(NULL), last_ve(NULL),
  first_de(NULL), last_de(NULL),
  first_dt(NULL), last_dt(NULL),
  first_dp(NULL), last_dp(NULL)
{
  set_window_limits(tlx, tly, brx, bry);
  init_tess();
}

Delaunay2D::~Delaunay2D()
{
  // lots of memory freeing to do here but not implemented yet -- naughty!!
}

static void error(char *s)
{
  cerr << s << endl;
  std::exit(1);
}


void Delaunay2D::set_window_limits(int tlx, int tly, int brx, int bry)
{
  gx[0] = tlx;
  gy[0] = tly;
  gx[1] = brx;
  gy[1] = bry;
}


/*
 ****** MAIN ROUTINES ****** 
*/

void Delaunay2D::ComputeDelaunay(std::vector<Point2D> &pointlist)
{
  if (n_point!=4) 
    error("Need to set up boundary first\n");


  // should fix this to use an iterator...
  for (unsigned int i=0; i<pointlist.size(); i++) 
    UpdateTessellation(pointlist[i].GetX(), pointlist[i].GetY());

  return;
}


std::vector<LineSeg2D> *Delaunay2D::GetDelaunayEdges()
{
  std::vector<LineSeg2D> *edgelist = new std::vector<LineSeg2D>;
  DEdge *cur_de;

  edgelist->clear();

  cur_de = first_de;
  while (cur_de != NULL) {
    edgelist->push_back(LineSeg2D(cur_de->p[0]->x, cur_de->p[0]->y, 
				  cur_de->p[1]->x, cur_de->p[1]->y));
    cur_de = cur_de->next;
  }

  return edgelist;
}


int Delaunay2D::ispoint_in_list(double x, double y)
{
  Point	*point;

  if (x > first_point->next->next->x || x < first_point->x) {
    cerr << "Warning: x = " << x << " y = " << y << "is not in frame" << endl;
    return 0;
  }
  if (y > first_point->next->next->y || y < first_point->y) {
    cerr << "Warning: x = " << x << " y = " << y << "is not in frame" << endl;
    return 0;
  }
	
  point = first_point;
  while ( point != NULL ) {
    if ((point->x == x) && (point->y == y))  {
      cerr << "Warning: x = " << x << " y = " << y << "already in list" << endl;
      return 0;
    }
    point = point->next;
  }
  return 1;
}


void Delaunay2D::UpdateTessellation(double x, double y)
{
  add_point(x, y, 0);
  increment_voronoi();
}


void Delaunay2D::UpdateTessellation(Point2D p)
{
  add_point(p.GetX(), p.GetY(), 0);
  increment_voronoi();
}


void Delaunay2D::UpdateTessellation(Point2D *p)
{
  add_point(p->GetX(), p->GetY(), 0);
  increment_voronoi();
}


void Delaunay2D::add_point(double x, double y, int status)
{
  /*
   * This adds a point the list of points
   */
  if(n_point == 0) {
    first_point = last_point = new Point;
    first_point->previous = NULL;
  } else  {
    last_point->next = new Point;
    last_point->next->previous = last_point;
    last_point = last_point->next;
  }
  last_point->next = NULL;
  ++n_point;
  last_point->x=x;
  last_point->y=y;
  last_point->first_neighbour = NULL;
  last_point->last_neighbour = NULL;
  last_point->label = ++l_point;
  last_point->n_neighbour = 0;
  last_point->status = status;
}


/*
 **********Initialize tesselation ***********
*/
void Delaunay2D::init_tess(void)
{
  Point	*point1, *point2, *point3, *point4;
  DEdge	*edge1, *edge2, *edge3, *edge4, *edge5;

  if (n_point != 0)
    return;

  n_de = 0; n_dt = 0; 
  first_ve = NULL;
  last_ve  = NULL; 

  add_point(gx[0], gy[0], 1);
  point1 = last_point;
  add_point(gx[1], gy[0], 1);
  point2 = last_point;
  add_point(gx[1], gy[1], 1);
  point3 = last_point;
  add_point(gx[0], gy[1], 1);
  point4 = last_point;

  make_edge(point1, point2);
  edge1 = last_de;

  make_edge(point2, point3);
  edge2 = last_de;
 
  make_edge(point3, point4);
  edge3 = last_de;
 
  make_edge(point4, point1);
  edge4 = last_de;
 
  make_edge(point2, point4);
  edge5 = last_de;
 
  make_triangle(edge1, edge5, edge4);

  make_triangle(edge2, edge3, edge5);
}


void Delaunay2D::increment_voronoi(void)
{
  /*
    This increments the voronoi tessellation
    and Delaunay triangulation
  */
  if (n_point<5) {
    error("Fatal error: increment called with fewer than 5 points!");
  } else {
    new_triangle_list();
    new_edge_list();
  }
}


/*
************** NEW_TRIANGLE_LIST *******************
  new_triangle_list() will erase the triangles whose 
                      circumcircle contains  "last_point" and label
		      the associated edges 
*/
void Delaunay2D::new_triangle_list(void)
{
  Triangle  *current_dt, *next_dt;

  current_dt = first_dt;

  while (current_dt != NULL) {
    if (!isincircle(current_dt)) 
      current_dt = current_dt->next;
    else {
      ++(current_dt->de[0]->label);
      make_dp(current_dt->de[0]);
      ++(current_dt->de[1]->label);
      make_dp(current_dt->de[1]);
      ++(current_dt->de[2]->label);
      make_dp(current_dt->de[2]);
      /* If an edge appears twice it will be labeled 2
	 and will be erased in the 2nd pass */
	
      /* Now erase the triangle */	
      next_dt = current_dt->next;
      delete_triangle(current_dt);
      current_dt = next_dt;
    }  /* end if */
    
  }  /* End while */

} /* End "new_triangle_list()" */


/*
 *********** ISINCIRCLE ***************
 isincircle()
              returns 1 if  "last_point" is in circumcircle
	      and 0 if not 
*/
int Delaunay2D::isincircle(Triangle *triangle)
{
  double sq_distance;

  sq_distance = (last_point->x - triangle->x) * (last_point->x - triangle->x);
  sq_distance += (last_point->y - triangle->y) * (last_point->y - triangle->y);

  return (sq_distance <= triangle->sr);
}


/*
 ********* MAKE_DP ********************
 Adds "edge" in the list of edges
 pointed by "first_dp" 
*/
void Delaunay2D::make_dp(DEdge *edge)
{
    if (edge->label == 2) 
	return; /* de_ptr already exists */
    if (first_dp == NULL) {
	first_dp = last_dp = new DEdgePointer;
    } else {
	last_dp->next = new DEdgePointer;
	last_dp = last_dp->next;
    }
    last_dp->de = edge;
    last_dp->next = NULL;
}


/*
 *********** DELETE_TRIANGLE ***********
*/
void Delaunay2D::delete_triangle(Triangle *triangle)
{
  if (triangle->previous != NULL)
    triangle->previous->next = triangle->next;
  else {
    first_dt = triangle->next;
  }

  if (triangle->next != NULL)
    triangle->next->previous = triangle->previous;
  else {
    last_dt = triangle->previous;
  }
  --n_dt;

  /* "Update triangle->de[]->dt[]" */
  if (triangle->de[0]->dt[0] == triangle) 
    triangle->de[0]->dt[0] = triangle->de[0]->dt[1];
  triangle->de[0]->dt[1] = NULL;

  if (triangle->de[1]->dt[0] == triangle) 
    triangle->de[1]->dt[0] = triangle->de[1]->dt[1];
  triangle->de[1]->dt[1] = NULL;

  if (triangle->de[2]->dt[0] == triangle) 
    triangle->de[2]->dt[0] = triangle->de[2]->dt[1];
  triangle->de[2]->dt[1] = NULL;

  delete triangle;
}


/*
 ************* NEW_EDGE_LIST **************
*/
void Delaunay2D::new_edge_list(void)
{
  DEdgePointer *current_dp, *previous_dp;
  DEdge *edge1, *edge2;

  if ((current_dp = first_dp) == NULL) {
    error("erase_edge() called as first_dp = NULL");
  }
 
  while (current_dp != NULL) {
    previous_dp = current_dp;
    switch (current_dp->de->label)  
      {
      case 0:
	error("One edge's label is 0");
	break;
      case 1:
	if (ispoint_on_edge(current_dp->de)) 
	  delete_edge(current_dp->de);
	else {
	  current_dp->de->label = 0;
	  edge1 = make_new_edge(last_point,current_dp->de->p[0]);
	  edge2 = make_new_edge(last_point,current_dp->de->p[1]);
	  make_triangle(current_dp->de, edge1, edge2);
	}
	current_dp = current_dp->next;
	break;
      case 2:
	
	/* The edge must be deleted */
	delete_edge(current_dp->de);
	current_dp = current_dp->next;
	break;

      default:
	error("One edge's label is greater than 2");
	break;
      }  /* end switch */

    delete previous_dp;

  }  /* end while */

  first_dp = last_dp = NULL;
	
}  /* End of "new_edge_list()"*/


int Delaunay2D::ispoint_on_edge(DEdge *edge)
{
  double	delta;
  double	x1, x2, y1, y2;

  x1 = edge->p[0]->x - edge->p[1]->x;
  y1 = edge->p[0]->y - edge->p[1]->y;
  x2 = edge->p[0]->x - last_point->x;
  y2 = edge->p[0]->y - last_point->y;
  delta = (x1 * y2) - (y1 * x2);
  return (delta==0);
}


void Delaunay2D::make_neighbours(Point *point1, Point *point2)
{
  NPointer *new_neighbour;

  /*now make two points neighbours*/
  /* put point2 in point1's neighbourhood list.*/

  new_neighbour = new NPointer;

  new_neighbour->point = point2;
  new_neighbour->next = NULL;
  new_neighbour->previous = point1->last_neighbour;
  point1->last_neighbour =  new_neighbour;
  if (point1->first_neighbour != NULL) {
    new_neighbour->previous->next = new_neighbour;
  } else {
    point1->first_neighbour = new_neighbour;
  }
	
  /*next  put point1 in point2's neighbourhood list.*/

  new_neighbour = new NPointer;
  
  new_neighbour->point = point1;
  new_neighbour->next = NULL;
  new_neighbour->previous = point2->last_neighbour;
  point2->last_neighbour =  new_neighbour;
  if (point2->first_neighbour != NULL) {
    new_neighbour->previous->next = new_neighbour;
  } else {
    point2->first_neighbour = new_neighbour;
  }		
}


void Delaunay2D::make_edge(Point *point1, Point *point2)
{
  DEdge	*new_edge;
  /*This makes an edge out of two points */
  
  make_neighbours(point1, point2);
  ++(point1->n_neighbour);
  ++(point2->n_neighbour);
  new_edge = new DEdge;
  if (last_de != NULL) 
    last_de->next = new_edge;
  else {
    first_de = new_edge;
  }
  new_edge->p[0] = point1;
  new_edge->p[1] = point2;
  new_edge->next = NULL;
  new_edge->previous = last_de;
  last_de = new_edge;
  new_edge->label = 0;
  ++n_de;

  point1->last_neighbour->de = new_edge;
  point2->last_neighbour->de = new_edge;

}


DEdge *Delaunay2D::make_new_edge(Point *point1, Point *point2)
{
  DEdge	   *new_edge;
  NPointer *current_neighbour;

  /* First check whether point1 and point2 are not already neighbours */
  
  current_neighbour = point1->first_neighbour;
  while (current_neighbour != NULL) {
    if (current_neighbour->point == point2) 
      return current_neighbour->de;
    current_neighbour = current_neighbour->next;
  }

  /*This makes an edge out of two points */

  new_edge = new DEdge;
  if (last_de != NULL) {
    last_de->next = new_edge;
    new_edge->previous = last_de;
  } else {
    first_de = new_edge;
  }
  new_edge->p[0] = point1;
  new_edge->p[1] = point2;
  new_edge->next = NULL;
  last_de = new_edge;
  new_edge->label = 0;
  ++n_de;
  make_neighbours(point1, point2);
  ++(point1->n_neighbour);
  ++(point2->n_neighbour);
  point1->last_neighbour->de = new_edge;
  point2->last_neighbour->de = new_edge;
  return new_edge;
}


/*
 *************** MAKE_TRIANGLE *****************
*/
void Delaunay2D::make_triangle(DEdge *edge1, DEdge *edge2, DEdge *edge3)
{
  Triangle *new_triangle;

  new_triangle  = new Triangle;
  if (last_dt != NULL) 
    last_dt->next = new_triangle;
  else {
    first_dt = new_triangle;
  }
  new_triangle->de[0] = edge1;	
  new_triangle->de[1] = edge2;
  new_triangle->de[2] = edge3;
  new_triangle->next = NULL;
  new_triangle->previous = last_dt;
  last_dt = new_triangle;
  new_triangle->label = 0;
  ++n_dt;

  /* Now compute the center and radius of circumcircle */
  center_radius(new_triangle);

  /* Update "edge->dt[]" */
  if (edge1->dt[0] == NULL) edge1->dt[0] = new_triangle;
  else edge1->dt[1] = new_triangle;
  if (edge2->dt[0] == NULL) edge2->dt[0] = new_triangle;
  else edge2->dt[1] = new_triangle;
  if (edge3->dt[0] == NULL) edge3->dt[0] = new_triangle;
  else edge3->dt[1] = new_triangle;

}


/*
 *************** CENTER_RADIUS *****************
*/
void Delaunay2D::center_radius(Triangle *triangle)
{
  double	x1, x2, x3, y1, y2, y3;
  double	xc, yc, lambda_n, lambda_d, lambda, sq_rad;

  x1 = triangle->de[0]->p[0]->x;
  y1 = triangle->de[0]->p[0]->y;
  x2 = triangle->de[0]->p[1]->x;
  y2 = triangle->de[0]->p[1]->y;
  x3 = triangle->de[1]->p[1]->x;
  y3 = triangle->de[1]->p[1]->y;

  /* Make sure the last point is none of the previous */
  if (((x3 == x2) && (y3 == y2)) || ((x3 == x1) && (y3 == y1))) {
    x3 = triangle->de[1]->p[0]->x;
    y3 = triangle->de[1]->p[0]->y;
	
  }

  lambda_n = (x1 - x3) * (x2 - x1) + (y1 - y3) * (y2 - y1);
  lambda_d = (y3 - y2) * (x2 - x1) - (x3 - x2) * (y2 - y1);
  lambda = lambda_n / lambda_d;
  xc = (x2 + x3 + lambda * (y3 - y2)) / 2;
  yc = (y2 + y3 - lambda * (x3 - x2)) / 2;
  sq_rad = (x1 - xc) * (x1 - xc) + (y1 - yc) * (y1 - yc);

  triangle->x = xc;
  triangle->y = yc;
  triangle->sr = sq_rad;

}    /* End of "center_radius()" */


void Delaunay2D::delete_edge(DEdge *edge)
{
    erase_neighbours(edge->p[0],  edge->p[1]);
    --(edge->p[0]->n_neighbour); 
    --(edge->p[1]->n_neighbour); 
    if (edge->previous != NULL) 
	edge->previous->next = edge->next;
    else {
	first_de = edge->next;
    }

    if (edge->next != NULL) 
	edge->next->previous = edge->previous;
    else {
	last_de = edge->previous;
    }
    --n_de;
    delete edge;
}


void Delaunay2D::erase_neighbours(Point *point1, Point *point2)
{
    NPointer	 *current;

    /*first erase point2 from point1's neighbourhood list.*/
    current = point1->first_neighbour;
    while (current->point != point2)
	current = current->next;
    if (current->previous == NULL)
	point1->first_neighbour = current->next;
    else {
	current->previous->next = current->next;
    }
    if (current->next == NULL)
	point1->last_neighbour = current->previous;
    else {
	current->next->previous = current->previous;
    }

    delete current;	


    /* erase point1 from point2's neighbourhood list.*/
    current = point2->first_neighbour;
    while (current->point != point1)
	current = current->next;
    if (current->previous == NULL)
	point2->first_neighbour = current->next;
    else {
	current->previous->next = current->next;
    }
    if (current->next == NULL)
	point2->last_neighbour = current->previous;
    else {
	current->next->previous = current->previous;
    }
    
    delete current;	
} /* end "erase_neighbours()"*/


/*
 ****** Write tessalation in a user-understandable way ********
*/
void Delaunay2D::show_tess(void)
{
    Point	*cur_point;
    NPointer	*cur_neigh;
    DEdge	*cur_de;
    Triangle	*cur_dt;
    int		j;

    j = 0;
    cur_de = first_de;
    while (cur_de != NULL) {
	cur_de->label = ++j;
	cur_de = cur_de->next;
    }

    j = 0;
    cur_dt = first_dt;
    while (cur_dt != NULL) {
	cur_dt->label = ++j;
	cur_dt = cur_dt->next;
    }

    /* First write points and their neighbours */
    cur_point = first_point;
    cout << "POINTS :";
    while (cur_point != NULL) {
	cout << endl << cur_point->label << " : x = " 
	     << cur_point->x << " y = " << cur_point->y << endl;
	cur_neigh = cur_point->first_neighbour;
	cout << "\t" << cur_point->n_neighbour << " NEIGHBOURS : "; 
	while (cur_neigh != NULL) {
	  cout << " " << cur_neigh->point->label << ",";
	  cur_neigh = cur_neigh->next;
	}
	cur_point = cur_point->next;
    }

    /* Write edges */
    cur_de = first_de;
    cout << "\nEDGES :\n";
    while (cur_de != NULL) {
      cout << cur_de->label << " : points " << cur_de->p[0]->label 
	   << " , " << cur_de->p[1]->label;
      cout << " triangle " <<  cur_de->dt[0]->label << " , " 
	   << ((cur_de->dt[1] != NULL)? cur_de->dt[1]->label: 0 ) << endl;
      cur_de = cur_de->next;
    }

    /* Write triangles */
    j = 0;
    cur_dt = first_dt;
    cout << "\nTRIANGLES :\n";
    while (cur_dt != NULL) {
      cout <<  ++j << " : edges " << cur_dt->de[0]->label 
	   << ", " << cur_dt->de[1]->label << ", " << cur_dt->de[2]->label << "\t";
      cout << "x = " << cur_dt->x << "\t , y = " 
	   << cur_dt->y << " ,sr = " << cur_dt->sr << endl;
      cur_dt = cur_dt->next;
    }
} /* End show_tess() */


/*
 *************** Read old tesselation from stdin ************
*/
void Delaunay2D::read_tess()
{
    Point *previous_point, **pt_point;
    DEdge	*previous_de, **pt_de;
    Triangle	**pt_dt;
    Triangle	*previous_dt;
    int	i, l, m, n;

    /* Read n_point, n_de, n_dt */

    cin >> n_point;
    cin >> n_de;
    cin >> n_dt;
 
    /* Allocate memory for 3 arrays of pointers, for points,
       edges and triangles */

    pt_point = new Point * [n_point + 1];
    if (pt_point == NULL) error("No more memory for point-pointers");
    pt_point [n_point] = NULL;

    pt_de = new DEdge * [n_de + 1];
    if (pt_de == NULL) error("No more memory for de-pointers");
    pt_de[n_de] = NULL;

    pt_dt = new Triangle * [n_dt + 1];
    if (pt_dt == NULL) error("No more memory for dt-pointers");
    pt_de[n_dt] = NULL;

    for (i = 0; i < n_point; i++)
	pt_point[i] = new Point;

    for (i = 0; i < n_de; i++)
	pt_de[i] = new DEdge;

    for (i = 0; i < n_dt; i++)
	pt_dt[i] = new Triangle;

    /* read points and neighbours */

    previous_point = NULL;
    first_point = pt_point[0];
    l_point = 0;
    for ( i = 0; i < n_point; i++) {
      cin >> pt_point[i]->x >> pt_point[i]->y;
      pt_point[i]->label = ++l_point;
      pt_point[i]->next = pt_point[i + 1];
      pt_point[i]->previous = previous_point;
      previous_point = pt_point[i];
      read_neighbours(previous_point, pt_point, pt_de);
    } 	
    
    last_point = pt_point[n_point - 1];

    /* read Delaunay edges */

    previous_de = NULL;
    first_de  = pt_de[0];

    for (i = 0; i < n_de; i++) {
	cin >> l >> m;
	pt_de[i]->next = pt_de[i + 1];
	pt_de[i]->previous = previous_de;
	pt_de[i]->p[0] = pt_point[l - 1];
	pt_de[i]->p[1] = pt_point[m - 1];
	cin >> l >> m;
	pt_de[i]->dt[0] = pt_dt[l - 1];
	pt_de[i]->dt[1] = ((m != 0) ? pt_dt[m - 1]: NULL);
	pt_de[i]->label = 0;
	previous_de = pt_de[i];
    } 
    last_de = pt_de[n_de - 1];

    /* read Delaunay triangles */

    previous_dt = NULL;
    first_dt = pt_dt[0];

    for ( i = 0; i < n_dt; i++) {
	cin >> l >> m >> n;
	pt_dt[i]->next = pt_dt[i + 1];
	pt_dt[i]->de[0] = pt_de[l - 1];
	pt_dt[i]->de[1] = pt_de[m - 1];
	pt_dt[i]->de[2] = pt_de[n - 1];
	cin >> pt_dt[i]->x >> pt_dt[i]->y >> pt_dt[i]->sr;
	pt_dt[i]->previous = previous_dt;
	previous_dt = pt_dt[i];
    } 
    last_dt = pt_dt[n_dt - 1];

    /* Free memory from the arrays of pointer */
    delete pt_point;
    delete pt_de;
    delete pt_de;

}


void Delaunay2D::read_neighbours(Point *point, 
				 Point **pt_point, 
				 DEdge **pt_de) 
{
    NPointer 	*cur_pt, *previous_pt;
    int		l, m;

    point->first_neighbour = cur_pt = new NPointer;
    cin >> l >> m;
    cur_pt->point = pt_point[ l - 1];
    cur_pt->de = pt_de[m - 1];
    cur_pt->previous = NULL;
    n_neighbour = 1;

    do {
	cin >> l >> m;
	if ( l != 0 && m!= 0) {
	    previous_pt = cur_pt;
	    cur_pt->next = new NPointer;
	    cur_pt = cur_pt->next;
	    cur_pt->point = pt_point[ l - 1];
	    cur_pt->de = pt_de[m - 1];
	    cur_pt->previous = previous_pt;
	    ++n_neighbour;
	}
	
    } while ( l != 0 && m!= 0);
    cur_pt->next = NULL;
    point->last_neighbour = cur_pt;
    point->n_neighbour = n_neighbour;
}


/*
 *************** Save tessalation in a binary file *****************
*/
void Delaunay2D::write_tess(void)
{
    Point	*cur_point, *previous_point;
    NPointer	*cur_neigh, *previous_neigh;
    DEdge		*cur_de, *previous_de;
    Triangle		*cur_dt,*previous_dt;
    int	i, j; 

    /* This is an attempt to keep the links between the structures
       of different types */

    cur_point = first_point;
    for (j = 0; cur_point != NULL; cur_point = cur_point->next) 
	cur_point->label = ++j;
    if (n_point != j) error("n_point is not the exact number of points");


    cur_de = first_de;
    for (j = 0; cur_de != NULL; cur_de = cur_de->next) 
	cur_de->label = ++j;
    if (n_de != j) error("n_de is not the exact number of edges");

    cur_dt = first_dt;
    for (j = 0; cur_dt != NULL; cur_dt = cur_dt->next) 
	cur_dt->label = ++j;
    if (n_dt != j) error("n_dt is not the exact number of triangles");
    

    /* First write n_point, n_de, n_de into file */
    cout << n_point << " " << n_de << " " << n_dt << endl;

    /*  write points and their neighbours */
    cur_point = first_point;
    while (cur_point != NULL) {
      cout << cur_point->x << " " << cur_point->y;
      cur_neigh = cur_point->first_neighbour;
      while (cur_neigh != NULL) {
	previous_neigh = cur_neigh;
	cout << "\t" << cur_neigh->point->label << " " << cur_neigh->de->label << endl;;
	cur_neigh = cur_neigh->next;
	delete previous_neigh;
      }
      cout << "\t0 0\n";
      cur_point = cur_point->next;
    }
    cout << endl;


    /* Write edges */
    cur_de = first_de;
    while (cur_de != NULL) {
      cout << cur_de->p[0]->label << " " << cur_de->p[1]->label << " " << cur_de->dt[0]->label;
      i = (cur_de->dt[1] == NULL) ? 0 : cur_de->dt[1]->label;
      cout << i << endl;
      cur_de = cur_de->next;
    }
    cout << endl;

    /* Write triangles */
    cur_dt = first_dt;
    while (cur_dt != NULL) {
      cout << cur_dt->de[0]->label << " " 
	   << cur_dt->de[1]->label << " " 
	   << cur_dt->de[2]->label << " " 
	   << cur_dt->x << " " <<  cur_dt->y << " " <<  cur_dt->sr << endl;
      cur_dt = cur_dt->next;
    }
    cout << endl;

    /* Now free memory allocated for those structures */
    cur_point = first_point;
    while ((previous_point = cur_point) != NULL) {
	cur_point = cur_point->next;
	delete previous_point;
    }

    /* free edges */
    cur_de = first_de;
    while ((previous_de = cur_de) != NULL) {
	cur_de = cur_de->next;
	delete previous_de;
    }

    /* free triangles */
    cur_dt = first_dt;
    while ((previous_dt = cur_dt) != NULL) {
	cur_dt = cur_dt->next;
	delete previous_dt;
    }

} /* End write_tess() */
