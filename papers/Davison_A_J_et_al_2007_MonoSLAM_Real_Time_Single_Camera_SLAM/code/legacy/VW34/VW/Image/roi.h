/**
 * @class   VW::ROI
 * @ingroup Image
 * @author  ID Reid, BJ Tordoff
 * @date    15/01/2003
 *
 *          (c) Copyright University of Oxford
 *                  All rights reserved
 *
 * @brief   Class to hold the bounds on the Region of Interest (ROI) of a 2D 
 *          array (image).
 *
 * Data is stored as the first row/column of interest, and one beyond the last, so
 * a loop that does for(x=roi.Left();x<roi.Right();x++) {} does the right thing.
 */

#ifndef _roi_h_
#define _roi_h_


namespace VW {

class ROI {

 public:

  /** Construct with zero for all coords. */
  ROI();

  /** Construct with user defined region. */
  ROI(int left, int top, int right, int bottom);

  /** Construct from an axisting region of interest. */
  ROI(const ROI &r);



  /** Set to an existing region of interest (eg when copying images). */
  void Set(const VW::ROI &r);

  /** Set to a user defined region. */
  void Set(int left, int top, int right, int bottom);

  /** Recover all four bounds of the region. */
  void Get(int &left, int &top, int &right, int &bottom) const;

  /** Recover all four bounds of the region (as unsigned). */
  void Get(unsigned int &left, unsigned int &top, 
	   unsigned int &right, unsigned int &bottom) const;



  /** Recover index of the leftmost column inside the region. */
  int           Left()   const;

  /** Recover index of the first column outside and to the right of the region.
   */
  int           Right()  const;

  /** Recover index of the topmost row inside the region. */
  int           Top()    const;

  /** Recover index of the first row outside and below the region. */
  int           Bottom() const;

  /** Recover the width of the region. */
  unsigned int  Width()  const;

  /** Recover the height of the region. */
  unsigned int  Height() const;

  void SetLeft(const int newvalue);
  void SetRight(const int newvalue);
  void SetTop(const int newvalue);
  void SetBottom(const int newvalue);



  /** Reduce the region by 1 in all directions. */
  void Shrink();

  /** Reduce the region by 'a' in all directions. */
  void Shrink(unsigned int a);

  /** Reduce the region by 'a' from both left and right. */
  void ShrinkWidth(unsigned int a);

  /** Reduce the region by 'a' from both top and bottom. */
  void ShrinkHeight(unsigned int a);

  /** Increase the region by 1 in all directions. */
  void Grow();

  /** Increase the region by 'a' in all directions. */
  void Grow(unsigned int a);

  /** Increase the region by 'a' from both left and right. */
  void GrowWidth(unsigned int a);

  /** Increase the region by 'a' from both top and bottom. */
  void GrowHeight(unsigned int a);


  /** Is the specified point contained in the ROI */
  bool IsContained(double x, double y) const;
  
 protected:
  int c0, r0;     // first column and row of ROI (region of interest)
  int cl, rl;     // last column and row of ROI (+1)

};

}; // end namespace VW










// INLINES
inline
VW::ROI::ROI() 
{
  c0=0; r0=0; 
  cl=0; rl=0; 
};

/**
 * @param left the first column inside the region
 * @param right the first column beyond the right edge of the region
 * @param top the first row inside the region
 * @param bottom the first row beyond the bottom of the region
 */
inline
VW::ROI::ROI(int left, int top, int right, int bottom) 
{
  c0=left;  r0=top; 
  cl=right; rl=bottom; 
};

/**
 * @param r an existing ROI whose position and size will be copied
 */
inline
VW::ROI::ROI(const VW::ROI &r) 
{ 
  c0=r.c0; r0=r.r0; 
  cl=r.cl; rl=r.rl; 
};

/**
 * @returns the index of the first column inside the region 
 */
inline int           
VW::ROI::Left()   const
{
  return c0;
}

