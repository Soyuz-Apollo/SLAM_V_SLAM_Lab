#include <iostream>
#include <VNL/vector.h>
#include <VNL/matrix.h>
#include <VNL/crossproductmatrix.h>
#include <VNL/vnlmath.h> // For Sgn(..)
#include <VNL/leastsquaresfunction.h>
#include <VNL/Algo/svd.h>
#include <VNL/Algo/levenbergmarquardt.h>
#include <VNL/Algo/symmetriceigensystem.h>

#include <VW/GeomObjects/fmatrixplanar.h>
#include <VW/GeomObjects/point2d.h>

#include "fmplanarcomputenonlinear.h"

using namespace std;

// Select one parameterisation.  But don't use VLJK (it turns out to be wrong)!
//#define METHOD_HZ
//#define METHOD_VLJK
#define METHOD_HZJK

namespace VW { namespace FMPlanarCompute {

  /** Parameterises F as [e2]x [ls]x [e1]x (see HZ 8.4 p233).
   * Basically copied from oxl/mvl/FMPlanarNonLinFun.cxx
   */

  VNL::Vector<double> 
  GenerateParameterVectorHZ(const VW::FMatrixPlanar& fmatrix)
  {
    VW::Point2D e1, e2;
    fmatrix.GetEpipoles (e1, e2);

    VNL::Matrix<double> F = fmatrix.AsMatrix ();
    F /= F.FrobeniusNorm();

    VNL::SymmetricEigensystem<double>  symm_eig (F + F.Transpose());

    double eig0 = symm_eig.D (0,0);
    double eig1 = symm_eig.D (2,2);
     
    if (eig0 > 0 || eig1 < 0) {
      cerr << "ERROR in VW::TransformLeastSquaresFunction<VW::FMatrixPlanar,VW::Point2D>";
      cerr << "::GenerateParameterVector:\n   SymmetricEigensystem<double>  is unsorted: ";
      cerr << symm_eig.D << endl;
      abort();
    }
     
    if (fabs(symm_eig.D(1,1)) > 1e-12) {
      cerr << "VW::TransformLeastSquaresFunction<VW::FMatrixPlanar,VW::Point2D>: WARNING:\n";
      cerr << "middle eigenvalue not 0: " << symm_eig.D << endl;
    }

    VNL::Vector<double> v0 (symm_eig.GetEigenvector(0));
    VNL::Vector<double> v1 (symm_eig.GetEigenvector(2));

    VNL::VectorFixed<3,double> f1 = sqrt(eig1)*v1 + sqrt(-eig0)*v0;
    VNL::VectorFixed<3,double> f2 = sqrt(eig1)*v1 - sqrt(-eig0)*v0;

    // Choose between f1 and f2 for the image of the screw axis, ls.
    // Do so by finding the epipolar line associated with the intersection of
    // f1 and f2, which will be algebraically equal to lh.
    VNL::VectorFixed<3,double> s = Cross3D (f1, f2);
    VNL::VectorFixed<3,double> lhn = F * s;
    lhn *= VNL::Sgn (lhn[2]) / lhn.Magnitude ();
    VNL::VectorFixed<3,double> f1n = f1 * VNL::Sgn (f1[2]) / f1.Magnitude ();
    VNL::VectorFixed<3,double> f2n = f2 * VNL::Sgn (f2[2]) / f2.Magnitude ();
    VNL::VectorFixed<3,double> ls;
    if ((lhn - f1n).Magnitude () < (lhn - f2n).Magnitude ()) ls = f2;
    else ls = f1;
    ls.Normalize ();

    VNL::VectorFixed<3,double> e1n(e1.GetX(), e1.GetY(), 1.0);
    VNL::VectorFixed<3,double> e2n(e2.GetX(), e2.GetY(), 1.0);
    e1n.Normalize ();
    e2n.Normalize ();

    VNL::Vector<double> params (9);
    params[0] = ls[0];
    params[1] = ls[1];
    params[2] = ls[2];
    params[3] = e1n[0];
    params[4] = e1n[1];
    params[5] = e1n[2];
    params[6] = e2n[0];
    params[7] = e2n[1];
    params[8] = e2n[2];

    return params;
  }
  VW::FMatrixPlanar ReconstructTransformHZ(const VNL::Vector<double>& x)
  {
    assert (x.size () == 9);

    // F = [e2]x [ls]x [e1]x
    const double* v = x.DataBlock();
    VNL::CrossProductMatrix<double> L(v);
    VNL::CrossProductMatrix<double> E1(v+3);
    VNL::CrossProductMatrix<double> E2(v+6);
     
    VNL::Matrix<double> fmat = E2 * L * E1;

    return VW::FMatrixPlanar(fmat / fmat.FrobeniusNorm());
  }

