// This is vxl/VNL/algo/powell.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include <assert.h>
#include <iostream>

#include <VNL/matlabprint.h>
#include <VNL/vnlmath.h>

// Local includes
#include "powell.h"
#include "brent.h"

using namespace VNL;

namespace VNL {
class Powell1DFun : public CostFunction {
public:
  Powell* powell_;
  CostFunction* f_;
  unsigned int n_;
  Vector<double> x0_;
  Vector<double> dx_;
  Vector<double> tmpx_;
  Powell1DFun(int n, CostFunction* f, Powell* powell) :
    VNL::CostFunction(1),
    powell_(powell),
    f_(f),
    n_(n),
    x0_(n),
    dx_(n),
    tmpx_(n)
  {
  }

  void Init(VNL::Vector<double> const& x0, VNL::Vector<double> const& dx) {
    x0_ = x0;
    dx_ = dx;
    assert(x0.size() == n_);
    assert(dx.size() == n_);
  }

  double F(const VNL::Vector<double>& x) {
    Uninit(x[0], tmpx_);
    double e = f_->F(tmpx_);
    powell_->_PubReportEval(e);
    return e;
  }

  void Uninit(double lambda, VNL::Vector<double>& out) {
    for (unsigned int i = 0; i < n_; ++i)
      out[i] = x0_[i] + lambda * dx_[i];
  }
};
}; // End namespace VNL


VNL::Powell::Powell(VNL::CostFunction* functor):
  functor_(functor)
{
}

VNL::NonlinearMinimizer::ReturnCodes
VNL::Powell::Minimize(VNL::Vector<double>& p)
  //double p[], double **xi, int n
{
 // verbose_ = true;
  int n = p.size();
  VNL::Powell1DFun f1d(n, functor_, this);

  double linmin_ftol = 1e-4;

  VNL::Matrix<double> xi(n,n, VNL::MatrixIdentity);
  VNL::Vector<double> ptt(n);
  VNL::Vector<double> xit(n);
  double fret = functor_->F(p);
  _ReportEval(fret);
  VNL::Vector<double> pt = p;
  for (;;) {
    double fp = fret;
    int ibig=0;
    double del=0.0;

    for (int i=0;i<n;i++) {
      // xit = ith column of xi
      for (int j = 0; j < n; ++j)
        xit[j] = xi[j][i];
      double fptt = fret;

      // 1D minimization along xi
      f1d.Init(p, xit);
      VNL::Brent brent(&f1d);
      double ax = 0.0;
      double xx = 1.0;
      double bx;
      brent.BracketMinimum(&ax, &xx, &bx);
      fret = brent.MinimizeGivenBounds(bx, xx, ax, linmin_ftol, &xx);
      f1d.Uninit(xx, p);
      // Now p is minimizer along xi

      if (fabs(fptt-fret) > del) {
        del = fabs(fptt-fret);
        ibig = i;
      }
    }

    if (2.0*fabs(fp-fret) <= ftol*(fabs(fp)+fabs(fret))) {
      VNL::MatlabPrint(std::cerr, xi, "xi");
      return CONVERGED_FTOL;
    }

    if (num_iterations_ == unsigned(maxfev)) {
      return FAILED_TOO_MANY_ITERATIONS;
    }

    for (int j=0;j<n;++j) {
      ptt[j]=2.0*p[j]-pt[j];
      xit[j]=p[j]-pt[j];
      pt[j]=p[j];
    }

    double fptt = functor_->F(ptt);
    _ReportEval(fret);
    if (fptt < fp) {
      double t=2.0*(fp-2.0*fret+fptt)*VNL::Sqr(fp-fret-del)-del*VNL::Sqr(fp-fptt);
      if (t < 0.0) {
        f1d.Init(p, xit);
        VNL::Brent brent(&f1d);
        double ax = 0.0;
        double xx = 1.0;
        double bx;
        brent.BracketMinimum(&ax, &xx, &bx);
        fret = brent.MinimizeGivenBounds(bx, xx, ax, linmin_ftol, &xx);
        f1d.Uninit(xx, p);

        for (int j=0;j<n;j++) {
          xi[j][ibig]=xi[j][n-1];
          xi[j][n-1]=xit[j];
        }
      }
    }
    _ReportIter();
  }
}
