/** 
 * @file   essentialmatrixcompute5point.cpp
 * @author G Vogiatzis, BJ Tordoff
 * @date   19/04/2005		   		
 *
 *              (c) Copyright University of Cambridge
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

#include <iostream>

#include <VNL/realpolynomial.h>
#include <VNL/Algo/qr.h>
#include <VNL/Algo/determinant.h>
#include <VNL/Algo/rpolyroots.h>

#include <VW/Utils/timer.h>

#include <VW/GeomObjects/fmatrix.h>
#include <VW/GeomObjects/normalisation.h>
#include <VW/GeomObjects/point2d.h>

#include <VW/MathBits/random.h>

#include "essentialmatrixcompute5point.h"
#include "essentialmatrixcompute.h" // General helper funcs

#define VERBOSE false

using namespace std;

  ///////////////////////////////////////////////////////////////////


using namespace VW::EssentialMatrixCompute;

typedef VNL::VectorFixed< 4,double> Double4;
typedef VNL::VectorFixed< 5,double> Double5;
typedef VNL::VectorFixed< 7,double> Double7;
typedef VNL::VectorFixed< 8,double> Double8;
typedef VNL::VectorFixed<10,double> Double10;
typedef VNL::VectorFixed<11,double> Double11;
typedef VNL::VectorFixed<20,double> Double20;


/** Convolve two fixed size vectors */
static inline Double7 Conv(const Double4& a, const Double4& b)
{
  Double7 result;
  result[0] = a[0]*b[0];
  result[1] = a[1]*b[0] + a[0]*b[1];
  result[2] = a[2]*b[0] + a[1]*b[1] + a[0]*b[2];
  result[3] = a[3]*b[0] + a[2]*b[1] + a[1]*b[2] + a[0]*b[3];
  result[4] =             a[3]*b[1] + a[2]*b[2] + a[1]*b[3];
  result[5] =                         a[3]*b[2] + a[2]*b[3];
  result[6] =                                     a[3]*b[3];
  return result;
}

/** Convolve two fixed size vectors */
static inline Double8 Conv(const Double5& a, const Double4& b)
{
  Double8 result;
  result[0] = a[0]*b[0];
  result[1] = a[1]*b[0] + a[0]*b[1];
  result[2] = a[2]*b[0] + a[1]*b[1] + a[0]*b[2];
  result[3] = a[3]*b[0] + a[2]*b[1] + a[1]*b[2] + a[0]*b[3];
  result[4] = a[4]*b[0] + a[3]*b[1] + a[2]*b[2] + a[1]*b[3];
  result[5] =             a[4]*b[1] + a[3]*b[2] + a[2]*b[3];
  result[6] =                         a[4]*b[2] + a[3]*b[3];
  result[7] =                                     a[4]*b[3];
  return result;
}

/** Convolve two fixed size vectors */
static inline Double8 Conv(const Double4& a, const Double5& b)
{
  Double8 result;
  result[0] = a[0]*b[0];
  result[1] = a[1]*b[0] + a[0]*b[1];
  result[2] = a[2]*b[0] + a[1]*b[1] + a[0]*b[2];
  result[3] = a[3]*b[0] + a[2]*b[1] + a[1]*b[2] + a[0]*b[3];
  result[4] =             a[3]*b[1] + a[2]*b[2] + a[1]*b[3] + a[0]*b[4];
  result[5] =                         a[3]*b[2] + a[2]*b[3] + a[1]*b[4];
  result[6] =                                     a[3]*b[3] + a[2]*b[4];
  result[7] =                                                 a[3]*b[4];
  return result;
}

