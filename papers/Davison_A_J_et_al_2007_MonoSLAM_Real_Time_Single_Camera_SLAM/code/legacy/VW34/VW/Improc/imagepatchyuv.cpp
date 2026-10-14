/** 
 * @file   imagepatchyuv.cpp
 * @author BJ Tordoff
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


#include <iostream>

#include <VW/MathBits/nan.h> // for isnan
#include "imagepatchyuv.h"


#define VERBOSE true

enum CovarMethod {
  FULL_COVAR,                // Full covariance matrix
  INDEPENDENT_INTENSITY,     // Independent intensity, full colour covar
  INDEPENDENT_ISO,           // Independent intensity, isotropic colour covar
  INDEPENDENT,               // All independent
  ISOTROPIC,                 // All independent and isotropic
  MONOCHROME                 // Y channel only
};

#define METHOD INDEPENDENT

using namespace std;
using namespace VW;

/*************************/
/* Some useful functions */
/*************************/


// {{{ GetInterpolatedImagePatch()

void VW::GetInterpolatedImagePatch(const ImageYUV<unsigned char>& im,
					 const double x,
					 const double y,
					 VW::ImagePatchYUV& patch) {

  // Check patch size
  if ((patch.GetWidth()==0) || (patch.GetHeight()==0)) {
    cerr << "VW::GetInterpolatedImagePatch: patch not initialised!"<<endl;
    return;
  }

  // Get half sizes
  unsigned int halfwidth=patch.GetWidth()/2;
  unsigned int halfheight=patch.GetHeight()/2;

  // Store integer positions and offsets
  int px=int(x);
  int py=int(y);
  const float deltax=float(x)-px;
  const float deltay=float(y)-py;

  // Coefficeints of interpolation are constant
  const float a00 = (1.0f-deltax)*(1.0f-deltay);
  const float a01 = (1.0f-deltax)*(     deltay);
  const float a10 = (     deltax)*(1.0f-deltay);
  const float a11 = (     deltax)*(     deltay);

  for (int iy=int(py-halfheight) ; iy<=int(py+halfheight) ; iy++) {
    for (int ix=int(px-halfwidth) ; ix<=int(px+halfwidth) ; ix++) {
      const VW::PixelYUV<float> this_value 
	= a00 * im.GetPixel(ix  ,iy  )
	+ a01 * im.GetPixel(ix  ,iy+1)
	+ a10 * im.GetPixel(ix+1,iy  )
	+ a11 * im.GetPixel(ix+1,iy+1);
      patch.SetData(ix-px+halfwidth,iy-py+halfheight,this_value);
    }
  }

}

// }}}

// {{{ GetImagePatch()


void VW::GetImagePatch(const ImageYUV<unsigned char>& im,
			     const double x,
			     const double y,
			     VW::ImagePatchYUV& patch) {

  // Check patch size
  if ((patch.GetWidth()==0) || (patch.GetHeight()==0)) {
    cerr << "VW::GetImagePatchYUV: patch not initialised!"<<endl;
    return;
  }

  // Get half sizes
  unsigned int halfwidth=patch.GetWidth()/2;
  unsigned int halfheight=patch.GetHeight()/2;

  // Store integer positions and offsets
  int px=int(x+0.5);
  int py=int(y+0.5);


  for (int iy=int(py-halfheight) ; iy<=int(py+halfheight) ; iy++) {
    for (int ix=int(px-halfwidth) ; ix<=int(px+halfwidth) ; ix++) {
      const VW::PixelYUV<float> p(im[iy][ix].y, im[iy][ix].u, im[iy][ix].v);
      patch.SetData(ix-px+halfwidth,iy-py+halfheight,p);
    }
  }
    
}

// }}}


