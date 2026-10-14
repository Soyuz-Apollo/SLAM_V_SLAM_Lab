#include "pdfbase2diso.h"

#include <VNL/diagmatrix.h>

VW::PDF::PDFBase2DIso::PDFBase2DIso(const VNL::VectorFixed<2,double>& mean, 
				    double variance)
  : PDFBase2D(mean,VNL::MatrixFixed<2,2,double>()),
    _sigma(std::sqrt(variance))
{
  // There ought to be a better way, but...
  _C[0][0] = variance; _C[0][1] = 0;
  _C[1][0] = 0;        _C[1][1] = variance;
}
