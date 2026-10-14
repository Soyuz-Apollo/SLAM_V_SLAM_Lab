// This is vxl/VNL/math.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
/**
* \file
*/

#include <VNL/vnlmath.h>


#if defined(_WIN32)
// I don't think we need this, because <ieeefp.h> is available -- fsm
# include <Float.h> // for 'isnan' and 'finite'
// # define isnan _isnan
# define finite _finite
# define finitef _finite
# define isnan _isnan
#elif VXL_IEEEFP_HAS_FINITE
# include <ieeefp.h>
# ifndef finitef
#  define finitef finite
# endif

#elif 1
# include <math.h>  // dont_vxl_filter: this is *not* supposed to be <cmath>
# ifndef __alpha__ // on Alpha, finitef() must be used for float args instead of finite()
#  define finitef finite
# endif

#elif defined(SYSV) && !defined(hppa)
// needed on platforms with finite() declared in strange places, e.g. on alpha
extern "C" int finite(double);
# ifdef __alpha__ // on Alpha, finitef() must be used for float args instead of finite()
extern "C" int finitef(float);
# else
#  define finitef finite
# endif

#else
#warning finite() is not declared on this platform
#define VNL_HAS_NO_FINITE
#endif

#ifdef VCL_SUNPRO_CC_50
# include <math.h> // dont_vxl_filter: no HUGE_VAL or isnan() in <cmath>
#endif

//--------------------------------------------------------------------------------

// constants
//AJD: to work with gcc 4, use WIN32 behaviour here and no inline assignment
//#ifdef _WIN32
#if 1
const double VNL::Math::e              = 2.7182818284590452354;
const double VNL::Math::log2e          = 1.4426950408889634074;
const double VNL::Math::log10e         = 0.43429448190325182765;
const double VNL::Math::ln2            = 0.69314718055994530942;
const double VNL::Math::ln10           = 2.30258509299404568402;
const double VNL::Math::pi             = 3.14159265358979323846;
const double VNL::Math::pi_over_2      = 1.57079632679489661923;
const double VNL::Math::pi_over_4      = 0.78539816339744830962;
const double VNL::Math::one_over_pi    = 0.31830988618379067154;
const double VNL::Math::two_over_pi    = 0.63661977236758134308;
const double VNL::Math::two_over_sqrtpi= 1.12837916709551257390;
const double VNL::Math::sqrt2          = 1.41421356237309504880;
const double VNL::Math::sqrt1_2        = 0.70710678118654752440;
#endif

// IEEE double machine precision
const double VNL::Math::eps            = 2.2204460492503131e-16;
const double VNL::Math::sqrteps        = 1.490116119384766e-08;
//#endif

const int      VNL::Math::maxint       = 0x7fffffff;
const long int VNL::Math::maxlong      = 0x7fffffffL;
const double   VNL::Math::maxdouble    = 1.7976931348623157E+308; // not HUGE_VAL since that is inf
const float    VNL::Math::maxfloat     = 3.40282346638528860e+38F;

//--------------------------------------------------------------------------------
#if !defined(VNL_HAS_NO_FINITE) && !defined(VCL_SGI_CC_7) && !defined(__alpha__) && !defined(_WIN32)
/** Return true iff x is "Not a Number".
*/
bool VNL::IsNaN(float x) { return x != x; } // causes "floating exception" on alpha & sgi
/** Return true iff x is "Not a Number".
*/
bool VNL::IsNaN(double x) { return x != x; }
/** Return true iff x is "Not a Number".
*/
bool VNL::IsNaN(long double x) { return x != x; }
#else
// Auxiliary function to simplify notation
# ifndef DEBUG
static inline unsigned int bMp(void*x,unsigned int y,int p=0){return ((((unsigned int*)x)[p])&y);}
static inline bool bMe(void*x,unsigned int y,int p=0){return ((((unsigned int*)x)[p])&y)==y;}
# else
# include <iostream>
static inline unsigned int bMp(void* x, unsigned int y, int p=0) {
  unsigned char* v=(unsigned char*)x;
  std::cout<<int(v[4*p])<<' '<<int(v[4*p+1])<<' '<<int(v[4*p+2])<<' '<<int(v[4*p+3])<<" & ";
  v=(unsigned char*)(&y);
  std::cout<<int(v[0])<<' '<<int(v[1])<<' '<<int(v[2])<<' '<<int(v[3])<<" = ";
  unsigned int z = ((((unsigned int*)x)[p]) & y);
  v=(unsigned char*)(&z);
  std::cout<<int(v[0])<<' '<<int(v[1])<<' '<<int(v[2])<<' '<<int(v[3]);
  if (z == y) std::cout<<" ==";
  std::cout << '\n';
  return z;
}
static inline bool bMe(void* x, unsigned int y, int p=0) { return bMp(x,y,p) == y; }
# endif
# if VXL_BIG_ENDIAN
static const int sz_f = 0;
static const int sz_d = 0;
static const int sz_l = 0;
# else
static const int sz_f = sizeof(float)/sizeof(int) -1;
static const int sz_d = sizeof(double)/sizeof(int) -1;
static const int sz_l = sizeof(long double)/sizeof(int) -1;
# endif
// Assume IEEE floating point number representation
bool VNL::IsNaN( float x){return bMe(&x,0x7f800000L,sz_f)&&bMp(&x,0x007fffffL,sz_f);}
bool VNL::IsNaN(double x){return bMe(&x,0x7ff00000L,sz_d)&&bMp(&x,0x000fffffL,sz_d);}
bool VNL::IsNaN(long double x) {
  if (sizeof(long double) == 8) return bMe(&x,0x7ff00000L,sz_l) && bMp(&x,0x000fffffL,sz_l);
  else if (sizeof(long double) <= 12) return bMe(&x,0x4001ffffL,sz_l) && bMp(&x,0x40000000,sz_l-4);
  else return bMe(&x,0x7ff70000L,sz_l) && bMp(&x,0x0008ffffL,sz_l);
}
#endif

