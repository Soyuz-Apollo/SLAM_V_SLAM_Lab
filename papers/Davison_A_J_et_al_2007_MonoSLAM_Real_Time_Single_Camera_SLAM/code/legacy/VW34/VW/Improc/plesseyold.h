/*
 *                   The Plessey Corner Detector
 *              (with Harris-Stephens scoring system)
 *                          (old version)
 *
 * AUTHORS: ian, bjt
 * SHORT:   the Plessey (or Harris) corner detection system. The main
 *          detector is a function object 'Plessey'. Also defined are
 *          a 'PlesseyCorner' class which extends 'Point2D' and a
 *          'PlesseyParams' class which holds the user-controlled
 *          params.
 *          See REAMDE.txt file for a description of how/why, etc
 *
 */




#ifndef _plesseyold_h_
#define _plesseyold_h_

// System includes
#include <vector>

// VW includes (these had better all be above this in the dependency tree)
#include <VW/geom.h>
#include <VW/image.h>
#include <VW/iobasics.h>

// Local includes
#include "convolve.h"
#include "plesseycorner.h"



namespace VW {

// These default settings appear severally, so
// are set here
  namespace PlesseyOldParamsDefaults {
    const float DEF_THRESH = 0.25;
    const float DEF_SIGMA = 1.0;
    const int DEF_SIZE = 5;
    const float DEF_EDGEPHOBIA = 0.05;
  }


/*
 *                 PlesseyOldParams
 *
 * note that the edge_phobia and thresholds should 
 * be values between 0 and 1. An edge-phobia of 0.8 
 * strongly rejects edges, and gives a score of +1 
 * for a perfect corner, -1 for a perfect edge, but
 * reduces the number of corners drastically. 0.05 
 * is a good compromise.
 */

class PlesseyOldParams {
 public:
  friend class PlesseyOld;

  // Genesis & Revelations
  PlesseyOldParams() : 
    strength_threshold(PlesseyOldParamsDefaults::DEF_THRESH),
    gaussian_sigma(PlesseyOldParamsDefaults::DEF_SIGMA),
    patch_size(PlesseyOldParamsDefaults::DEF_SIZE),
    edge_phobia(PlesseyOldParamsDefaults::DEF_EDGEPHOBIA) {};

  PlesseyOldParams(float        _strength_threshold,
		float        _gaussian_sigma,
		unsigned int _patch_size,
		float        _edge_phobia) :
    strength_threshold(_strength_threshold),
    gaussian_sigma(_gaussian_sigma),
    patch_size(_patch_size),
    edge_phobia(_edge_phobia) {};
    
  virtual ~PlesseyOldParams() {};


  // Access functions
  inline float        GetStrengthThreshold() const
    {return(strength_threshold);};
  inline float        GetGaussianSigma() const     
    {return(gaussian_sigma);};
  inline unsigned int GetPatchSize() const         
    {return(patch_size);};
  inline float        GetEdgePhobia() const        
    {return(edge_phobia);};


  // Manipulator functions
  virtual void LoadParameterFile (string paramfile = ".params");
  virtual void SaveParameterFile (string paramfile = ".params");
  virtual void Set(const float        new_strengththreshold,
		   const float        new_gaussiansigma,
		   const unsigned int new_patchsize,
		   const float        new_edgephobia) {
    strength_threshold = new_strengththreshold;
    gaussian_sigma     = new_gaussiansigma;
    patch_size         = new_patchsize;
    edge_phobia        = new_edgephobia;
  };

  virtual void SetStrengthThreshold(const float new_value)
    {strength_threshold=new_value;};

  virtual void SetGaussianSigma(const float new_value)
    {gaussian_sigma=new_value;};

  virtual void SetPatchSize(const unsigned int new_value)
    {patch_size=new_value;};

  virtual void SetEdgePhobia(const float new_value)
    {edge_phobia=new_value;};

 protected:
  float        strength_threshold;
  float        gaussian_sigma;
  unsigned int patch_size;
  float        edge_phobia;
};



/*
 *              PlesseyOld
 */

class PlesseyOld {    

public:
  PlesseyOld(string paramfile = "");
  PlesseyOld(const ImageMono<unsigned char> &im, 
	  vector<PlesseyCorner>& corners,
	  float thresh = PlesseyOldParamsDefaults::DEF_THRESH, 
	  float sigma = PlesseyOldParamsDefaults::DEF_SIGMA, 
	  int size = PlesseyOldParamsDefaults::DEF_SIZE, 
	  float ep = PlesseyOldParamsDefaults::DEF_EDGEPHOBIA,
	  const Normalisation& C = Normalisation());
  PlesseyOld(const ImageMono<unsigned char> &im, 
	  vector<PlesseyCorner>& corners,
	  const PlesseyOldParams& _params,
	  const Normalisation& C = Normalisation());
  PlesseyOld(const ImageMono<unsigned char> &im, 
	  vector<PlesseyCorner>& corners,
	  string paramfile,
	  const Normalisation& C = Normalisation());

