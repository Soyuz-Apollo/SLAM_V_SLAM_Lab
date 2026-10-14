// This is vxl/VNL/algo/amoeba.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
* \author Andrew W. Fitzgibbon, Oxford RRG
* \date   23 Oct 97
*/

#include "amoeba.h"

#include <cstdlib>
#include <cstdio> // for sprintf()
#include <iostream>
#include <vector>
#include <VNL/vnlmath.h>
#include <VNL/vector.h>
#include <VNL/costfunction.h>
#include <VNL/leastsquaresfunction.h>

using namespace VNL;


bool VNL::Amoeba::default_verbose = false;

VNL::Amoeba::Amoeba(VNL::CostFunction& f)
  : fptr(&f)
{
  verbose = default_verbose;
  maxiter = f.GetNumberOfUnknowns() * 200;
  X_tolerance = 1e-8;
  F_tolerance = 1e-4;
  relative_diameter = 0.05;
}

/** Define maximum number of iterations to use.
*/
void VNL::Amoeba::SetMaxIterations(int n)
{
  maxiter = n;
}

/** Define tolerance on elements of x.
*/
void VNL::Amoeba::SetXTolerance(double tol)
{
  X_tolerance = tol;
}

/** Define tolerance on function evaluation.
*/
void VNL::Amoeba::SetFTolerance(double tol)
{
  F_tolerance = tol;
}

/** Define scaling used to select starting vertices relative to initial x0.
*  I.e. the i'th vertex has x[i] = x0[i]*(1+relative_diameter)
*/
void VNL::Amoeba::SetRelativeDiameter(double r)
{
  relative_diameter = r;
}

namespace VNL {
struct AmoebaFit : public VNL::Amoeba {
  int cnt;

  AmoebaFit(VNL::Amoeba& a): VNL::Amoeba(a) {
    cnt = 0;
  }

/** Initialise the simplex given one corner, x (scale each element to get other corners).
*/
  void SetUpSimplexRelative(std::vector<AmoebaSimplexCorner>& simplex,
			    const VNL::Vector<double>& x);

/** Initialise the simplex given one corner, x and displacements of others.
*/
  void SetUpSimplexAbsolute(std::vector<AmoebaSimplexCorner>& simplex,
			    const VNL::Vector<double>& x,
			    const VNL::Vector<double>& dx);

/** Perform optimisation.\  Start simplex defined by scaling elements of x.
*/
  void DoAmoeba(VNL::Vector<double>& x);

/** Perform optimisation.\  Start simplex defined by adding dx[i] to each x[i].
*/
  void DoAmoeba(VNL::Vector<double>& x, const VNL::Vector<double>& dx);

/** Perform optimisation, given simplex to start.
*/
  void DoAmoeba(VNL::Vector<double>& x, std::vector<AmoebaSimplexCorner>& simplex);

  double F(const VNL::Vector<double>& x) {
    return fptr->F(x);
  }

  void SetCorner(AmoebaSimplexCorner * s,
		 const VNL::Vector<double>& v)
  {
    s->v = v;
    s->fv = F(v);
    cnt++;
  }
  void SetCornerAPlusBL(AmoebaSimplexCorner * s,
			const VNL::Vector<double>& vbar,
			const VNL::Vector<double>& v,
                            double lambda)
  {
    s->v = (1 - lambda) * vbar + lambda * v;
    s->fv = F(s->v);
    cnt++;
  }
};
}; // End namespace VNL


int AmoebaSimplexCorner::Compare(AmoebaSimplexCorner const& s1,
				 AmoebaSimplexCorner const& s2)
{
  return VNL::Sgn(s1.fv - s2.fv);
}

#ifdef VCL_SUNPRO_CC
extern "C"
#else
static
#endif
int CompareAux(const void * s1, const void * s2)
{
  return AmoebaSimplexCorner::Compare(*(const AmoebaSimplexCorner*)s1,
				      *(const AmoebaSimplexCorner*)s2);
}