/** Convolve two fixed size vectors */
static inline Double11 Conv(const Double7& a, const Double5& b)
{
  Double11 result;
  result[ 0] = a[0]*b[0];
  result[ 1] = a[1]*b[0] + a[0]*b[1];
  result[ 2] = a[2]*b[0] + a[1]*b[1] + a[0]*b[2];
  result[ 3] = a[3]*b[0] + a[2]*b[1] + a[1]*b[2] + a[0]*b[3];
  result[ 4] = a[4]*b[0] + a[3]*b[1] + a[2]*b[2] + a[1]*b[3] + a[0]*b[4];
  result[ 5] = a[5]*b[0] + a[4]*b[1] + a[3]*b[2] + a[2]*b[3] + a[1]*b[4];
  result[ 6] = a[6]*b[0] + a[5]*b[1] + a[4]*b[2] + a[3]*b[3] + a[2]*b[4];
  result[ 7] =             a[6]*b[1] + a[5]*b[2] + a[4]*b[3] + a[3]*b[4];
  result[ 8] =                         a[6]*b[2] + a[5]*b[3] + a[4]*b[4];
  result[ 9] =                                     a[6]*b[3] + a[5]*b[4];
  result[10] =                                                 a[6]*b[4];
  return result;
}

/** Convolve two fixed size vectors */
static inline Double11 Conv(const Double8& a, const Double4& b)
{
  Double11 result;
  result[ 0] = a[0]*b[0];
  result[ 1] = a[1]*b[0] + a[0]*b[1];
  result[ 2] = a[2]*b[0] + a[1]*b[1] + a[0]*b[2];
  result[ 3] = a[3]*b[0] + a[2]*b[1] + a[1]*b[2] + a[0]*b[3];
  result[ 4] = a[4]*b[0] + a[3]*b[1] + a[2]*b[2] + a[1]*b[3];
  result[ 5] = a[5]*b[0] + a[4]*b[1] + a[3]*b[2] + a[2]*b[3];
  result[ 6] = a[6]*b[0] + a[5]*b[1] + a[4]*b[2] + a[3]*b[3];
  result[ 7] = a[7]*b[0] + a[6]*b[1] + a[5]*b[2] + a[4]*b[3];
  result[ 8] =             a[7]*b[1] + a[6]*b[2] + a[5]*b[3];
  result[ 9] =                         a[7]*b[2] + a[6]*b[3];
  result[10] =                                     a[7]*b[3];
  return result;
}



static inline Double10 O1(const Double4& a, const Double4& b)
{
  Double10 r;
  r[0] = a[0]*b[0];
  r[1] = a[1]*b[1];
  r[2] = a[2]*b[2];
  r[3] = a[0]*b[1] + a[1]*b[0];
  r[4] = a[1]*b[2] + a[2]*b[1];
  r[5] = a[0]*b[2] + a[2]*b[0];
  r[6] = a[0]*b[3] + a[3]*b[0];
  r[7] = a[1]*b[3] + a[3]*b[1];
  r[8] = a[2]*b[3] + a[3]*b[2];
  r[9] = a[3]*b[3];
  return r;
}


static inline Double20 O2(const Double10& A, const Double4& b)
{
  Double20 r;
  r[ 0] = A[0]*b[0];
  r[ 1] = A[1]*b[1];
  r[ 2] = A[0]*b[1] + A[3]*b[0];
  r[ 3] = A[1]*b[0] + A[3]*b[1];
  r[ 4] = A[0]*b[2] + A[5]*b[0];
  r[ 5] = A[0]*b[3] + A[6]*b[0];
  r[ 6] = A[1]*b[2] + A[4]*b[1];
  r[ 7] = A[1]*b[3] + A[7]*b[1];
  r[ 8] = A[3]*b[2] + A[4]*b[0] + A[5]*b[1];
  r[ 9] = A[3]*b[3] + A[6]*b[1] + A[7]*b[0];
  r[10] = A[2]*b[0] + A[5]*b[2];
  r[11] = A[5]*b[3] + A[6]*b[2] + A[8]*b[0];
  r[12] = A[6]*b[3] + A[9]*b[0];
  r[13] = A[2]*b[1] + A[4]*b[2];
  r[14] = A[4]*b[3] + A[7]*b[2] + A[8]*b[1];
  r[15] = A[7]*b[3] + A[9]*b[1];
  r[16] = A[2]*b[2];
  r[17] = A[2]*b[3] + A[8]*b[2];
  r[18] = A[8]*b[3] + A[9]*b[2];
  r[19] = A[9]*b[3];
  return r;
}


