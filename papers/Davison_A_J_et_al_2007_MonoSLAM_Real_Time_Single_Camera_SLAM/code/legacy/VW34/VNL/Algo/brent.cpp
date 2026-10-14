// This is vxl/VNL/algo/brent.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include <cmath>
#include <iostream>
#include <algorithm>

#include <VNL/vnlmath.h>
#include <VNL/vectorfixed.h>

#include "brent.h"


namespace VNL {
struct BrentData {
  VNL::CostFunction* functor;
  VNL::VectorFixed<1,double> vx;
  double tol;

  double F(double x) {
    vx[0] = x;
    return functor->F(vx);
  }
};
}; // End namespace VNL

VNL::Brent::Brent(VNL::CostFunction* functor)
{
  p = new VNL::BrentData;
  p->functor = functor;
  p->tol = 2.0e-4;
}

VNL::Brent::~Brent()
{
  delete p;
}

static const int ITMAX = 100;
static const double CGOLD = 0.3819660;
static const double ZEPS = 1.0e-10;

static
void SHFT(double* a, double* b, double* c, double d) {
  *a = *b;
  *b = *c;
  *c = d;
}

double VNL::Brent::MinimizeGivenBounds(double ax, double bx, double cx,
                                       double tol,
                                       double *xmin)
{
  int iter;
  double a,b,d=0.0,etemp,fu,fv,fw,fx,p1,q,r,tol1,tol2,u,v,w,x,xm;
  double e=0.0;

  a=(ax < cx ? ax : cx);
  b=(ax > cx ? ax : cx);
  x=w=v=bx;
  fw=fv=fx=p->F(x);
  if (verbose_) std::cerr << "brent f("<<x<<") \t= "<<fx <<std::endl;
  for (iter=1;iter<=ITMAX;iter++) {
    xm=0.5*(a+b);
    tol1=tol*fabs(x)+ZEPS;
    tol2=2.0*(tol1);
    if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
      *xmin=x;
      return fx;
    }
    if (fabs(e) > tol1) {
      r=(x-w)*(fx-fv);
      q=(x-v)*(fx-fw);
      p1=(x-v)*q-(x-w)*r;
      q=2.0*(q-r);
      if (q > 0.0) p1 = -p1;
      q=fabs(q);
      etemp=e;
      e=d; // Warning: The variable d has not yet been assigned a value.
      if (fabs(p1) >= fabs(0.5*q*etemp) || p1 <= q*(a-x) || p1 >= q*(b-x))
        d=CGOLD*(e=(x >= xm ? a-x : b-x));
      else {
        d=p1/q;
        u=x+d;
        if (u-a < tol2 || b-u < tol2)
          d=tol1 * VNL::Sgn(xm-x);
      }
    } else {
      d=CGOLD*(e=(x >= xm ? a-x : b-x));
    }
    u=(fabs(d) >= tol1 ? x+d : x + tol1 * VNL::Sgn(d));
    fu= p->F(u);
    if (verbose_) std::cerr << "brent f("<<u<<") \t= "<<fu <<std::endl;
    if (fu <= fx) {
      if (u >= x) a=x; else b=x;
      SHFT(&v,&w,&x,u);
      SHFT(&fv,&fw,&fx,fu);
    } else {
      if (u < x) a=u; else b=u;
      if (fu <= fw || w == x) {
        v=w;
        w=u;
        fv=fw;
        fw=fu;
      } else if (fu <= fv || v == x || v == w) {
        v=u;
        fv=fu;
      }
    }
  }
  std::cerr << "Too many iterations in brent\n";
  *xmin=x;
  return fx;
}

double VNL::Brent::MinimizeGivenBoundsAnd1stF(double ax, double bx,
					      double fb, double cx,
					      double tol, double *xmin)