  /** Parameterises F as antisymmetric part and symmetric part, but symmetric
   * part is parameterised as fixed lines lh and ls as per Vieville and
   * Lingrand (ECCV96).
   * So F = [fa]x + (lh.ls' + ls.lh')
   *
   * ERROR: It turns out this is an under-parameterisation, as fa is a point on
   *        lh and therefore only constitues one parameter, not two.
   */

  VNL::Vector<double> GenerateParameterVectorVLJK(const VW::FMatrixPlanar& fmatrix)
  {
    VW::Point2D e1, e2;
    fmatrix.GetEpipoles (e1, e2);

    VNL::Matrix<double> F = fmatrix.AsMatrix ();
    F /= F.FrobeniusNorm();

    VNL::SymmetricEigensystem<double>  symm_eig (F + F.Transpose());

    double eig0 = symm_eig.D (0,0);
    double eig1 = symm_eig.D (2,2);
     
    if (eig0 > 0 || eig1 < 0) {
      cerr << "ERROR in VW::TransformLeastSquaresFunction<VW::FMatrixPlanar,VW::Point2D>";
      cerr << "::GenerateParameterVector:\n   SymmetricEigensystem<double>  is unsorted: ";
      cerr << symm_eig.D << endl;
      abort();
    }
     
    if (fabs(symm_eig.D(1,1)) > 1e-12) {
      cerr << "VW::TransformLeastSquaresFunction<VW::FMatrixPlanar,VW::Point2D>: WARNING:\n";
      cerr << "middle eigenvalue not 0: " << symm_eig.D << endl;
    }

    VNL::Vector<double> v0 (symm_eig.GetEigenvector(0));
    VNL::Vector<double> v1 (symm_eig.GetEigenvector(2));

    VNL::VectorFixed<3,double> f1 = sqrt(eig1)*v1 + sqrt(-eig0)*v0;
    VNL::VectorFixed<3,double> f2 = sqrt(eig1)*v1 - sqrt(-eig0)*v0;
     
    // Although it is not important for calculating a good F, it may be that the user
    // wishes the horizon line lh, and the image of the screw axis ls, to be correctly
    // sorted.  Do so by finding the epipolar line associated with the intersection of
    // f1 and f2, which will be algebraically equal to lh.  Use to choose between f1/f2.
    VNL::VectorFixed<3,double> s = Cross3D (f1, f2);
    VNL::VectorFixed<3,double> lhn = F * s;
    lhn *= VNL::Sgn (lhn[2]) / lhn.Magnitude ();
    VNL::VectorFixed<3,double> f1n = f1 * VNL::Sgn (f1[2]) / f1.Magnitude ();
    VNL::VectorFixed<3,double> f2n = f2 * VNL::Sgn (f2[2]) / f2.Magnitude ();
    VNL::VectorFixed<3,double> lh, ls;
    if ((lhn - f1n).Magnitude () < (lhn - f2n).Magnitude ()) {
      lh = f1;
      ls = f2;
    }
    else {
      lh = f2;
      ls = f1;
    }
    lh.Normalize ();
    ls.Normalize ();

    VNL::MatrixFixed<3,3,double> Fa = F - F.Transpose ();
    VNL::SVD<double> svdFa (Fa);
    VNL::Vector<double> fa = svdFa.Nullvector ();
    if (VNL::Sgn (fa[2]) != VNL::Sgn (Fa(0,1))) fa *= -1;
    fa.Normalize ();

    VNL::Vector<double> params (9);
    params[0] = lh[0];
    params[1] = lh[1];
    params[2] = lh[2];
    params[3] = ls[0];
    params[4] = ls[1];
    params[5] = ls[2];
    params[6] = fa[0];
    params[7] = fa[1];
    params[8] = fa[2];

    return params;
  }