/** Reduce a matrix to its row-echelon form (unit on pivots) */
void GetRowEchelonForm(VNL::MatrixFixed<10,20,double>& M)
{
  const unsigned int maxrow = M.Rows();
  const unsigned int maxcol = M.Columns();

  unsigned int row_index=0;
  for (unsigned int col_index=0;col_index<maxcol;col_index++) {

	 // Scan this column for largest value
	 unsigned int best_index = row_index;
	 double best_val = M[row_index][col_index];
	 for (unsigned int row2=row_index+1;row2<maxrow;row2++) {
		const double this_val = M[row2][col_index];
		if (abs(this_val)>abs(best_val)) {
		  best_val = this_val;
		  best_index = row2;
		}
	 }

	 if (abs(best_val) > 0.0) {
		const Double20 pivot_row = M.GetRow(best_index)/best_val;
		if (best_index > row_index) {
		  // Pivot is on a different row, so swap with this one
		  M.SetRow(best_index, M.GetRow(row_index));
		}
		{ // Copy in only the valued parts of the pivot row
		  const double* srcptr = &(pivot_row[col_index]);
		  double* dstptr = M[row_index] + col_index;
		  for (unsigned int col2=col_index;col2<maxcol;col2++) *dstptr++ = *srcptr++;
		}
		// Now scale each row (pivot row is divided by best_val)
		for (unsigned int row2=0;row2<row_index;row2++) {
		  double* dstptr = M[row2] + col_index;
		  const double scale = *dstptr;
		  *dstptr++ = 0.0;
		  const double* srcptr = &(pivot_row[col_index+1]);
		  for (unsigned int col2=col_index+1;col2<maxcol;col2++,srcptr++)
			 (*dstptr++) -= scale * (*srcptr);
		}
		for (unsigned int row2=row_index+1;row2<maxrow;row2++) {
		  double* dstptr = M[row2] + col_index;
		  const double scale = *dstptr;
		  *dstptr++ = 0.0;
		  const double* srcptr = &(pivot_row[col_index+1]);
		  for (unsigned int col2=col_index+1;col2<maxcol;col2++,srcptr++)
			 (*dstptr++) -= scale * (*srcptr);
		}
		row_index++;
		if (row_index==maxrow) break;
	 } else {
		// Already zeroed, so just move on to the next column
	 }
  }
}


