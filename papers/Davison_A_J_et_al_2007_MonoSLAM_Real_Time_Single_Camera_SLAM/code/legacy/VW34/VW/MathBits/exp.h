#ifndef _exp_h_
#define _exp_h_


namespace VW {

  /** An approximation to exp using the 1,1 Pade approximation, valid
	* to within 10% up to x=1.0 (3 flops).
	*/
  double PadeExp11(const double x);

  /** An approximation to exp using the 1,1 Pade approximation, valid
	* to within 10% up to x=2.3 (7 flops).
	*/
  double PadeExp22(const double x);

  /** An approximation to exp using the 3,3 Pade approximation, valid
	* to within 10% up to x=3.45 (11 flops).
	*/
  double PadeExp33(const double x);

  /** An approximation to exp using the 4,4 Pade approximation, valid
	* to within 10% up to x=4.85 (15 flops).
	*/
  double PadeExp44(const double x);




};



inline double VW::PadeExp11(const double x)
{
  const double p = 2.0 + x;
  const double q = 2.0 - x;
  return p / q;
}

inline double VW::PadeExp22(const double x)
{
  const double p = x*(x + 6.0) + 12.0;
  const double q = x*(x - 6.0) + 12.0;
  return p / q;
}

inline double VW::PadeExp33(const double x)
{
  const double p = x*(x*(x + 12.0) + 60.0) + 120.0;
  const double q = -x*(x*(x - 12.0) + 60.0) + 120.0;
  return p / q;
}

inline double VW::PadeExp44(const double x)
{
  const double p = x*(x*(x*(x + 20.0) + 180.0) + 840.0) + 1680;
  const double q = x*(x*(x*(x - 20.0) + 180.0) - 840.0) + 1680;
  return p / q;
}



#endif
