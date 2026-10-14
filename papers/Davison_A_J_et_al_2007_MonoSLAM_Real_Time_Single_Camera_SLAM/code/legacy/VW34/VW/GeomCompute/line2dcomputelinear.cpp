
#include <VNL/Algo/svd.h>
#include <VW/GeomObjects/point2d.h>
#include <VW/GeomObjects/line2d.h>
#include <VW/GeomObjects/normalisation.h>

#include "line2dcomputelinear.h"

#ifdef VERBOSE
#   undef VERBOSE
#endif
#define VERBOSE false

using namespace std;

static inline void RecoverL(const VNL::SVD<double>& svd,
	VW::Line2D& result) 
{
  result.Set(svd.Nullvector());
}

static inline void BuildMatrixRow(VNL::Matrix<double>& D, unsigned int row,
	const VW::Point2D& x)
{
    D(row, 0) = x.GetX();
    D(row, 1) = x.GetY();
    D(row, 2) = 1.0;

}




namespace VW { namespace Line2DCompute {	
    void LinearFromPoints(VW::Line2D& result,
			  const std::vector<VW::Point2D>& x,
			  const bool normalise,
			  const std::vector<unsigned int>& inlierlist)
{
  // 4 steps:
  //   - Determine the optimal normalisation
  //   - Build the DLT matrix
  //   - Solve
  //   - Reconstruct

  VW::Normalisation n;


  // * Step 1 - Build normalised list *

  bool useinlierlist(inlierlist.size() > 0);

// Take a pointer to one or other inlier list;
std::vector<unsigned int> defaultinliers;
const std::vector<unsigned int>* which_features;

  unsigned int num_points;
  if (useinlierlist) {
    // Use only those on the inlier list
    num_points = static_cast<unsigned int>(inlierlist.size());
    which_features = &inlierlist;
  } else {
    num_points = static_cast<unsigned int>(x.size());
    // If inlier list was not setup initially, fill it now
    for (unsigned int i=0;i<num_points;i++) {
      defaultinliers.push_back(i);
    }
    which_features = &defaultinliers;

  }

assert(num_points > 1);

// As the line has 3 params any of which might be zero, whilst scale is free
// calculation from 2 points must be done differently.
if (num_points == 2) {

	VW::Point2D s(x[(*which_features)[0]].GetX(),x[(*which_features)[0]].GetY());
	VW::Point2D e(x[(*which_features)[1]].GetX(),x[(*which_features)[1]].GetY());
	if (normalise) {
	  n.Init(x,*which_features);
	  if (VERBOSE) std::cerr << "Norm = " << n.AsMatrix() << std::endl;
	  s = n.Normalise(s);
	  e = n.Normalise(e);
	}

	// Get the line using a segment
	result = VW::LineSeg2D(s,e).GetLine();

} else {

// Given n>2 points, do a standard least-squares

// Each point gives one row in the design matrix
  VNL::Matrix<double> D(num_points, 3);

  if (VERBOSE) {
    std::cerr << "Building measurement matrix using "<< num_points;
    std::cerr <<" features" << std::endl;
  }

// Code splits here, according to whether normalisation is used

  if (normalise) {
  // Get the normalisation -- This bit doesn't seem to be a huge computational burden
  n.Init(x,*which_features);

	if (VERBOSE) std::cerr << "Norm = " << n.AsMatrix() << std::endl;

  // * Step 2 - Build DLT *

  // Loop through design matrix D filling with values
  vector<unsigned int>::const_iterator it = which_features->begin();
  for (unsigned int index =0; index < num_points; index++, it++) {
	VW::Point2D norm_point = n.Normalise(x[*it]);   
	BuildMatrixRow(D,index,norm_point);

  }
} else {
// No normalisation, so skip straight to building the matrix

  // * Step 2 - Build DLT *

  // Loop through design matrix D filling with values
  vector<unsigned int>::const_iterator it = which_features->begin();
  for (unsigned int index =0; index < num_points; index++, it++) {
	const VW::Point2D p(x[*it].GetX(),x[*it].GetY());
	BuildMatrixRow(D,index,p);

  }
}
    if (VERBOSE) std::cerr <<" D=" << D << std::endl;


  // * Step 3 - Solve *
  VNL::SVD<double> svd (D);

  

  // * Step 4 - Recover *
  RecoverL(svd,result);

} // End if (n==2)

if (normalise) {
result = n.UnNormalise(result,VW::Line2D());
}


}


}; }; // End namespaces