  VW::FMatrixPlanar ReconstructTransformVLJK(const VNL::Vector<double>& x)
  {
    assert (x.size () == 9);

    // F = lh ls' + ls lh' + [fa]x
    const double* v = x.DataBlock();
    VNL::Vector<double> lh (v, 3);
    VNL::Vector<double> ls (v+3, 3);
    lh.Normalize ();
    ls.Normalize ();
    VNL::CrossProductMatrix<double> Fa (v+6);

    VNL::Vector<double> fa (v+6, 3);
    fa.Normalize ();
    cerr << "lh = " << lh << "  ls = " << ls << "  fa = " << fa << endl;

    VNL::Matrix<double> fmat = OuterProduct (lh, ls) + OuterProduct (ls, lh) + Fa;

    return VW::FMatrixPlanar(fmat / fmat.FrobeniusNorm());
  }


  /** Parameterises F as fixed lines lh and ls, from the symmetric part of
   * F, and two two-vector [a1 b1]' and [a2 b2]' which define the position
   * of the epipoles e1 and e2 on lh (the horizon line).
   *
   * So F = [e2]x [ls]x [e1]x as per HZ, but
   *    e1 = a1 xs + b1 xp, and
   *    e2 = a2 xs + b2 xp, where  xs = lh x ls, and
   *                               xp = xs x lh
   * (ie xp is a point on lh perpendicular to xs, the intersection of lh and ls)
   * We have from this that a1|2 = xs' e1|2, b1|2 = xp' e1|2.
   */
  VNL::Vector<double> 
  GenerateParameterVectorHZJK(const VW::FMatrixPlanar& fmatrix)
  {
	 VNL::Matrix<double> F = fmatrix.AsMatrix ();
    F /= F.FrobeniusNorm();
	 
	 VNL::SymmetricEigensystem<double>  symm_eig (F + F.Transpose());
	 
    double eig0 = symm_eig.D (0,0);
    double eig1 = symm_eig.D (2,2);
     
    if (eig0 > 0 || eig1 < 0) {
      cerr << "ERROR in VW::TransformLeastSquaresFunction<VW::FMatrixPlanar,VW::Point2D>";
      cerr << "::GenerateParameterVector:\n   SymmetricEigensystem<double>  is unsorted: ";
      cerr << symm_eig.D << endl;
      abort();
    }
     
    if (fabs(symm_eig.D(1,1)) > 1e-12) {
      cerr << "VW::TransformLeastSquaresFunction<VW::FMatrixPlanar,VW::Point2D>: WARNING:\n";
      cerr << "middle eigenvalue not 0: " << symm_eig.D << endl;
    }

     VNL::Vector<double> v0 (symm_eig.GetEigenvector(0));
     VNL::Vector<double> v1 (symm_eig.GetEigenvector(2));

     VNL::VectorFixed<3,double> f1 = sqrt(eig1)*v1 + sqrt(-eig0)*v0;
     VNL::VectorFixed<3,double> f2 = sqrt(eig1)*v1 - sqrt(-eig0)*v0;
    f1.Normalize ();
    f2.Normalize ();

    // We have to distinguish between lh and ls.  Do so by finding the epipolar line
    // associated with the intersection of f1 and f2, which will be algebraically
    // equal to lh.  Use to choose between f1/f2
     VNL::VectorFixed<3,double> xs = Cross3D(f1, f2).Normalize ();
     VNL::VectorFixed<3,double> lhn = F * xs;
    lhn *= VNL::Sgn (lhn[2]) / lhn.Magnitude ();
     VNL::VectorFixed<3,double> f1n = f1 * VNL::Sgn (f1[2]) / f1.Magnitude ();
     VNL::VectorFixed<3,double> f2n = f2 * VNL::Sgn (f2[2]) / f2.Magnitude ();
     VNL::VectorFixed<3,double> lh, ls;
    if ((lhn - f1n).Magnitude () < (lhn - f2n).Magnitude ()) {
      lh = f1;
      ls = f2;
    }
    else {
      lh = f2;
      ls = f1;
    }

     VNL::VectorFixed<3,double> xp = Cross3D(xs, lh).Normalize ();

     VNL::SVD<double> svdF (F);
     VNL::SVD<double> svdFt (F.Transpose ());
     VNL::VectorFixed<3,double> e1 = svdF.Nullvector().Normalize ();
     VNL::VectorFixed<3,double> e2 = svdFt.Nullvector().Normalize ();

     VNL::VectorFixed<2,double> a1, a2;
    a1[0] = DotProduct (xs, e1);
    a1[1] = DotProduct (xp, e1);
    a2[0] = DotProduct (xs, e2);
    a2[1] = DotProduct (xp, e2);
    a1.Normalize ();
    a2.Normalize ();

     VNL::Vector<double> params (10);
    params[0] = lh[0];
    params[1] = lh[1];
    params[2] = lh[2];
    params[3] = ls[0];
    params[4] = ls[1];
    params[5] = ls[2];
    params[6] = a1[0];
    params[7] = a1[1];
    params[8] = a2[0];
    params[9] = a2[1];

    return params;
  }

VW::FMatrixPlanar ReconstructTransformHZJK(const VNL::Vector<double>& x)
{
  assert (x.size () == 10);

  const double* v = x.DataBlock();
  VNL::VectorFixed<3,double> lh (v[0],v[1],v[2]);
    VNL::VectorFixed<3,double> ls (v[3],v[4],v[5]);
    VNL::VectorFixed<2,double> a1 (v[6],v[7]);
    VNL::VectorFixed<2,double> a2 (v[8],v[9]);
    lh.Normalize ();
    ls.Normalize ();
    a1.Normalize ();
    a2.Normalize ();

    //    cerr << "lh = " << lh << "  ls = " << ls << "  a1 = " << a1 << "  a2 = " << a2 << endl;

    VNL::VectorFixed<3,double> xs = Cross3D(lh, ls);xs=xs.Normalize ();
    VNL::VectorFixed<3,double> xp = Cross3D(xs, lh);xp=xp.Normalize ();
    VNL::VectorFixed<3,double> e1 = (a1[0]*xs + a1[1]*xp);e1=e1.Normalize ();
    VNL::VectorFixed<3,double> e2 = (a2[0]*xs + a2[1]*xp);e2=e2.Normalize ();

    // F = [e2]x [ls]x [e1]x
    VNL::CrossProductMatrix<double> L (ls);
    VNL::CrossProductMatrix<double> E1 (e1);
    VNL::CrossProductMatrix<double> E2 (e2);
     
    VNL::Matrix<double> fmat = E2 * L * E1;

    return FMatrixPlanar(fmat / fmat.FrobeniusNorm());

}