static inline
void SolveForEs (const VNL::MatrixFixed<3,3,double>& E1,
					  const VNL::MatrixFixed<3,3,double>& E2,
					  const VNL::MatrixFixed<3,3,double>& E3,
					  const VNL::MatrixFixed<3,3,double>& E4,
					  vector<VW::EssentialMatrix>& ematrices)
{
  Double4 E[3][3];
  int index=0;
  for (unsigned int i=0;i<3;i++) {
	 for (unsigned int j=0;j<3;j++) {
		E[i][j][0] = E1[i][j];
		E[i][j][1] = E2[i][j];
		E[i][j][2] = E3[i][j];
		E[i][j][3] = E4[i][j];
	 }
  }

  const Double10 EEt11 = O1(E[0][0],E[0][0]) + O1(E[0][1],E[0][1]) + O1(E[0][2],E[0][2]);
  const Double10 EEt12 = O1(E[0][0],E[1][0]) + O1(E[0][1],E[1][1]) + O1(E[0][2],E[1][2]);
  const Double10 EEt13 = O1(E[0][0],E[2][0]) + O1(E[0][1],E[2][1]) + O1(E[0][2],E[2][2]);
  const Double10 EEt21 = O1(E[1][0],E[0][0]) + O1(E[1][1],E[0][1]) + O1(E[1][2],E[0][2]);
  const Double10 EEt22 = O1(E[1][0],E[1][0]) + O1(E[1][1],E[1][1]) + O1(E[1][2],E[1][2]);
  const Double10 EEt23 = O1(E[1][0],E[2][0]) + O1(E[1][1],E[2][1]) + O1(E[1][2],E[2][2]);
  const Double10 EEt31 = O1(E[2][0],E[0][0]) + O1(E[2][1],E[0][1]) + O1(E[2][2],E[0][2]);
  const Double10 EEt32 = O1(E[2][0],E[1][0]) + O1(E[2][1],E[1][1]) + O1(E[2][2],E[1][2]);
  const Double10 EEt33 = O1(E[2][0],E[2][0]) + O1(E[2][1],E[2][1]) + O1(E[2][2],E[2][2]);

  const Double10 q = 0.5 * (EEt11 + EEt22 + EEt33);
  const Double10  L11 = EEt11 - q;
  const Double10& L12 = EEt12;
  const Double10& L13 = EEt13;
  const Double10& L21 = EEt21;
  const Double10  L22 = EEt22 - q;
  const Double10& L23 = EEt23;
  const Double10& L31 = EEt31;
  const Double10& L32 = EEt32;
  const Double10  L33 = EEt33 - q;

  const Double20 R11 = O2(L11, E[0][0]) + O2(L12, E[1][0]) + O2(L13, E[2][0]);
  const Double20 R12 = O2(L11, E[0][1]) + O2(L12, E[1][1]) + O2(L13, E[2][1]);
  const Double20 R13 = O2(L11, E[0][2]) + O2(L12, E[1][2]) + O2(L13, E[2][2]);
  const Double20 R21 = O2(L21, E[0][0]) + O2(L22, E[1][0]) + O2(L23, E[2][0]);
  const Double20 R22 = O2(L21, E[0][1]) + O2(L22, E[1][1]) + O2(L23, E[2][1]);
  const Double20 R23 = O2(L21, E[0][2]) + O2(L22, E[1][2]) + O2(L23, E[2][2]);
  const Double20 R31 = O2(L31, E[0][0]) + O2(L32, E[1][0]) + O2(L33, E[2][0]);
  const Double20 R32 = O2(L31, E[0][1]) + O2(L32, E[1][1]) + O2(L33, E[2][1]);
  const Double20 R33 = O2(L31, E[0][2]) + O2(L32, E[1][2]) + O2(L33, E[2][2]);

  const Double20 detE = (  O2( O1(E[0][1],E[1][2]) - O1(E[0][2],E[1][1]),  E[2][0] ) 
								 + O2( O1(E[0][2],E[1][0]) - O1(E[0][0],E[1][2]),  E[2][1] ) 
 								 + O2( O1(E[0][0],E[1][1]) - O1(E[0][1],E[1][0]),  E[2][2] )  );

  VNL::MatrixFixed<10,20,double> S;
  S.SetRow(0, detE); 
  S.SetRow(1, R11);  S.SetRow(2, R12);  S.SetRow(3, R13); 
  S.SetRow(4, R21);  S.SetRow(5, R22);  S.SetRow(6, R23); 
  S.SetRow(7, R31);  S.SetRow(8, R32);  S.SetRow(9, R33);

  // Get in row-echelon form
  GetRowEchelonForm(S);

  VNL::MatrixFixed<3,4,double> Cxa, Cya, Cxb, Cyb, Cx, Cy;
  VNL::MatrixFixed<3,5,double> Ca, Cb, C;
  Cxa[0][0]=0.0; Cxa[0][1]=S[4][10]; Cxa[0][2]=S[4][11]; Cxa[0][3]=S[4][12];
  Cxa[1][0]=0.0; Cxa[1][1]=S[6][10]; Cxa[1][2]=S[6][11]; Cxa[1][3]=S[6][12];
  Cxa[2][0]=0.0; Cxa[2][1]=S[8][10]; Cxa[2][2]=S[8][11]; Cxa[2][3]=S[8][12];

  Cya[0][0]=0.0; Cya[0][1]=S[4][13]; Cya[0][2]=S[4][14]; Cya[0][3]=S[4][15];
  Cya[1][0]=0.0; Cya[1][1]=S[6][13]; Cya[1][2]=S[6][14]; Cya[1][3]=S[6][15];
  Cya[2][0]=0.0; Cya[2][1]=S[8][13]; Cya[2][2]=S[8][14]; Cya[2][3]=S[8][15];

  Ca[0][0]=0.0; Ca[0][1]=S[4][16]; Ca[0][2]=S[4][17]; Ca[0][3]=S[4][18]; Ca[0][4]=S[4][19];
  Ca[1][0]=0.0; Ca[1][1]=S[6][16]; Ca[1][2]=S[6][17]; Ca[1][3]=S[6][18]; Ca[1][4]=S[6][19];
  Ca[2][0]=0.0; Ca[2][1]=S[8][16]; Ca[2][2]=S[8][17]; Ca[2][3]=S[8][18]; Ca[2][4]=S[8][19];

  Cxb[0][0]=S[5][10]; Cxb[0][1]=S[5][11]; Cxb[0][2]=S[5][12]; Cxb[0][3]=0.0;
  Cxb[1][0]=S[7][10]; Cxb[1][1]=S[7][11]; Cxb[1][2]=S[7][12]; Cxb[1][3]=0.0;
  Cxb[2][0]=S[9][10]; Cxb[2][1]=S[9][11]; Cxb[2][2]=S[9][12]; Cxb[2][3]=0.0;
  
  Cyb[0][0]=S[5][13]; Cyb[0][1]=S[5][14]; Cyb[0][2]=S[5][15]; Cyb[0][3]=0.0;
  Cyb[1][0]=S[7][13]; Cyb[1][1]=S[7][14]; Cyb[1][2]=S[7][15]; Cyb[1][3]=0.0;
  Cyb[2][0]=S[9][13]; Cyb[2][1]=S[9][14]; Cyb[2][2]=S[9][15]; Cyb[2][3]=0.0;
  
  Cb[0][0]=S[5][16]; Cb[0][1]=S[5][17]; Cb[0][2]=S[5][18]; Cb[0][3]=S[5][19]; Cb[0][4]=0.0;
  Cb[1][0]=S[7][16]; Cb[1][1]=S[7][17]; Cb[1][2]=S[7][18]; Cb[1][3]=S[7][19]; Cb[1][4]=0.0;
  Cb[2][0]=S[9][16]; Cb[2][1]=S[9][17]; Cb[2][2]=S[9][18]; Cb[2][3]=S[9][19]; Cb[2][4]=0.0;

  Cx = Cxa-Cxb;
  Cy = Cya-Cyb;
  C  = Ca - Cb;

  const Double4 B11 = Cx.GetRow(0), B12 = Cy.GetRow(0);
  const Double4 B21 = Cx.GetRow(1), B22 = Cy.GetRow(1);
  const Double4 B31 = Cx.GetRow(2), B32 = Cy.GetRow(2);
  const Double5 B13 = C.GetRow(0), B23 = C.GetRow(1), B33 = C.GetRow(2);


  const Double8 p1 = Conv(B12, B23) - Conv(B13, B22);
  const Double8 p2 = Conv(B13, B21) - Conv(B11, B23);
  const Double7 p3 = Conv(B11, B22) - Conv(B12, B21);

  const Double11 N = Conv(p1, B31) + Conv(p2, B32) + Conv(p3, B33);

  const VNL::RPolyRoots roots(N);
  const VNL::RealPolynomial poly1(p1);
  const VNL::RealPolynomial poly2(p2);
  const VNL::RealPolynomial poly3(p3);

  const VNL::Vector<double> realroots = roots.RealRoots();
  
  ematrices.clear();
  for (unsigned int i=0;i<realroots.size();i++) {
	 const double& zk = realroots[i];
	 const double sc = 1.0 / poly3.Evaluate(zk);
	 const double xk = sc * poly1.Evaluate(zk); 
	 const double yk = sc * poly2.Evaluate(zk);

	 const VW::EssentialMatrix E(xk*E1 + yk*E2 + zk*E3 + E4);
	 ematrices.push_back(E);
  }  
}