void 
VW::ImagePatchYUV::CalcSums() const 
{
  const unsigned int N=data.size();
  my_mean = 0.0f;
  switch(METHOD) {
  default:
  case FULL_COVAR: {
    covar.SetIdentity();
    // Get upper triangle of covar
    for (unsigned int i=0;i<N;i++) {
      const float y = data[i].y;
      const float u = data[i].u-127.0f; // subtract centre value
      const float v = data[i].v-127.0f; // subtract centre value
      my_mean += data[i];
      covar[0][0] += y*y;
      covar[0][1] += y*u;
      covar[0][2] += y*v;
      covar[1][1] += u*u;
      covar[1][2] += u*v;
      covar[2][2] += v*v;
    }
    const float scale = 1.0f/float(N);
    my_mean *= scale;
    covar[0][0] *= scale;
    covar[0][1] *= scale;
    covar[0][2] *= scale;
    covar[1][1] *= scale;
    covar[1][2] *= scale;
    covar[2][2] *= scale;
    const float my = my_mean.y;
     const float mu = my_mean.u-127.0f; // subtract centre value
     const float mv = my_mean.v-127.0f; // subtract centre value
    covar[0][0] -= my*my; 
    covar[0][1] -= my*mu; 
    covar[0][2] -= my*mv; 
//     covar[1][1] -= mu*mu;
//     covar[1][2] -= mu*mv;
//     covar[2][2] -= mv*mv;
    
    // Force symmetry
    covar[1][0] = covar[0][1];
    covar[2][0] = covar[0][2];
    covar[2][1] = covar[1][2];
    // Now form root
    // [ a b c ]
    // [ 0 d e ]
    // [ 0 0 f ]
    const float a = sqrt(covar[0][0]);
    const float b = covar[0][1]/a;
    const float c = covar[0][2]/a;
    const float d = sqrt(covar[1][1] - b*b);
    const float e = (covar[1][2] - b*c)/d;
    const float f = sqrt(covar[2][2] - c*c - e*e);
    //    cerr << "Vars = " << a << ", " << b << ", " << c << ", " << d << ", " << e << ", " << f << endl;

    // And inverse
    const float ri_cov_yy = 1.0f/a;
    const float ri_cov_uu = 1.0f/d;
    const float ri_cov_vv = 1.0f/f;
    
    const float ri_cov_yu = -b*ri_cov_yy*ri_cov_uu;
    const float ri_cov_yv = (b*e-c*d)*ri_cov_yy*ri_cov_uu*ri_cov_vv;
    const float ri_cov_uv = -e*ri_cov_uu*ri_cov_vv;

    // Normalise data
    norm_data.resize(data.size());
    vector<VW::PixelYUV<float> >::const_iterator src = data.begin();
    vector<VW::PixelYUV<float> >::const_iterator src_end = data.end();
    vector<VW::PixelYUV<float> >::iterator dest = norm_data.begin();
    for ( ; src!=src_end ; src++, dest++) {
      const float y_bar = (src->y - my_mean.y);
//       const float u_bar = (src->u - my_mean.u);
//       const float v_bar = (src->v - my_mean.v);
      const float u_bar = (src->u - 127.0f);
      const float v_bar = (src->v - 127.0f);
      dest->y = y_bar*ri_cov_yy;
      dest->u = y_bar*ri_cov_yu + u_bar*ri_cov_uu;
      dest->v = y_bar*ri_cov_yv + u_bar*ri_cov_uv + v_bar*ri_cov_vv;
    }
    break;
  }
  case INDEPENDENT_INTENSITY: {
    covar.SetIdentity();
    // Get upper triangle of covar
    for (unsigned int i=0;i<N;i++) {
      const float y = data[i].y;
      const float u = data[i].u-127.0f; // subtract centre value
      const float v = data[i].v-127.0f; // subtract centre value
      my_mean += data[i];
      covar[0][0] += y*y;
      covar[1][1] += u*u;
      covar[1][2] += u*v;
      covar[2][2] += v*v;
    }
    const float scale = 1.0f/float(N);
    my_mean *= scale;
    covar[0][0] *= scale;
    covar[1][1] *= scale;
    covar[1][2] *= scale;
    covar[2][2] *= scale;
    const float my = my_mean.y;
//     const float mu = my_mean.u-127.0; // subtract centre value
//     const float mv = my_mean.v-127.0; // subtract centre value
    covar[0][0] -= my*my; 
//     covar[1][1] -= mu*mu;
//     covar[1][2] -= mu*mv;
//     covar[2][2] -= mv*mv;
    
    // Force symmetry
    covar[2][1] = covar[1][2];

    // Now form root inv covar and normalise data
    const float& a = covar[1][1];
    const float& b = covar[1][2];
    const float& c = covar[2][2];
    const float inv_root_det = 1.0f/sqrt(a*c - b*b);
    const float ri_cov_yy = 1.0f/sqrt(covar[0][0]);
    const float ri_cov_uu = 1.0f/sqrt(a);
    const float ri_cov_uv = -b*ri_cov_uu*inv_root_det;
    const float ri_cov_vv = inv_root_det/ri_cov_uu;
    
    norm_data.resize(data.size());
    vector<VW::PixelYUV<float> >::const_iterator src = data.begin();
    vector<VW::PixelYUV<float> >::const_iterator src_end = data.end();
    vector<VW::PixelYUV<float> >::iterator dest = norm_data.begin();
    for ( ; src!=src_end ; src++, dest++) {
      const float y_bar = (src->y - my_mean.y);
//       const float u_bar = (src->u - my_mean.u);
//       const float v_bar = (src->v - my_mean.v);
      const float u_bar = (src->u - 127.0f);
      const float v_bar = (src->v - 127.0f);
      dest->y = y_bar*ri_cov_yy;
      dest->u = u_bar*ri_cov_uu;
      dest->v = v_bar*ri_cov_vv + u_bar*ri_cov_uv;
    }
    break;
  }
  case INDEPENDENT_ISO: {
    covar.SetIdentity();
    // Get upper triangle of covar
    for (unsigned int i=0;i<N;i++) {
      const float y = data[i].y;
      const float u = data[i].u-127.0f; // subtract centre value
      const float v = data[i].v-127.0f; // subtract centre value
      my_mean += data[i];
      covar[0][0] += y*y;
      covar[1][1] += u*u + v*v;
   }
    const float scale = 1.0f/float(N);
    my_mean *= scale;
    covar[0][0] *= scale;
    covar[1][1] *= scale;
    const float my = my_mean.y;
 //    const float mu = my_mean.u-127.0; // subtract centre value
//     const float mv = my_mean.v-127.0; // subtract centre value
    covar[0][0] -= my*my; 
//     covar[1][1] -= (mu*mu+mv*mv);
    covar[1][1] /= 2;

    // Force symmetry
    covar[2][2] = covar[1][1];

    // Now form root inv covar and normalise data
    const float ri_cov_yy = 1.0f/sqrt(covar[0][0]);
    const float ri_cov_uu = 1.0f/sqrt(covar[1][1]);
    const float ri_cov_vv = ri_cov_uu;
    
    norm_data.resize(data.size());
    vector<VW::PixelYUV<float> >::const_iterator src = data.begin();
    vector<VW::PixelYUV<float> >::const_iterator src_end = data.end();
    vector<VW::PixelYUV<float> >::iterator dest = norm_data.begin();
    for ( ; src!=src_end ; src++, dest++) {
      const float y_bar = (src->y - my_mean.y);
//       const float u_bar = (src->u - my_mean.u);
//       const float v_bar = (src->v - my_mean.v);
      const float u_bar = (src->u - 127.0f);
      const float v_bar = (src->v - 127.0f);
      dest->y = y_bar*ri_cov_yy;
      dest->u = u_bar*ri_cov_uu;
      dest->v = v_bar*ri_cov_vv;
    }
    break;
  }
  case INDEPENDENT: {
    covar.SetIdentity();
    // Get upper triangle of covar
    for (unsigned int i=0;i<N;i++) {
      const float y = data[i].y;
      const float u = data[i].u-127.0f; // subtract centre value
      const float v = data[i].v-127.0f; // subtract centre value
      my_mean += data[i];
      covar[0][0] += y*y;
      covar[1][1] += u*u;
      covar[2][2] += v*v;
    }
    const float scale = 1.0f/float(N);
    my_mean *= scale;
    covar *= scale;
    const float my = my_mean.y;
//     const float mu = my_mean.u-127.0; // subtract centre value
//     const float mv = my_mean.v-127.0; // subtract centre value
    covar[0][0] -= my*my; 
//     covar[1][1] -= mu*mu;
//     covar[2][2] -= mv*mv;
    
    // Now form root inv covar and normalise data
    const float ri_cov_yy = 1.0f/sqrt(covar[0][0]);
    const float ri_cov_uu = 1.0f/sqrt(covar[1][1]);
    const float ri_cov_vv = 1.0f/sqrt(covar[2][2]);
    
    norm_data.resize(data.size());
    vector<VW::PixelYUV<float> >::const_iterator src = data.begin();
    vector<VW::PixelYUV<float> >::const_iterator src_end = data.end();
    vector<VW::PixelYUV<float> >::iterator dest = norm_data.begin();
    for ( ; src!=src_end ; src++, dest++) {
      const float y_bar = (src->y - my_mean.y);
//       const float u_bar = (src->u - my_mean.u);
//       const float v_bar = (src->v - my_mean.v);
      const float u_bar = (src->u - 127.0f);
      const float v_bar = (src->v - 127.0f);
      dest->y = y_bar*ri_cov_yy;
      dest->u = u_bar*ri_cov_uu;
      dest->v = v_bar*ri_cov_vv;
    }
    break;
  }
  case ISOTROPIC: {
    covar.Fill(0.0);
    // Get 0,0 element of covar
    for (unsigned int i=0;i<N;i++) {
      const float y = data[i].y;
      const float u = data[i].u-127.0f; // subtract centre value
      const float v = data[i].v-127.0f; // subtract centre value
      my_mean += data[i];
      covar[0][0] += y*y + u*u + v*v;
    }
    const float scale = 1.0f/float(N);
    my_mean *= scale;
    covar[0][0] *= scale;
    const float my = my_mean.y;
//     const float mu = my_mean.u-127.0; // subtract centre value
//     const float mv = my_mean.v-127.0; // subtract centre value
    //    covar[0][0] -= (my*my + mu*mu + mv*mv); 
    covar[0][0] -= (my*my); 

    covar[0][0] /= 3.0f;
    covar[2][2] = covar[1][1] = covar[0][0];
    // Now form root inv covar and normalise data
    const float ri_cov = 1.0f/sqrt(covar[0][0]);
    
    norm_data.resize(data.size());
    vector<VW::PixelYUV<float> >::const_iterator src = data.begin();
    vector<VW::PixelYUV<float> >::const_iterator src_end = data.end();
    vector<VW::PixelYUV<float> >::iterator dest = norm_data.begin();
    for ( ; src!=src_end ; src++, dest++) {
      const float y_bar = (src->y - my_mean.y);
//       const float u_bar = (src->u - my_mean.u);
//       const float v_bar = (src->v - my_mean.v);
      const float u_bar = (src->u - 127.0f);
      const float v_bar = (src->v - 127.0f);
      dest->y = y_bar*ri_cov;
      dest->u = u_bar*ri_cov;
      dest->v = v_bar*ri_cov;
    }

    break;
  }
  case MONOCHROME: {
    covar.Fill(0.0);
    // Get 0,0 element of covar
    for (unsigned int i=0;i<N;i++) {
      my_mean += data[i];
      const float y = data[i].y;
      covar[0][0] += y*y;
    }
    const float scale = 1.0f/float(N);
    my_mean *= scale;
    covar[0][0] *= scale;
    const float my = my_mean.y;
    covar[0][0] -= (my*my); 
    covar[0][0] /= 3; 

    covar[2][2] = covar[1][1] = covar[0][0];
    // Now form root inv covar and normalise data
    const float ri_cov = 1.0f/sqrt(covar[0][0]);
    
    norm_data.resize(data.size());
    vector<VW::PixelYUV<float> >::const_iterator src = data.begin();
    vector<VW::PixelYUV<float> >::const_iterator src_end = data.end();
    vector<VW::PixelYUV<float> >::iterator dest = norm_data.begin();
    for ( ; src!=src_end ; src++, dest++) {
      const float y_bar = (src->y - my_mean.y);
      dest->y = y_bar*ri_cov;
      dest->u = 0;
      dest->v = 0;
    }

    break;
  }
  } // End switch
  sums_up_to_date=true;
}
float VW::ImagePatchYUV::GetZNCC(const VW::ImagePatchYUV& p2) const
{
  assert(this->GetWidth() == p2.GetWidth());
  assert(this->GetHeight() == p2.GetHeight());
  if (!sums_up_to_date) CalcSums();
  if (!p2.sums_up_to_date) p2.CalcSums();
//   cerr << norm_data.size() << " " << p2.norm_data.size() << endl;

  float result=0.0f;
  std::vector<VW::PixelYUV<float> >::const_iterator i1 = norm_data.begin(), 
                                     i2 = p2.norm_data.begin();
  const unsigned int n=norm_data.size();
  for (unsigned int i=0;i<n;i++,i1++,i2++) {
//     cerr << i << "("<<i1->y<<", "<<i1->u<<", "<<i1->v<<").("<<i2->y<<", "<<i2->u<<", "<<i2->v<<")"<<endl;
    result += ( (i1->y * i2->y) 
		+ (i1->u * i2->u) 
		+ (i1->v * i2->v) );
  }
  result /= (3*n);
  // Numerical errors sometimes cause probs
  if (VW::IsNaN(result)) {
    cerr << "GetZNCC: NaN!" << endl;
    cerr << "n="<<n<<endl;
    VNL::MatrixFixed<3,3,float> C = p2.GetCovariance();
    cerr << "covar2=[\n"<<C<<"]"<<endl;
    const float& a = C[1][1];
    const float& b = 0.5f*(C[1][2]+C[2][1]);
    const float& c = C[2][2];
    const float& det = a*c - b*b;
    cerr << "det = "<<det<<endl;
    cerr << "p1:\n";
    for (unsigned int i=0;i<n;i++) cerr << "   ["<<norm_data[i].y<<","<<norm_data[i].u<<","<<norm_data[i].v<<"]\n";
    cerr << "p2:\n";
    for (unsigned int i=0;i<n;i++) cerr << "   ["<<p2.norm_data[i].y<<","<<p2.norm_data[i].u<<","<<p2.norm_data[i].v<<"]\n";
  }

  if (result>1.0f) {
    cerr << "GetZNCC: result out of bounds ("<<result<<")"<<endl;
    result=1.0f;
  }
  if (result<-1.0f) {
    cerr << "GetZNCC: result out of bounds ("<<result<<")"<<endl;
    result=-1.0f;
  }
  return result;
};