  ///////////////////////////////////////////////////////////////////

  /** A small class that knows how to be minimised. */
  class LeastSquaresFunctionPoints : public VNL::LeastSquaresFunction {
  public:
  
    LeastSquaresFunctionPoints(const std::vector<VW::Point2D>& features0,
			       const std::vector<VW::Point2D>& features1,
			       const std::vector<unsigned int>& whichfeatures);
    void F(const VNL::Vector<double>& x, VNL::Vector<double>& fx);
    
    
  protected:
    //  Store pointers to data for later access
    const std::vector<VW::Point2D> &_features0, &_features1;
    const std::vector<unsigned int> &_whichfeatures;
    
  }; // End of minimisation class
  
  ///////////////////////////////////////////////////////////////////


  LeastSquaresFunctionPoints::LeastSquaresFunctionPoints(const std::vector<VW::Point2D>& features0,
							 const std::vector<VW::Point2D>& features1,
							 const std::vector<unsigned int>& whichfeatures)
    :
#if defined(METHOD_HZ) || defined(METHOD_VLJK)
  VNL::LeastSquaresFunction (9, whichfeatures.size(), no_gradient),
#else // METHOD_HZJK
  VNL::LeastSquaresFunction (10, whichfeatures.size(), no_gradient),
#endif    
    _features0(features0),
    _features1(features1),
    _whichfeatures(whichfeatures)
  {}
  