/** Recover E's assuming the 5-point method. In this case there are
 * 4 null vectors.
 */
static inline
void RecoverEs5Point(VNL::QR<double>& qr,
		     vector<VW::EssentialMatrix>& ematrices)
{
  if (VERBOSE) std::cerr << "Recover (no normalisation)" << std::endl;
  //  VW::Timer t;
  const VNL::Matrix<double>& Q = qr.Q();

  // Get the 4D null-space of svd
  const VNL::MatrixFixed<3,3,double> e1 (Q.GetColumn(5).DataBlock());
  const VNL::MatrixFixed<3,3,double> e2 (Q.GetColumn(6).DataBlock());
  const VNL::MatrixFixed<3,3,double> e3 (Q.GetColumn(7).DataBlock());
  const VNL::MatrixFixed<3,3,double> e4 (Q.GetColumn(8).DataBlock());

  // Calculate and solve the cubic polynomial (fsm's stuff)
  ematrices.clear ();
  SolveForEs (e1, e2, e3, e4, ematrices);
  //  t.Stop();
  //  cerr << "Got " << ematrices.size() << " E's in " << t << endl;
}

static inline
void RecoverEs5Point(VNL::QR<double>& qr,
		     const VW::Normalisation& n0,
		     const VW::Normalisation& n1,
		     vector<VW::EssentialMatrix>& ematrices)
{
  if (VERBOSE) std::cerr << "Recover (normalised)" << std::endl;
  const VNL::Matrix<double>& Q = qr.Q();

  // Get the 4D null-space of svd
  const VNL::MatrixFixed<3,3,double> e1 (Q.GetColumn(5).DataBlock());
  const VNL::MatrixFixed<3,3,double> e2 (Q.GetColumn(6).DataBlock());
  const VNL::MatrixFixed<3,3,double> e3 (Q.GetColumn(7).DataBlock());
  const VNL::MatrixFixed<3,3,double> e4 (Q.GetColumn(8).DataBlock());

  // Calculate and solve the cubic polynomial (fsm's stuff)
  ematrices.clear ();
  SolveForEs (e1, e2, e3, e4, ematrices);

  for (unsigned int i=0;i<ematrices.size();i++) {
	 ematrices[i] = Unnormalise(ematrices[i],n0,n1);
  }
}