  PlesseyOld(const ImageYUV<unsigned char> &im, 
	  vector<PlesseyCorner>& corners,
	  float thresh = PlesseyOldParamsDefaults::DEF_THRESH, 
	  float sigma = PlesseyOldParamsDefaults::DEF_SIGMA, 
	  int size = PlesseyOldParamsDefaults::DEF_SIZE, 
	  float ep = PlesseyOldParamsDefaults::DEF_EDGEPHOBIA,
	  const Normalisation& C = Normalisation());
  PlesseyOld(const ImageYUV<unsigned char> &im, 
	  vector<PlesseyCorner>& corners,
	  const PlesseyOldParams& _params,
	  const Normalisation& C = Normalisation());
  PlesseyOld(const ImageYUV<unsigned char> &im, 
	  vector<PlesseyCorner>& corners,
	  string paramfile,
	  const Normalisation& C = Normalisation());

  ~PlesseyOld() {}; 

  operator bool() const { return true; }

  void Compute(const ImageMono<unsigned char> &im, 
	       vector<PlesseyCorner>& corners,
	       const Normalisation& C = Normalisation()) const;

  void Compute(const ImageYUV<unsigned char> &im, 
	       vector<PlesseyCorner>& corners,
	       const Normalisation& C = Normalisation()) const;


  PlesseyOldParams* GetParams()                          {return(&params);};
  void           SetParams(PlesseyOldParams& new_params) {params=new_params;};

  bool Verbose;

private:

  void calc_gradients(const ImageMono<unsigned char> &im,
		      ImageMono<float> &gx2arr,
		      ImageMono<float> &gy2arr,
		      ImageMono<float> &gxyarr) const;
  void calc_gradients(const ImageYUV<unsigned char> &im,
		      ImageMono<float> &gx2arr,
		      ImageMono<float> &gy2arr,
		      ImageMono<float> &gxyarr) const;
  void compute_corner_strength(const ImageMono<float> &gradx2,
			       const ImageMono<float> &grady2,
			       const ImageMono<float> &gradxy,
			       ImageMono<float> &corn_strength ) const;
  void compute_corners(const ImageMono<float> &strength, 
		       float score_threshold,
		       float raw_thresh, 
		       vector<PlesseyCorner>& pl) const;
  float get_max_strength(const ConvolutionMask1D<float> gauss_mask) const;
  void FitQuadratic3x3(float& peak_value,
		       float& peak_x,
		       float& peak_y,
		       const float data[9]) const;

  // the data that Compute() will work on 
  PlesseyOldParams params;
};



}; // end of VW namespace

// Read and write functions live in the VW::Misc namespace
template <>
bool VW::XMLStuff::ReadXMLString<VW::PlesseyOldParams>(const string& str,
						    VW::PlesseyOldParams& p);
template <>
string VW::XMLStuff::GenerateXMLString<VW::PlesseyOldParams>(const VW::PlesseyOldParams& p, 
						      unsigned int indentation);

/// INLINES ///
inline VW::PlesseyOld::PlesseyOld (string paramfile = "") :
  Verbose (false) 
{
  if (paramfile != "") params.LoadParameterFile (paramfile);
}

inline VW::PlesseyOld::PlesseyOld(const ImageMono<unsigned char> &im, 
			    vector<PlesseyCorner>& corners,
			    float thresh, float sigma, 
			    int size, float ep,
			    const VW::Normalisation& C) :
  Verbose(false),
  params(thresh,sigma,size,ep)
{
  Compute(im, corners, C);
}

inline VW::PlesseyOld::PlesseyOld(const ImageMono<unsigned char> &im, 
			    vector<PlesseyCorner>& corners,
			    const PlesseyOldParams& _params,
			    const VW::Normalisation& C) :
  Verbose(false),
  params(_params)
{
  Compute(im, corners, C);
}

inline VW::PlesseyOld::PlesseyOld(const ImageMono<unsigned char> &im, 
			    vector<PlesseyCorner>& corners,
			    string paramfile,
			    const VW::Normalisation& C) :
  Verbose(false)
{
  if (paramfile != "") params.LoadParameterFile (paramfile);
  Compute(im, corners, C);
}

inline VW::PlesseyOld::PlesseyOld(const ImageYUV<unsigned char> &im, 
			    vector<PlesseyCorner>& corners,
			    float thresh, float sigma, 
			    int size, float ep,
			    const VW::Normalisation& C) :
  Verbose(false),
  params(thresh,sigma,size,ep)
{
  Compute(im, corners, C);
}

inline VW::PlesseyOld::PlesseyOld(const ImageYUV<unsigned char> &im, 
			    vector<PlesseyCorner>& corners,
			    const PlesseyOldParams& _params,
			    const VW::Normalisation& C) :
  Verbose(false),
  params(_params)
{
  Compute(im, corners, C);
}

inline VW::PlesseyOld::PlesseyOld(const ImageYUV<unsigned char> &im, 
			    vector<PlesseyCorner>& corners,
			    string paramfile,
			    const VW::Normalisation& C) :
  Verbose(false)
{
  if (paramfile != "") params.LoadParameterFile (paramfile);
  Compute(im, corners, C);
}



#endif /* _plessey_h_ */