  void LeastSquaresFunctionPoints::F(const VNL::Vector<double>& x, VNL::Vector<double>& fx)
  {
# if defined(METHOD_HZ)
    VW::FMatrixPlanar F = ReconstructTransformHZ(x);
 # elif defined(METHOD_VLJK)
    VW::FMatrixPlanar F = ReconstructTransformVLJK(x);
 # else
    VW::FMatrixPlanar F = ReconstructTransformHZJK(x);
 # endif
    // We use Sampson distance (HZ 10.4.3-p270) rather than symmetric epipolar
    // distance, because it apparently gives slightly better results.
    VNL::Vector<double>::iterator fx_it = fx.begin();
    vector<unsigned int>::const_iterator it = _whichfeatures.begin();
    vector<unsigned int>::const_iterator itend = _whichfeatures.end();
    for ( ; it != itend ; it++) {
     const VW::Point2D& p0 = _features0[*it];
      const VW::Point2D& p1 = _features1[*it];
      const VW::Line2D e0 = F.Transfer(p0);
      const VW::Line2D e1 = F.ReverseTransfer(p1);
      
      const double numerator = p1.GetX()*e0[0] + p1.GetY()*e0[1] + e0[2];
      const double denominator2 = e0[0]*e0[0] + e0[1]*e0[1] + e1[0]*e1[0] + e1[1]*e1[1];
		const double cost = numerator / sqrt(denominator2);
		*fx_it++ = cost;
    }
  }


 
} } // End namespaces



using namespace VW::FMPlanarCompute;





///////////////////////////////////////////////////////////////////


bool VW::FMPlanarComputeNonLinear::
Compute( VW::FMatrixPlanar& result,
			const std::vector<VW::Point2D>& x0,
			const std::vector<VW::Point2D>& x1)
{
  // Make a default list of every feature
  const unsigned int num_features = x0.size();
  std::vector<unsigned int> whichfeatures;
  whichfeatures.reserve(num_features);
  for (unsigned int i=0;i<num_features;i++) whichfeatures.push_back(i);

  Compute(result, x0, x1, whichfeatures);
}

bool VW::FMPlanarComputeNonLinear::
Compute( VW::FMatrixPlanar& result,
			const std::vector<VW::Point2D>& x0,
			const std::vector<VW::Point2D>& x1,
			const std::vector<unsigned int>& whichfeatures)
{
  
   LeastSquaresFunctionPoints lsfunc(x0,x1,whichfeatures);


  VNL::LevenbergMarquardt minimiser(lsfunc);
  minimiser.SetFTolerance (_tolerance);
  minimiser.SetMaxFunctionEvals (_max_iters * lsfunc.GetNumberOfUnknowns());
  
  // Form the parameter vector from the initial transform
# if defined(METHOD_HZ)
  VNL::Vector<double> x = GenerateParameterVectorHZ(result);
  bool _success = minimiser.Minimize (x);
  if (_success) result = ReconstructTransformHZ(x);

# elif defined(METHOD_VLJK)
  VNL::Vector<double> x = GenerateParameterVectorVLJK(result);
  bool _success = minimiser.Minimize (x);
  if (_success) result = ReconstructTransformVLJK(x);

# else
  VNL::Vector<double> x = GenerateParameterVectorHZJK(result);
  bool _success = minimiser.Minimize (x);
  if (_success) result = ReconstructTransformHZJK(x);

# endif

  return(_success);
 
}
 
