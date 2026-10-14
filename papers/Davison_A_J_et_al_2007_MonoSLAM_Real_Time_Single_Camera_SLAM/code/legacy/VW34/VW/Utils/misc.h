/*
 * @file   misc.h 
 * @author
 * @date   02/08/02
 *
 * (c) Copyright University of Oxford
 *     All rights reserved
 *
 */


#ifndef _misc_h_
#define _misc_h_

// There should be *NO VW INCLUDES* to avoid dependency problems

namespace VW {

/* The misc namespace - a namespace for miscalleaneous objects (no really).
 *
 * Random bits of handy code.
 */
namespace Misc {


  // Handy deletion template
  template <class T>
  void Clean (T* pointer) {
    delete pointer;
    pointer = 0;
  }
};

}; // End of namespace VW



  
#endif // #ifndef _misc_h_