/**
 * @returns the index of the first row to the right of the region 
 *          (ie. Left()<=x<Right())
 */
inline int           
VW::ROI::Right()  const
{
  return cl;
}

/**
 * @returns the index of the first row inside the region 
 */
inline int           
VW::ROI::Top()    const
{
  return r0;
}

/**
 * @returns the index of the first row below the region (ie. Top()<=y<Bottom())
 */
inline int           
VW::ROI::Bottom() const
{
  return rl;
}

/**
 * @returns the width of (number of columns inside) the region
 */
inline unsigned int  
VW::ROI::Width()  const
{
  return cl-c0; 
};

/**
 * @returns the height of (number of rows inside) the region
 */
inline unsigned int  
VW::ROI::Height() const
{
  return rl-r0; 
}
 
inline void 
VW::ROI::SetLeft(const int newvalue)
{
  c0 = newvalue;
}

inline void 
VW::ROI::SetRight(const int newvalue)
{
  cl = newvalue;
}

inline void 
VW::ROI::SetTop(const int newvalue)
{
  r0 = newvalue;
}

inline void 
VW::ROI::SetBottom(const int newvalue)
{
  rl = newvalue;
}

/**
 * @param left the first column inside the region
 * @param right the first column beyond the right edge of the region
 * @param top the first row inside the region
 * @param bottom the first row beyond the bottom of the region
 */
inline void 
VW::ROI::Get(int &left, int &top, int &right, int &bottom) const 
{ 
  left =c0; top   =r0; 
  right=cl; bottom=rl; 
};

/**
 * @param left the first column inside the region
 * @param right the first column beyond the right edge of the region
 * @param top the first row inside the region
 * @param bottom the first row beyond the bottom of the region
 */
inline void 
VW::ROI::Get(unsigned int &left, unsigned int &top, 
	     unsigned int &right, unsigned int &bottom) const
{
  left =c0; top   =r0; 
  right=cl; bottom=rl; 
}


/**
 * @param r an existing ROI whose position and size will be copied
 */
inline void 
VW::ROI::Set(const VW::ROI &r) 
{ 
  c0=r.c0; r0=r.r0; 
  cl=r.cl; rl=r.rl; 
};

/**
 * @param left the first column inside the region
 * @param right the first column beyond the right edge of the region
 * @param top the first row inside the region
 * @param bottom the first row beyond the bottom of the region
 */
inline void 
VW::ROI::Set(int left, int top, int right, int bottom) 
{ 
  c0=left;  r0=top; 
  cl=right; rl=bottom; 
};

inline void 
VW::ROI::Shrink() 
{ 
  c0++; r0++; 
  cl--; rl--; 
}

/**
 * @param a the amount by which the region is increased
 */
inline void 
VW::ROI::Shrink(unsigned int a) 
{ 
  c0+=a; r0+=a; 
  cl-=a; rl-=a; 
}

inline void 
VW::ROI::ShrinkWidth(unsigned int a)
{
  c0+=a; cl-=a;
}

inline void 
VW::ROI::ShrinkHeight(unsigned int a)
{
  r0+=a; rl-=a;  
}


inline void 
VW::ROI::Grow() 
{ 
  c0--; r0--; 
  cl++; rl++; 
}

/**
 * @param a the amount by which the region is increased
 */
inline void 
VW::ROI::Grow(unsigned int a) 
{ 
  c0-=a; r0-=a; 
  cl+=a; rl+=a; 
}

inline void 
VW::ROI::GrowWidth(unsigned int a)
{
  c0-=a; cl+=a;
}

inline void 
VW::ROI::GrowHeight(unsigned int a)
{
  r0-=a; rl+=a;  
}

inline bool 
VW::ROI::IsContained(double x, double y) const
{
  const bool h = (x >= c0) && (x < cl);
  const bool v = (y >= r0) && (y < rl);
  return (h && v);
}

#endif
