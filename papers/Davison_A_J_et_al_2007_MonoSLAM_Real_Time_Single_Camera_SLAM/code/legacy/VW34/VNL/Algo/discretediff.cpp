#include "discretediff.h"
#include <VNL/leastsquaresfunction.h>
#include <assert.h>
/*
  fsm@robots.ox.ac.uk
*/

bool vnl_discrete_diff_fwd(VNL::LeastSquaresFunction *lsf,
                           double h_,
                           VNL::Vector<double> const &x,
                           VNL::Matrix<double>       &J)
{
  VNL::Vector<double> y(lsf->GetNumberOfResiduals());
  lsf->F(x,y);
  if (lsf->failure)
    return false;
  VNL::Vector<double> h(lsf->GetNumberOfUnknowns());
  h.Fill(h_);
  return vnl_discrete_diff_fwd(lsf,h,x,y,J);
}

bool vnl_discrete_diff_fwd(VNL::LeastSquaresFunction *lsf,
                           VNL::Vector<double> const &h,
                           VNL::Vector<double> const &x,
                           VNL::Matrix<double>       &J)
{
  VNL::Vector<double> y(lsf->GetNumberOfResiduals());
  lsf->F(x,y);
  if (lsf->failure)
    return false;
  return vnl_discrete_diff_fwd(lsf,h,x,y,J);
}

bool vnl_discrete_diff_fwd(VNL::LeastSquaresFunction *lsf,
                           VNL::Vector<double> const &h,
                           VNL::Vector<double> const &x,
                           VNL::Vector<double> const &y,
                           VNL::Matrix<double>       &J)
{
  unsigned m=J.Rows();
  unsigned n=J.Columns();
  assert((int)m==lsf->GetNumberOfResiduals());
  assert(m==y.size());
  assert((int)n==lsf->GetNumberOfUnknowns());
  assert(n==h.size());
  assert(n==x.size());

  VNL::Vector<double> tx(n);
  VNL::Vector<double> ty(m);

  for (unsigned j=0;j<n;j++) {
    tx=x; tx(j) += h(j);
    lsf->F(tx,ty);
    if (lsf->failure)
      return false;
    for (unsigned i=0;i<m;i++)
      J(i,j) = (ty(i)-y(i))/h(j);
  }
  return true;
}

bool vnl_discrete_diff_sym(VNL::LeastSquaresFunction *lsf,
                           double h_,
                           VNL::Vector<double> const &x,
                           VNL::Matrix<double>       &J)
{
  VNL::Vector<double> h(lsf->GetNumberOfUnknowns());
  h.Fill(h_);
  return vnl_discrete_diff_sym(lsf,h,x,J);
}

bool vnl_discrete_diff_sym(VNL::LeastSquaresFunction *lsf,
                           VNL::Vector<double> const &h,
                           VNL::Vector<double> const &x,
                           VNL::Matrix<double>       &J)
{
  unsigned m=J.Rows();
  unsigned n=J.Columns();
  assert((int)m==lsf->GetNumberOfResiduals());
  assert((int)n==lsf->GetNumberOfUnknowns());
  assert(n==h.size());
  assert(n==x.size());

  VNL::Vector<double> xp(n),xm(n);
  VNL::Vector<double> yp(m),ym(m);

  for (unsigned j=0;j<n;j++) {
    xp=x; xp(j) += h(j);
    lsf->F(xp,yp);
    if (lsf->failure)
      return false;

    xm=x; xm(j) -= h(j);
    lsf->F(xm,ym);
    if (lsf->failure)
      return false;

    for (unsigned i=0;i<m;i++)
      J(i,j) = (yp(i)-ym(i))/(2*h(j));
  }
  return true;
}

//----------------------------------------------------------------------

#include <iostream>

void vnl_discrete_diff_test_lsf(VNL::LeastSquaresFunction *lsf, VNL::Vector<double> const &x)
{
  unsigned int m = lsf->GetNumberOfResiduals();
  unsigned int n = lsf->GetNumberOfUnknowns ();
  assert(x.size() == n);

  VNL::Matrix<double> J1(m, n);
  lsf->GradF(x, J1);

  VNL::Matrix<double> J2(m, n);
  vnl_discrete_diff_sym(lsf, 0.0001, x, J2);

  double e = (J1 - J2).FrobeniusNorm();
  double t = VNL::CosAngle(J1, J2);

  std::cerr << __FILE__ ": e = " << e << std::endl;
  std::cerr << __FILE__ ": t = " << t << std::endl;

  //assert(e <= 1e-3);
  //assert(t >= 0.99);
}