static
void SortSimplex(std::vector<AmoebaSimplexCorner>& simplex)
{
  qsort(&simplex[0], simplex.size(), sizeof simplex[0], CompareAux);
}

static
double MaxAbsDiff(const VNL::Vector<double>& a, const VNL::Vector<double>& b)
{
  double v = 0;
  for (unsigned i = 0; i < a.size(); ++i) {
    double ad = VNL::Abs(a[i] - b[i]);
    if (ad > v)
      v = ad;
  }
  return v;
}

static
double SortedSimplexFDiameter(const std::vector<AmoebaSimplexCorner>& simplex)
{
  return simplex[simplex.size()-1].fv - simplex[0].fv;
}

#if 0
static
double SimplexFDiameter(const std::vector<AmoebaSimplexCorner>& simplex)
{
  // simplex assumed sorted, so fdiam is n - 0
  double max = 0;
  for (unsigned i = 1; i < simplex.size(); i++) {
    double thismax = VNL::Abs(simplex[0].fv - simplex[i].fv);
    if (thismax > max)
      max = thismax;
  }
  return max;
}
#endif

static
double SimplexDiameter(const std::vector<AmoebaSimplexCorner>& simplex)
{
  double max = 0;
  for (unsigned i = 0; i < simplex.size() - 1; i++) {
    double thismax = MaxAbsDiff(simplex[i].v, simplex[i+1].v);
    if (thismax > max)
      max = thismax;
  }
  return max;
}

namespace VNL {
std::ostream& operator<<(std::ostream& s, const AmoebaSimplexCorner& simplex)
{
  s << "S" << simplex.fv << " ";
  return s;
}

std::ostream& operator<<(std::ostream& s, const std::vector<AmoebaSimplexCorner>& simplex)
{
  for (unsigned i = 0; i < simplex.size(); ++i)
    s << simplex[i].fv << " ";
  return s;
}


bool operator==(const AmoebaSimplexCorner& a, const AmoebaSimplexCorner& b)
{
  return (&a) == (&b);
}
}; // End namespace VNL



/** Initialise the simplex given one corner, x.
*/
void AmoebaFit::SetUpSimplexRelative(std::vector<AmoebaSimplexCorner>& simplex,
				     const VNL::Vector<double>& x)
{
  int n = x.size();

  simplex[0].v = x;
  simplex[0].fv = F(x);

  // Following improvement suggested by L.Pfeffer at Stanford
  const double usual_delta = relative_diameter;             // 5 percent deltas for non-zero terms
  const double zero_term_delta = 0.00025;      // Even smaller delta for zero elements of x
//  VNL::Vector<double> y(n);
  for (int j = 0; j < n; ++j) {
    AmoebaSimplexCorner *s = &simplex[j+1];
    s->v = x;

    // perturb s->v(j)
    if (VNL::Abs(s->v[j]) > zero_term_delta)
      s->v[j] = (1 + usual_delta)*s->v[j];
    else
      s->v[j] = zero_term_delta;

    s->fv = F(s->v);
  }
}

/** Initialise the simplex given one corner, x and displacements of others.
*/
void AmoebaFit::SetUpSimplexAbsolute(std::vector<AmoebaSimplexCorner>& simplex,
				     const VNL::Vector<double>& x,
				     const VNL::Vector<double>& dx)
{
  int n = x.size();

  simplex[0].v = x;
  simplex[0].fv = F(x);

  for (int j = 0; j < n; ++j) {
    AmoebaSimplexCorner *s = &simplex[j+1];
    s->v = x;

    // perturb s->v(j)
    s->v[j] = s->v[j] + dx[j];

    s->fv = F(s->v);
  }
}