static inline void AddToDesignMatrix(VNL::MatrixFixed<9,5,double>& D,
												 const unsigned int index,
												 const VW::Point2D& p0, 
												 const VW::Point2D& p1)
{
  const double x1 = p0.GetX(), y1 = p0.GetY();
  const double x2 = p1.GetX(), y2 = p1.GetY();
  double* dstptr = D[0] + index;
  //x2.*x1 x2.*y1 x2.*e y2.*x1 y2.*y1 y2.*e x1 y1 e
  *dstptr = x1*x2; dstptr+=5;
  *dstptr = y1*x2; dstptr+=5;
  *dstptr =    x2; dstptr+=5;
  *dstptr = x1*y2; dstptr+=5;
  *dstptr = y1*y2; dstptr+=5;
  *dstptr =    y2; dstptr+=5;
  *dstptr =  x1; dstptr+=5;
  *dstptr =  y1; dstptr+=5;
  *dstptr = 1.0;
}



bool VW::EssentialMatrixCompute5Point::
Compute(std::vector<EssentialMatrix>& ematrices,
		  const std::vector<VW::Point2D>& x0,
		  const std::vector<VW::Point2D>& x1)
{
  // 4 steps:
  //   - Determine the optimal normalisation
  //   - Build the DLT matrix
  //   - Solve
  //   - Reconstruct E


  // * Step 1 - Build normalised list *

  // check sizes
  assert(x0.size() == x1.size());
  assert(x0.size() == 5);
  
  
  // For efficiency we calculate D'D
  VNL::MatrixFixed<9,5,double> D(0.0);
  
  
  // Code splits here, according to whether normalisation is used
  
  VW::Normalisation n0;
  VW::Normalisation n1;
  
  if (_normalise) {
    // Get the normalisation
    n0.Init(x0);
    n1.Init(x1);
	 
    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
    vector<VW::Point2D>::const_iterator it0 = x0.begin(), it1 = x1.begin();
    vector<VW::Point2D>::const_iterator it0_end = x0.end();
	 unsigned int index=0;
    for ( ; it0 != it0_end ; it0++, it1++, index++) {
      VW::Point2D norm_point0 = n0.Normalise(*it0);   
      VW::Point2D norm_point1 = n1.Normalise(*it1);   
      AddToDesignMatrix(D,index,norm_point0,norm_point1);
    }
  } else {
    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
    vector<VW::Point2D>::const_iterator it0 = x0.begin(), it1 = x1.begin();
    vector<VW::Point2D>::const_iterator it0_end = x0.end();
	 unsigned int index=0;
    for ( ; it0 != it0_end ; it0++, it1++, index++) {
      AddToDesignMatrix(D,index,*it0,*it1);
    }
  }


  // * Step 3 - Solve *
  if (VERBOSE) std::cerr << "Solve using symmetric eigendecomposition" << std::endl;
  VNL::QR<double> qr(D);
  
  // * Step 4 - Recover *
  if (VERBOSE) std::cerr << "Recover" << std::endl;
  if (_normalise)
    RecoverEs5Point(qr,n0,n1,ematrices);
  else
    RecoverEs5Point(qr,ematrices);

  if (ematrices.size()==0) return false;

  return true; 
}


