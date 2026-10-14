/** 
 * @file   linefit.cpp
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


// System includes
#include <math.h>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
#include <iostream>


// Local includes
#include "VW/Improc/edgel.h"
#include "linefit.h"

// VW includes
#include "VW/geomobjects.h"


#ifndef PI
#define PI 3.1415927
#endif

using namespace std;
using namespace VW;


double LineFit::CalcMergeScore(LineSegFitted &l1, LineSegFitted &l2, LineInfo& info) 
{
  info.sumx = l1.info.sumx + l2.info.sumx - l2.sp.GetX();
  info.sumy = l1.info.sumy + l2.info.sumy - l2.sp.GetY();
  info.sumxx  = l1.info.sumxx + l2.info.sumxx -  (l2.sp.GetX())*(l2.sp.GetX());
  info.sumxy  = l1.info.sumxy + l2.info.sumxy - l2.sp.GetX()*l2.sp.GetY();
  info.sumyy  = l1.info.sumyy + l2.info.sumyy - (l2.sp.GetY())*(l2.sp.GetY());
  info.n    = l1.info.n + l2.info.n - 1;

  // compute second moments
  double mxx = info.sumxx - (info.sumx * info.sumx) / info.n;
  double mxy = info.sumxy - info.sumx * info.sumy / info.n;
  double myy = info.sumyy - (info.sumy * info.sumy) / info.n;

  double lambda = (mxx + myy - sqrt((mxx - myy)*(mxx - myy) + 4*mxy*mxy)) / 2.0;

  info.R = lambda_lim - lambda / info.n;

  return info.R;
}


// Join adjacent edgels into putative line segments
void LineFit::StartLines(list<LineSegFitted>& lines, const EdgeString *s)
{
  deque<Edgel *>::const_iterator       e,enext;

  // for each edgel in the string
  for (e=s->begin(); e!=s->end(); e++) {
    enext = e;
    if (++enext != s->end()) {
      // create new segment with two edgels
      lines.push_back(LineSegFitted(*e, *enext));
    }
  }

  return;
}


void LineFit::MergeLines(list<LineSegFitted>& lines)
{
  list<LineSegFitted>::iterator l, lbest, lnext;
  LineInfo precal, best;

  while (true) {

    best.R = -2000000.0;

    // find best pair to merge
    for (l=lines.begin(); l!=lines.end(); l++) {
      lnext=l; ++lnext;
      if (lnext!=lines.end()) {
	precal.R = CalcMergeScore(*l, *lnext, precal);
	if (precal.R > best.R) {
	  best = precal;
	  lbest = l;
	}
      } 
    }
  
    // but stop all merging if best is below threshold
    if (best.R < min_conf) break;

    // merge this and next elements
    lnext = lbest; ++lnext;
    //LineSegFitted *l1 = *lbest, *l2 = lnext;

    lbest->info = best;
    lbest->ep = lnext->ep;
    // splice edgels from l2 to end of l1's list, excluding the first element
    list<Edgel *>::iterator sp=lnext->edgels.begin(); sp++;
    lbest->edgels.splice(lbest->edgels.end(), lnext->edgels, sp, lnext->edgels.end());

    lines.erase(lnext);

  }

  return;
}


// Fits line segments to an edge map
void LineFit::Compute(const EdgeData& edat, list<LineSegFitted> &lines)
{
  list<LineSegFitted> tmp;
  list<LineSegFitted>::iterator l;
  vector<EdgeString *>::const_iterator s;

  lines.clear();

  for (s=edat.strings.begin(); s!=edat.strings.end(); s++) {
    tmp.clear();
    StartLines(tmp, *s);             // create putative lines from pairs of points
    MergeLines(tmp);                 // attempt to merge
    tmp.remove_if(LenThresh(lengththresh));  // remove linesegs which are too small
    lines.splice(lines.end(), tmp);
  }

  for (l=lines.begin(); l!=lines.end(); l++) {
    l->CorrectEndpoints();
//				cout << "   line ("<<l->GetStartPoint()<<")<->("<<l->GetEndPoint()<<")"<<endl;
  }

  return;
}


// Fits line segments to an edge string
void LineFit::Compute(const EdgeString *s, 
		      list<VW::LineSegFitted> &lines)
{
  list<LineSegFitted> tmp;
  list<LineSegFitted>::iterator l;

  lines.clear();

  tmp.clear();
  StartLines(tmp, s);                     // create putative lines from pairs of points
  MergeLines(tmp);                        // attempt to merge
  tmp.remove_if(LenThresh(lengththresh));  // remove linesegs which are too small
  lines.splice(lines.end(), tmp);

  for (l=lines.begin(); l!=lines.end(); l++) {
    l->CorrectEndpoints();
  }

  return;
}
