/** 
 * @file   fmatrixcompute7point.h
 * @author JGH Knight
 * @date   02/08/2002		   		
 *
 *              (c) Copyright University of Oxford
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/**
 * @class    VW::FMatrixCompute::Linear7Point
 * @ingroup  GeometricAlgorithms
 * @author   JGH Knight
 * @date     02/11/02
 * @brief    FMatrixCompute7Point
 *
 * 7-point linear algorithm for FMatrices.  Returns 1 or 3 solutions.
 * Not part of the standard ComputeLinear<> implementation as it has to return
 * multiple solutions.
 */


#ifndef _fmatrix_compute7point_h_
#define _fmatrix_compute7point_h_

#include <vector>

namespace VW {

  class FMatrix;
  class Point2D;

  namespace FMatrixCompute {

   void Linear7Point(std::vector<FMatrix>& fmatrices,
		     const std::vector<VW::Point2D>& x0,
		     const std::vector<VW::Point2D>& x1,
		     const bool normalise = true,
		     const std::vector<unsigned int>& inlierlist = std::vector<unsigned int>());

  }; // End namespace FMatrixCompute
}; // End namespace VW		 

#endif // #ifndef _fmatrix_compute7point_h_