{
  int iter;
  double a,b,d=0.0,etemp,fu,fv,fw,fx,p1,q,r,tol1,tol2,u,v,w,x,xm;
  double e=0.0;

  a=(ax < cx ? ax : cx);
  b=(ax > cx ? ax : cx);
  x=w=v=bx;
  fw=fv=fx=fb;
  for (iter=1;iter<=ITMAX;iter++) {
    xm=0.5*(a+b);
    tol1=tol*fabs(x)+ZEPS;
    tol2=2.0*(tol1);
    if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
      *xmin=x;
      return fx;
    }
    if (fabs(e) > tol1) {
      r=(x-w)*(fx-fv);
      q=(x-v)*(fx-fw);
      p1=(x-v)*q-(x-w)*r;
      q=2.0*(q-r);
      if (q > 0.0) p1 = -p1;
      q=fabs(q);
      etemp=e;
      e=d; // Warning: The variable d has not yet been assigned a value.
      if (fabs(p1) >= fabs(0.5*q*etemp) || p1 <= q*(a-x) || p1 >= q*(b-x))
        d=CGOLD*(e=(x >= xm ? a-x : b-x));
      else {
        d=p1/q;
        u=x+d;
        if (u-a < tol2 || b-u < tol2)
          d=tol1 * VNL::Sgn(xm-x);
      }
    } else {
      d=CGOLD*(e=(x >= xm ? a-x : b-x));
    }
    u=(fabs(d) >= tol1 ? x+d : x + tol1 * VNL::Sgn(d));
    fu= p->F(u);
    if (verbose_) std::cerr << "brent f("<<u<<") \t= "<<fu <<std::endl;
    if (fu <= fx) {
      if (u >= x) a=x; else b=x;
      SHFT(&v,&w,&x,u);
      SHFT(&fv,&fw,&fx,fu);
    } else {
      if (u < x) a=u; else b=u;
      if (fu <= fw || w == x) {
        v=w;
        w=u;
        fv=fw;
        fw=fu;
      } else if (fu <= fv || v == x || v == w) {
        v=u;
        fv=fu;
      }
    }
  }
  std::cerr << "Too many iterations in brent\n";
  *xmin=x;
  return fx;
}


void VNL::Brent::BracketMinimum(double *ax, double *bx, double *cx)
{
  double fa, fb, fc;
  BracketMinimum(ax,bx,cx,&fa,&fb,&fc);
}

const double GOLD = 1.618034;
const double GLIMIT = 100.0;
const double TINY = 1.0e-20;

void VNL::Brent::BracketMinimum(double *ax, double *bx, double *cx,
                                double *fa, double *fb, double *fc)
{
  double ulim,u,r,q,fu;
  using namespace std;

  *fa=p->F(*ax);
  *fb=p->F(*bx);
  if (*fb > *fa) {
    std::swap(*ax, *bx);
    std::swap(*fa, *fb);
  }
  *cx=(*bx)+GOLD*(*bx-*ax);
  *fc=p->F(*cx);
  while (*fb > *fc) {
    r=(*bx-*ax)*(*fb-*fc);
    q=(*bx-*cx)*(*fb-*fa);
    double dq = q-r;
    if (abs(dq) < TINY) //removed std:: qualifier is quest for MSVC V6 happiness
      dq = VNL::Sgn(dq) * TINY;

    u=(*bx)-((*bx-*cx)*q-(*bx-*ax)*r)/(2.0*dq);
    ulim=(*bx)+GLIMIT*(*cx-*bx);
    if ((*bx-u)*(u-*cx) > 0.0) {
      fu=p->F(u);
      if (fu < *fc) {
        *ax=(*bx);
        *bx=u;
        *fa=(*fb);
        *fb=fu;
        return;
      } else if (fu > *fb) {
        *cx=u;
        *fc=fu;
        return;
      }
      u=(*cx)+GOLD*(*cx-*bx);
      fu=p->F(u);
    } else if ((*cx-u)*(u-ulim) > 0.0) {
      fu=p->F(u);
      if (fu < *fc) {
        //SHFT(bx,cx,&u,*cx+GOLD*(*cx-*bx)); awf dumped -- c is useless
        SHFT(bx,cx,&u,u+GOLD*(u-*cx));
        SHFT(fb,fc,&fu,p->F(u));
      }
    } else if ((u-ulim)*(ulim-*cx) >= 0.0) {
      u=ulim;
      fu=p->F(u);
    } else {
      u=(*cx)+GOLD*(*cx-*bx);
      fu=p->F(u);
    }
    SHFT(ax,bx,cx,u);
    SHFT(fa,fb,fc,fu);
  }
}


double VNL::Brent::Minimize(double x)
{
  double ax=x-1.0;
  double xx=x+1.0;
  double bx,fa,fx,fb;
  BracketMinimum(&ax,&xx,&bx,&fa,&fx,&fb);
  MinimizeGivenBounds(bx,xx,ax,ftol,&x);
  return x;
}