// fsm@robots.ox.ac.uk
// On linux noshared builds, with optimisation on, calling 'finite' within the
// scope of vnl_math causes vnl_math_isinf to be called. This blows the stack.
// Plausible theory : 'finite' is a preprocessor macro, defined in terms of a
// macro called 'isinf'.
#if defined(isinf)
# if defined(__GNUC__) || defined(VCL_METRO_WERKS)
// I dont know if MW accepts #warning. Comment out the #undef if not.
#  warning macro isinf is defined
#  undef isinf
# else
// do not fail silently
#  error macro isinf is defined
# endif
#endif

#if !defined(VNL_HAS_NO_FINITE)
/** Return true if x is neither NaN nor Inf.
*/
bool VNL::IsFinite(float x) { return finitef(x) != 0; }
/** Return true if x is neither NaN nor Inf.
*/
bool VNL::IsFinite(double x) { return finite(x) != 0; }
/** Return true if x is neither NaN nor Inf.
*/
bool VNL::IsFinite(long double x) { return finite(x) != 0; }
#else
// Assume IEEE floating point number representation
bool VNL::IsFinite(float x) { return !bMe(&x,0x7f800000L,sz_f) && bMp(&x,0x7fffffffL,sz_f) != 0x7f7fffffL; }
bool VNL::IsFinite(double x) { return !bMe(&x,0x7ff00000L,sz_d); }
bool VNL::IsFinite(long double x) {
  if (sizeof(long double) == 8) return !bMe(&x,0x7ff00000L,sz_l);
  else if (sizeof(long double) <= 12) return !bMe(&x,0xbfff7fffL,sz_l) && !bMe(&x,0x4001ffffL,sz_l);
  else return !bMe(&x,0x7ff70000L,sz_l);
}
#endif


#if !defined(VNL_HAS_NO_FINITE)
/** Return true if x is inf.
*/
bool VNL::IsInf(float x) { return !finitef(x) && !isnan(x); }
/** Return true if x is inf.
*/
bool VNL::IsInf(double x) { return !finite(x) && !isnan(x); }
/** Return true if x is inf.
*/
bool VNL::IsInf(long double x) { return !finite(x) && !isnan(x); }
#else
// Assume IEEE floating point number representation
bool VNL::IsInf(float x) {return(bMe(&x,0x7f800000L,sz_f)&&!bMp(&x,0x007fffffL,sz_f))||bMp(&x,0x7fffffffL,sz_f)==0x7f7fffffL;}
bool VNL::IsInf(double x) { return bMe(&x,0x7ff00000L,sz_d) && !bMp(&x,0x000fffffL,sz_d); }
bool VNL::IsInf(long double x) {
  if (sizeof(long double) == 8) return bMe(&x,0x7ff00000L,sz_l) && !bMp(&x,0x000fffffL,sz_l);
  else if (sizeof(long double) <= 12) return (bMe(&x,0xbfff7fffL,sz_l)||bMe(&x,0x4001ffffL,sz_l))&&!bMp(&x,0x40000000,sz_l-4);
  else return bMe(&x,0x7ff70000L,sz_l) && !bMp(&x,0x0008ffffL,sz_l);
}
#endif

//----------------------------------------------------------------------

/** Type-accessible infinities for use in templates.
*/
template <class T> T VNL::HugeVal(T);
double VNL::HugeVal(double) { return HUGE_VAL; }
float  VNL::HugeVal(float)  { return (float)HUGE_VAL; }
#ifdef _INT_64BIT_
long int VNL::HugeVal(long int) { return 0x7fffffffffffffffL; }
int    VNL::HugeVal(int)    { return 0x7fffffffffffffffL; }
#else
int    VNL::HugeVal(int)    { return 0x7fffffff; }
#endif
short  VNL::HugeVal(short)  { return 0x7fff; }
char   VNL::HugeVal(char)   { return 0x7f; }

//----------------------------------------------------------------------