/** FMINS Minimize a function of several variables.
*  FMINS('F',X0) attempts to return a vector x which is a local minimizer
*  of F(x) near the starting vector X0.  'F' is a string containing the
*  name of the objective function to be minimized.  F(x) should be a
*  scalar valued function of a vector variable.
*
*  FMINS('F',X0,OPTIONS) uses a vector of control parameters.
*  If OPTIONS(1) is nonzero, intermediate steps in the solution are
*  displayed; the default is OPTIONS(1) = 0.  OPTIONS(2) is the termination
*  tolerance for x; the default is 1.e-4.  OPTIONS(3) is the termination
*  tolerance for F(x); the default is 1.e-4.  OPTIONS(14) is the maximum
*  number of steps; the default is OPTIONS(14) = 500.  The other components
*  of OPTIONS are not used as input control parameters by FMIN.  For more
*  information, see FOPTIONS.
*
*  FMINS('F',X0,OPTIONS,[],P1,P2,...) provides for up to 10 additional
*  arguments which are passed to the objective function, F(X,P1,P2,...)
*
*  FMINS uses a simplex search method.
*
*  See also FMIN.
*
*  Reference: J. E. Dennis, Jr. and D. J. Woods, New Computing
*  Environments: Microcomputers in Large-Scale Computing,
*  edited by A. Wouk, SIAM, 1987, pp. 116-122.
*/

void AmoebaFit::DoAmoeba(VNL::Vector<double>& x)
{
// Set up a simplex near the initial guess.
  int n = x.size();
  std::vector<AmoebaSimplexCorner> simplex(n+1, AmoebaSimplexCorner(n));

  SetUpSimplexRelative(simplex,x);
  DoAmoeba(x,simplex);
}

void AmoebaFit::DoAmoeba(VNL::Vector<double>& x, const VNL::Vector<double>& dx)
{
// Set up a simplex near the initial guess.
  int n = x.size();
  std::vector<AmoebaSimplexCorner> simplex(n+1, AmoebaSimplexCorner(n));

  SetUpSimplexAbsolute(simplex,x,dx);
  DoAmoeba(x,simplex);
}

/** Perform optimisation, given simplex to start.
*/
void AmoebaFit::DoAmoeba(VNL::Vector<double>& x,
		       std::vector<AmoebaSimplexCorner>& simplex)
{
  int n = x.size();
  SortSimplex(simplex);

  if (verbose > 1) {
    std::cerr << "initial\n" << simplex;
  } else if (verbose) {
    std::cerr << "initial: " << simplex << std::endl;
  }

  // Iterate until the diameter of the simplex is less than X_tolerance.
  AmoebaSimplexCorner reflect(n);
  AmoebaSimplexCorner expand(n);
  AmoebaSimplexCorner contract(n);
  AmoebaSimplexCorner shrink(n);
  AmoebaSimplexCorner *next;

  VNL::Vector<double> vbar(n);
  while (cnt < maxiter) {
    if (SimplexDiameter(simplex) < X_tolerance &&
        SortedSimplexFDiameter(simplex) < F_tolerance)
      break;

    // One step of the Nelder-Mead simplex algorithm
    for (int k =  0; k < n; ++k)  {
      vbar[k] = 0;
      for (int i = 0; i < n; ++i)
        vbar[k] += simplex[i].v[k];
      vbar[k] /= n;
    }

    SetCornerAPlusBL(&reflect, vbar, simplex[n].v, -1);

    next = &reflect;
    const char *how = "reflect ";
    if (reflect.fv < simplex[n-1].fv) {
      // Reflection not totally crap...
      if (reflect.fv < simplex[0].fv) {
        // Reflection actually the best, try expanding
        SetCornerAPlusBL(&expand, vbar, reflect.v, 2);

        if (expand.fv < simplex[0].fv) {
          next = &expand;
          how = "expand  ";
        }
      }
    } else {
      // Reflection *is* totally crap...
      {
        AmoebaSimplexCorner *tmp = &simplex[n];
        if (reflect.fv < tmp->fv)
          // replace simplex[n] by reflection as at least it's better than that
          tmp = &reflect;
        SetCornerAPlusBL(&contract, vbar, tmp->v, 0.5);
      }

      if (contract.fv < simplex[0].fv) {
        // The contraction point was really good, hold it there
        next = &contract;
        how = "contract";
      }
      else {
        // The contraction point was only average, shrink the entire simplex.
        for (int j = 1; j < n; ++j)
	  SetCornerAPlusBL(&simplex[j], simplex[0].v, simplex[j].v, 0.5);
        SetCornerAPlusBL(&shrink, simplex[0].v, simplex[n].v, 0.5);

        next = &shrink;
        how = "shrink  ";
      }
    }
    simplex[n] = *next;

    SortSimplex(simplex);

    // Print debugging info
    if (verbose) {
      char buf[16383];
      sprintf(buf, "iter %5d: %s ", cnt, how);
      std::cerr << buf;
      if (verbose ==2)
        std::cerr << "\nFirst corner: " << simplex[0].v;
      if (verbose > 1)
      {
        std::streamsize a = std::cerr.width(10);
        std::cerr << std::endl << simplex << std::endl;
        std::cerr.width(a);
      }
      else if (verbose)
        std::cerr << simplex << std::endl;
    }
  }
  num_evaluations_ = cnt;
  x = simplex[0].v;
}

