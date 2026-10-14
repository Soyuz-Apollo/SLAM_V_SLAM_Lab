
#include <cmath> // sqrt

#include "pdfbase1d.h"

VW::PDF::PDFBase1D::PDFBase1D(double mean, double variance)
  : _mu(mean),
    _sigma(std::sqrt(variance)),
    _sigma2(variance)
{
}