bool VW::EssentialMatrixCompute5Point::
Compute(std::vector<EssentialMatrix>& ematrices,
		  const std::vector<VW::Point2D>& x0,
		  const std::vector<VW::Point2D>& x1,
		  const std::vector<unsigned int>& which_features)
{
  // 4 steps:
  //   - Determine the optimal normalisation
  //   - Build the DLT matrix
  //   - Solve
  //   - Reconstruct E


  // * Step 1 - Build normalised list *

  // check sizes
  assert(x0.size() == x1.size());
  assert(which_features.size() == 5);
  
  // For efficiency we calculate D'D
  VNL::MatrixFixed<9,5,double> D(0.0);
  
  
  // Code splits here, according to whether normalisation is used
  
  VW::Normalisation n0;
  VW::Normalisation n1;
  
  if (_normalise) {
    // Get the normalisation
    n0.Init(x0,which_features);
    n1.Init(x1,which_features);
	 
    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
      vector<unsigned int>::const_iterator it = which_features.begin();
      vector<unsigned int>::const_iterator it_end = which_features.end();
		unsigned int index=0;
      for ( ; it != it_end ; it++, index++) {
		  VW::Point2D norm_point0 = n0.Normalise(x0[*it]);   
		  VW::Point2D norm_point1 = n1.Normalise(x1[*it]);   
		  AddToDesignMatrix(D,index,norm_point0,norm_point1);
      }
  } else {
    // * Step 2 - Build DLT *
    // Loop through design matrix D filling with values
	 vector<unsigned int>::const_iterator it = which_features.begin();
	 vector<unsigned int>::const_iterator it_end = which_features.end();
	 unsigned int index=0;
	 for ( ; it != it_end ; it++, index++) {
		AddToDesignMatrix(D,index,x0[*it],x1[*it]);
	 }
  }


  // * Step 3 - Solve *
  if (VERBOSE) std::cerr << "Solve using symmetric eigendecomposition" << std::endl;
  VNL::QR<double> qr(D);
  
  // * Step 4 - Recover *
  if (VERBOSE) std::cerr << "Recover" << std::endl;
  if (_normalise)
    RecoverEs5Point(qr,n0,n1,ematrices);
  else
    RecoverEs5Point(qr,ematrices);

  if (ematrices.size()==0) return false;

  return true; 
}