/** Modify x to minimise function supplied in constructor.
*  Start simplex defined by scaling elements of x
*/
void VNL::Amoeba::Minimize(VNL::Vector<double>& x)
{
  AmoebaFit af(*this);
  af.DoAmoeba(x);
  num_evaluations_ = af.num_evaluations_;
}

/** Perform optimisation.\  Start simplex defined by adding dx[i] to each x[i].
*/
void VNL::Amoeba::Minimize(VNL::Vector<double>& x, const VNL::Vector<double>& dx)
{
  AmoebaFit af(*this);
  af.DoAmoeba(x,dx);
  num_evaluations_ = af.num_evaluations_;
}


/** Static method.
*/
void VNL::Amoeba::Minimize(VNL::CostFunction& f, VNL::Vector<double>& x)
{
  Minimize(f, x, 0);
}

/** Static method.
*/
void VNL::Amoeba::Minimize(VNL::CostFunction& f, VNL::Vector<double>& x, double delta)
{
  Amoeba a(f);
  a.verbose = VNL::Amoeba::default_verbose;
  if (delta != 0)
    a.relative_diameter = delta;
  AmoebaFit amoeba(a);
  amoeba.DoAmoeba(x);
}

/** Static method.
*/
void VNL::Amoeba::Minimize(VNL::CostFunction& f, 
			   VNL::Vector<double>& x,
			   const VNL::Vector<double>& dx)
{
  Amoeba a(f);
  a.verbose = VNL::Amoeba::default_verbose;
  AmoebaFit amoeba(a);
  amoeba.DoAmoeba(x,dx);
}

namespace VNL {
class AmoebaLSCF : public VNL::CostFunction {
  VNL::LeastSquaresFunction* ls_;
  VNL::Vector<double> fx;
public:

  AmoebaLSCF(VNL::LeastSquaresFunction& ls):
    VNL::CostFunction(ls.GetNumberOfUnknowns()),
    ls_(&ls),
    fx(ls.GetNumberOfResiduals())
  {
  }

  ~AmoebaLSCF() {}

  double F(VNL::Vector<double> const& x) {
    ls_->F(x, fx);
    return fx.SquaredMagnitude();
  }
};
}; // End namespace VNL



void VNL::Amoeba::Minimize(VNL::LeastSquaresFunction& f, VNL::Vector<double>& x)
{
  AmoebaLSCF lsf(f);
  Minimize(lsf, x);
}

/////////////////////////////////////////////////////////////////////////////
VNL::AmoebaSimplexCorner::AmoebaSimplexCorner(int n):
  v(n)
{
}

AmoebaSimplexCorner& VNL::AmoebaSimplexCorner::operator=(const AmoebaSimplexCorner& that)
{
  v = that.v;
  fv = that.fv;
  return *this;
}

//--------------------------------------------------------------------------------
