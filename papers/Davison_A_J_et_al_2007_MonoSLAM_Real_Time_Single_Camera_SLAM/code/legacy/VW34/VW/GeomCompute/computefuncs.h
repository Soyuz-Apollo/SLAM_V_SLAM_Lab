#ifndef _computefuncs_h_
#define _computefuncs_h_

#include <vector>
#include <assert.h>
#include <VNL/vector.h>
#include <VNL/matrix.h>

namespace VW {

  /** Functions that are common to the computation routines */
  namespace ComputeFuncs {


    /** Convert a transform class into a parameter vector. */
    template <class T> void
    ConvertTransformToParamVector(const T& transform,
				  VNL::Vector<double>& x)
    {
      VNL::Matrix<double> mat = transform;
      
      mat /= mat.FrobeniusNorm();
      
      unsigned n = mat.size ();
      double* t_array = new double [n];
      mat.CopyOut (t_array);
      x.CopyIn (t_array);
      delete[] t_array;
    }
    

    /** Convert a parameter vector into a transform class. */
    template <class T>
    void ConvertParamVectorToTransform(const VNL::Vector<double>& x,
				       T& transform)
    {
      VNL::Matrix<double> M = transform;
      assert (transform.size () == x.size ());
      
      unsigned int n = M.size ();
      double* t_array = new double [n];
      x.CopyOut (t_array);
      M.CopyIn (t_array);
      M /= M.FrobeniusNorm ();
      transform = M;
      delete[] t_array;
    }



  }  // End namespace ComputeFuncs
}; // End namespace VW

#endif
