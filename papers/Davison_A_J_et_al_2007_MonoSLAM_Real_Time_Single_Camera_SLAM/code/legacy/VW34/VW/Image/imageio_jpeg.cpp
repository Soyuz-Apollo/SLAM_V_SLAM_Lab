#include "pixelconversions.h"
#include "imagemono.h"
#include "imagergb.h"
#include "imageyuv.h"


#include "imageio_jpeg.h"
#include <stdio.h>
#include <iostream>

using namespace std;

extern "C" {
#include <ctype.h>		/* to declare isprint() */
#include <stdlib.h>
#include <stdio.h>
/* Note: jpeglib.h prerequires stdlib.h and ctype.h.  It should include them
   itself, but doesn't.
*/
#include <jpeglib.h>
}

#include <VW/image.h>
#include <iostream>

using namespace std;

#define VERBOSE false

#define BUFFER_SIZE 4096


/////////////// READING ///////////////


struct sourceManager {
    /* The following structure _must_ be the first thing in this structure,
       because the methods below assume the address of the struct
       jpeg_source_mgr is the same as the address of the struct sourceManager.
       The former address is the only information the Jpeg library gives
       these methods to tell them on what source manager object they are
       supposed to operate.
    */
    struct jpeg_source_mgr jpegSourceMgr;
    FILE * ifP;
    JOCTET * currentBuffer;
    JOCTET * nextBuffer;
    unsigned int bytesInNextBuffer;
    JOCTET buffer1[BUFFER_SIZE];
    JOCTET buffer2[BUFFER_SIZE];
};


void
saveMarkers(struct jpeg_decompress_struct* const cinfoP) 
{
	// Save marker support appears to be disabled on windows
//#ifndef _WIN32
  unsigned int app_type;
  // Get all the miscellaneous markers (COM and APPn) saved for later access.
  jpeg_save_markers(cinfoP, JPEG_COM, 65535);
  for (app_type = 0; app_type <= 15; ++app_type) {
    if (app_type == 0 || app_type == 14) {
      /* The jpeg library uses APP0 and APP14 internally (see
	 libjpeg.doc), so we don't mess with those.
      */
    } else {
      jpeg_save_markers(cinfoP, JPEG_APP0 + app_type, 65535);
    }
  }
//#endif
}


void
beginJpegInput(struct jpeg_decompress_struct* const cinfoP,
               const J_DCT_METHOD dct_method, 
               const int max_memory_to_use, 
               const boolean nosmooth) 
{
/*----------------------------------------------------------------------------
   Read the JPEG header, create decompressor object (and
   allocate memory for it), set up decompressor.
-----------------------------------------------------------------------------*/
    /* Read file header, set default decompression parameters */
    jpeg_read_header(cinfoP, TRUE);

    cinfoP->dct_method = dct_method;
    if (max_memory_to_use != -1)
        cinfoP->mem->max_memory_to_use = max_memory_to_use;
    if (nosmooth)
        cinfoP->do_fancy_upsampling = FALSE;
    
}

static void
dsInitSourceFunc(j_decompress_ptr const cinfoP) {
/*----------------------------------------------------------------------------
   This is the init_source method for the data source manager the Jpeg
   library uses.
-----------------------------------------------------------------------------*/
    /* The object was created ready to go and remains ready from one image
       to the next.  No per-image initialization is necessary.
    */
}

static boolean
dsFillInputBuffer(j_decompress_ptr const cinfoP) {
/*----------------------------------------------------------------------------
   This is the fill_input_buffer method for the data source manager the Jpeg
   library uses.
-----------------------------------------------------------------------------*/
    struct sourceManager * const srcP = (struct sourceManager *) cinfoP->src;

    if (srcP->bytesInNextBuffer == 0) 
      fprintf(stderr,"End-of-file encountered in the middle of JPEG image.\n");
    else {
        /* Rotate the buffers */
        srcP->jpegSourceMgr.next_input_byte = srcP->nextBuffer;
        srcP->jpegSourceMgr.bytes_in_buffer = srcP->bytesInNextBuffer;
        {
            JOCTET * tmp;
            tmp = srcP->nextBuffer;
            srcP->nextBuffer = srcP->currentBuffer;
            srcP->currentBuffer = tmp;
        }

        /* Fill the new 'next' buffer */
        srcP->bytesInNextBuffer = 
            fread(srcP->nextBuffer, 1, BUFFER_SIZE, srcP->ifP);
    }
    return TRUE;
}

static void
dsSkipInputData(j_decompress_ptr const cinfoP, long const num_bytes) {
/*----------------------------------------------------------------------------
   This is the skip_input_data method for the data source manager the Jpeg
   library uses.
-----------------------------------------------------------------------------*/
    struct jpeg_source_mgr * const jpegSourceMgrP = cinfoP->src;

    long i;

    for (i = 0; i < num_bytes; ++i) {
        if (jpegSourceMgrP->bytes_in_buffer == 0)
            dsFillInputBuffer(cinfoP);
        ++jpegSourceMgrP->next_input_byte;
        --jpegSourceMgrP->bytes_in_buffer;
    }
}
static void
dsTermSource(j_decompress_ptr const cinfoP) {
/*----------------------------------------------------------------------------
   This is the term_source method for the data source manager the Jpeg
   library uses.
-----------------------------------------------------------------------------*/
    /* We couldn't care less that the Jpeg library is done reading an
       image.  The source manager object remains active and ready for the
       Jpeg library to read the next image.
    */
}
//////////////////////////////////////////////////////////

static void InitJPEGSource(struct jpeg_decompress_struct& jinfo,
			   struct jpeg_error_mgr& jerr,
			   struct sourceManager& jsourcemanager,
			   FILE* file)
{
  /* Initialize the JPEG decompression object with default error handling. */
  jinfo.err = jpeg_std_error(&jerr);
  jinfo.err->trace_level = VERBOSE;
  jpeg_create_decompress(&jinfo);
  

  // BJT: What does this do?
  saveMarkers(&jinfo);
  
  // Create the data source looking at the file
  jsourcemanager.ifP = file;

  jsourcemanager.jpegSourceMgr.init_source = dsInitSourceFunc;
  jsourcemanager.jpegSourceMgr.fill_input_buffer = dsFillInputBuffer;
  jsourcemanager.jpegSourceMgr.skip_input_data = dsSkipInputData;
  jsourcemanager.jpegSourceMgr.resync_to_restart = jpeg_resync_to_restart;
  jsourcemanager.jpegSourceMgr.term_source = dsTermSource;
  
  jsourcemanager.currentBuffer = jsourcemanager.buffer1;
  jsourcemanager.nextBuffer = jsourcemanager.buffer2;
  jsourcemanager.jpegSourceMgr.bytes_in_buffer = 
    fread(jsourcemanager.currentBuffer, 1, BUFFER_SIZE, jsourcemanager.ifP);
  jsourcemanager.jpegSourceMgr.next_input_byte = jsourcemanager.currentBuffer;
  jsourcemanager.bytesInNextBuffer = 
    fread(jsourcemanager.nextBuffer, 1, BUFFER_SIZE, jsourcemanager.ifP);


  jinfo.src = &(jsourcemanager.jpegSourceMgr);
}

bool VW::ReadJPEGImage(FILE* filepointer,
                      VW::ImageMono<unsigned char>& image)
{
    struct jpeg_decompress_struct jinfo;
    struct jpeg_error_mgr jerr;
    struct sourceManager jsourcemanager;

    InitJPEGSource(jinfo, jerr, jsourcemanager, filepointer);

    { // Some parameters
    const J_DCT_METHOD dct_method = JDCT_DEFAULT; // JDCT_ISLOW, JDCT_IFAST, JDCT_FLOAT
    const long int max_memory_to_use = -1; // unspecified
    const int no_smoothing = 0;
    beginJpegInput(&jinfo,
                   dct_method, 
                   max_memory_to_use, 
		   no_smoothing);
    }
                   
    /* The maximum value of a sample (color component), both in the input
     * and the output.
     */
    unsigned int maxval = (1 << jinfo.data_precision) - 1;

    /* Calculate output image dimensions so we can allocate space */
    JSAMPROW jpegbuffer;  /* Input buffer.  Filled by jpeg_scanlines() */
    jpeg_calc_output_dimensions(&jinfo);

    jpegbuffer = ((*jinfo.mem->alloc_sarray)
                  ((j_common_ptr) &jinfo, JPOOL_IMAGE,
                   jinfo.output_width * jinfo.output_components, 
                   (JDIMENSION) 1)
        )[0];

    /* Start decompressor */
    jpeg_start_decompress(&jinfo);

    if (VERBOSE) cerr << "Size = " << jinfo.output_width << "x" << jinfo.output_height << " with maxval " << maxval << endl;
  if (image.IsAlloced()) {
    if (jinfo.output_width != image.GetWidth() || jinfo.output_height != image.GetHeight()) {
      cerr << "VW::ReadJPEGImage(): Image size mismatch\n";
      return false;
    }
  } else {
    image.AllocImageData(jinfo.output_width, jinfo.output_height);
  }

  const J_COLOR_SPACE colorspace = jinfo.out_color_space;
    // Now actually read the data
    while (jinfo.output_scanline < jinfo.output_height) {
      VW::PixelMono<unsigned char>* out_ptr = image.GetRowStart(jinfo.output_scanline);
      // Note that reading the scanline also increments the counter
      jpeg_read_scanlines(&jinfo, &jpegbuffer, 1);
      JSAMPLE *in_ptr = jpegbuffer;
      switch (colorspace) {
      case JCS_GRAYSCALE: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  out_ptr->y = GETJSAMPLE(*in_ptr++);
	}
	break;
      }
	
      case JCS_RGB: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  const int r = GETJSAMPLE(*in_ptr++);
	  const int g = GETJSAMPLE(*in_ptr++); 
	  const int b = GETJSAMPLE(*in_ptr++); 
	  VW::ImageConversions::RGB2Y(r,g,b,out_ptr->y);
	}
	break;
      }
	
      case JCS_YCbCr: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  out_ptr->y = GETJSAMPLE(*in_ptr++);
	  in_ptr+=2; // Skip U,V
	}
	break;
      }
	
      case JCS_CMYK: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  const int c = GETJSAMPLE(*in_ptr++);
	  const int m = GETJSAMPLE(*in_ptr++);
	  const int y = GETJSAMPLE(*in_ptr++);
	  const int k = GETJSAMPLE(*in_ptr++);
	  const int r = (k*c)/maxval;
	  const int g = (k*m)/maxval;
	  const int b = (k*y)/maxval;
	  VW::ImageConversions::RGB2Y(r,g,b,out_ptr->y);
	}
	break;
      }
	
      case JCS_YCCK: {
	cerr << "YCCK not supported yet." << endl;
	exit(0);
	break;
      }
	
      default: {
	cerr << "Unknown or unsupported colourspace." << endl;
      }
      }
    }
    
    if (VERBOSE) cerr << "Finished, so deallocate" << endl;
    // Finish decompression and release decompressor memory.
    jpeg_finish_decompress(&jinfo);
   jpeg_destroy_decompress(&jinfo);

  return (true);
}

  
bool VW::ReadJPEGImage(FILE* filepointer,
                      VW::ImageMono<float>& image)
{
    struct jpeg_decompress_struct jinfo;
    struct jpeg_error_mgr jerr;
    struct sourceManager jsourcemanager;

    InitJPEGSource(jinfo, jerr, jsourcemanager, filepointer);

    { // Some parameters
    const J_DCT_METHOD dct_method = JDCT_DEFAULT; // JDCT_ISLOW, JDCT_IFAST, JDCT_FLOAT
    const long int max_memory_to_use = -1; // unspecified
    const int no_smoothing = 0;
    beginJpegInput(&jinfo,
                   dct_method, 
                   max_memory_to_use, 
		   no_smoothing);
    }
                   
    /* The maximum value of a sample (color component), both in the input
     * and the output.
     */
    unsigned int maxval = (1 << jinfo.data_precision) - 1;

    /* Calculate output image dimensions so we can allocate space */
    JSAMPROW jpegbuffer;  /* Input buffer.  Filled by jpeg_scanlines() */
    jpeg_calc_output_dimensions(&jinfo);

    jpegbuffer = ((*jinfo.mem->alloc_sarray)
                  ((j_common_ptr) &jinfo, JPOOL_IMAGE,
                   jinfo.output_width * jinfo.output_components, 
                   (JDIMENSION) 1)
        )[0];

    /* Start decompressor */
    jpeg_start_decompress(&jinfo);

    if (VERBOSE) cerr << "Size = " << jinfo.output_width << "x" << jinfo.output_height << " with maxval " << maxval << endl;
  if (image.IsAlloced()) {
    if (jinfo.output_width != image.GetWidth() || jinfo.output_height != image.GetHeight()) {
      cerr << "VW::ReadJPEGImage(): Image size mismatch\n";
      return false;
    }
  } else {
    image.AllocImageData(jinfo.output_width, jinfo.output_height);
  }

  const J_COLOR_SPACE colorspace = jinfo.out_color_space;
    // Now actually read the data
    while (jinfo.output_scanline < jinfo.output_height) {
      VW::PixelMono<float>* out_ptr = image.GetRowStart(jinfo.output_scanline);
      // Note that reading the scanline also increments the counter
      jpeg_read_scanlines(&jinfo, &jpegbuffer, 1);
      JSAMPLE *in_ptr = jpegbuffer;
      switch (colorspace) {
      case JCS_GRAYSCALE: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  out_ptr->y = float(GETJSAMPLE(*in_ptr++));
	}
	break;
      }
	
      case JCS_RGB: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  const int r = GETJSAMPLE(*in_ptr++);
	  const int g = GETJSAMPLE(*in_ptr++); 
	  const int b = GETJSAMPLE(*in_ptr++); 
	  float y=0;
	  VW::ImageConversions::RGB2Y(r,g,b,y);
	  out_ptr->y = y;
	}
	break;
      }
	
      case JCS_YCbCr: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  out_ptr->y = float(GETJSAMPLE(*in_ptr++));
	  in_ptr+=2; // Skip U,V
	}
	break;
      }
	
      case JCS_CMYK: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  const int c = GETJSAMPLE(*in_ptr++);
	  const int m = GETJSAMPLE(*in_ptr++);
	  const int y = GETJSAMPLE(*in_ptr++);
	  const int k = GETJSAMPLE(*in_ptr++);
	  const int r = (k*c)/maxval;
	  const int g = (k*m)/maxval;
	  const int b = (k*y)/maxval;
	  float y2=0;
	  VW::ImageConversions::RGB2Y(r,g,b,y2);
	  out_ptr->y = y2;
	}
	break;
      }
	
      case JCS_YCCK: {
	cerr << "YCCK not supported yet." << endl;
	exit(0);
	break;
      }
	
      default: {
	cerr << "Unknown or unsupported colourspace." << endl;
      }
      }
    }
    
    if (VERBOSE) cerr << "Finished, so deallocate" << endl;
    // Finish decompression and release decompressor memory.
    jpeg_finish_decompress(&jinfo);
   jpeg_destroy_decompress(&jinfo);

  return (true);
}

  
bool VW::ReadJPEGImage(FILE* filepointer,
                     VW::ImageYUV<unsigned char>& image)
{
    struct jpeg_decompress_struct jinfo;
    struct jpeg_error_mgr jerr;
    struct sourceManager jsourcemanager;

    InitJPEGSource(jinfo, jerr, jsourcemanager, filepointer);

    { // Some parameters
    const J_DCT_METHOD dct_method = JDCT_DEFAULT; // JDCT_ISLOW, JDCT_IFAST, JDCT_FLOAT
    const long int max_memory_to_use = -1; // unspecified
    const int no_smoothing = 0;
    beginJpegInput(&jinfo,
                   dct_method, 
                   max_memory_to_use, 
		   no_smoothing);
    }
                   
    /* The maximum value of a sample (color component), both in the input
     * and the output.
     */
    unsigned int maxval = (1 << jinfo.data_precision) - 1;

    /* Calculate output image dimensions so we can allocate space */
    JSAMPROW jpegbuffer;  /* Input buffer.  Filled by jpeg_scanlines() */
    jpeg_calc_output_dimensions(&jinfo);

    jpegbuffer = ((*jinfo.mem->alloc_sarray)
                  ((j_common_ptr) &jinfo, JPOOL_IMAGE,
                   jinfo.output_width * jinfo.output_components, 
                   (JDIMENSION) 1)
        )[0];

    /* Start decompressor */
    jpeg_start_decompress(&jinfo);

    if (VERBOSE) cerr << "Size = " << jinfo.output_width << "x" << jinfo.output_height << " with maxval " << maxval << endl;
  if (image.IsAlloced()) {
    if (jinfo.output_width != image.GetWidth() || jinfo.output_height != image.GetHeight()) {
      cerr << "VW::ReadJPEGImage(): Image size mismatch\n";
      return false;
    }
  } else {
    image.AllocImageData(jinfo.output_width, jinfo.output_height);
  }

  const J_COLOR_SPACE colorspace = jinfo.out_color_space;
    // Now actually read the data
    while (jinfo.output_scanline < jinfo.output_height) {
      VW::PixelYUV<unsigned char>* out_ptr = image.GetRowStart(jinfo.output_scanline);
      // Note that reading the scanline also increments the counter
      jpeg_read_scanlines(&jinfo, &jpegbuffer, 1);
      JSAMPLE *in_ptr = jpegbuffer;
      switch (colorspace) {
      case JCS_GRAYSCALE: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  out_ptr->y = GETJSAMPLE(*in_ptr++);
	  out_ptr->u = 127;
	  out_ptr->v = 127;
	  out_ptr->alpha = 0;
	}
	break;
      }
	
      case JCS_RGB: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  const int r = GETJSAMPLE(*in_ptr++);
	  const int g = GETJSAMPLE(*in_ptr++); 
	  const int b = GETJSAMPLE(*in_ptr++); 
	  VW::ImageConversions::RGB2YUV(r,g,b,
					out_ptr->y,out_ptr->u,out_ptr->v);
	  out_ptr->alpha = 0;
	}
	break;
      }
	
      case JCS_YCbCr: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  out_ptr->y = GETJSAMPLE(*in_ptr++);
	  out_ptr->u = GETJSAMPLE(*in_ptr++);
	  out_ptr->v = GETJSAMPLE(*in_ptr++);
	  out_ptr->alpha = 0;
	}
	break;
      }
	
      case JCS_CMYK: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  const int c = GETJSAMPLE(*in_ptr++);
	  const int m = GETJSAMPLE(*in_ptr++);
	  const int y = GETJSAMPLE(*in_ptr++);
	  const int k = GETJSAMPLE(*in_ptr++);
	  const int r = (k*c)/maxval;
	  const int g = (k*m)/maxval;
	  const int b = (k*y)/maxval;
	  VW::ImageConversions::RGB2YUV(r,g,b,
					out_ptr->y,out_ptr->u,out_ptr->v);
	  out_ptr->alpha = 0;
	}
	break;
      }
	
      case JCS_YCCK: {
	cerr << "YCCK not supported yet." << endl;
	exit(0);
	break;
      }
	
      default: {
	cerr << "Unknown or unsupported colourspace." << endl;
      }
      }
    }
    
    if (VERBOSE) cerr << "Finished, so deallocate" << endl;
    // Finish decompression and release decompressor memory.
    jpeg_finish_decompress(&jinfo);
   jpeg_destroy_decompress(&jinfo);

  return (true);
}

bool VW::ReadJPEGImage(FILE* filepointer,
                     VW::ImageRGB<unsigned char>& image)
{
  struct jpeg_decompress_struct jinfo;
    struct jpeg_error_mgr jerr;
    struct sourceManager jsourcemanager;

    InitJPEGSource(jinfo, jerr, jsourcemanager, filepointer);

    { // Some parameters
    const J_DCT_METHOD dct_method = JDCT_DEFAULT; // JDCT_ISLOW, JDCT_IFAST, JDCT_FLOAT
    const long int max_memory_to_use = -1; // unspecified
    const int no_smoothing = 0;
    beginJpegInput(&jinfo,
                   dct_method, 
                   max_memory_to_use, 
		   no_smoothing);
    }
                   
    /* The maximum value of a sample (color component), both in the input
     * and the output.
     */
    unsigned int maxval = (1 << jinfo.data_precision) - 1;

    /* Calculate output image dimensions so we can allocate space */
    JSAMPROW jpegbuffer;  /* Input buffer.  Filled by jpeg_scanlines() */
    jpeg_calc_output_dimensions(&jinfo);

    jpegbuffer = ((*jinfo.mem->alloc_sarray)
                  ((j_common_ptr) &jinfo, JPOOL_IMAGE,
                   jinfo.output_width * jinfo.output_components, 
                   (JDIMENSION) 1)
        )[0];

    /* Start decompressor */
    jpeg_start_decompress(&jinfo);

    if (VERBOSE) cerr << "Size = " << jinfo.output_width << "x" << jinfo.output_height << " with maxval " << maxval << endl;
  if (image.IsAlloced()) {
    if (jinfo.output_width != image.GetWidth() || jinfo.output_height != image.GetHeight()) {
      cerr << "VW::ReadJPEGImage(): Image size mismatch\n";
      return false;
    }
  } else {
    image.AllocImageData(jinfo.output_width, jinfo.output_height);
  }

  const J_COLOR_SPACE colorspace = jinfo.out_color_space;
    // Now actually read the data
    while (jinfo.output_scanline < jinfo.output_height) {
      VW::PixelRGB<unsigned char>* out_ptr = image.GetRowStart(jinfo.output_scanline);
      // Note that reading the scanline also increments the counter
      jpeg_read_scanlines(&jinfo, &jpegbuffer, 1);
      JSAMPLE *in_ptr = jpegbuffer;
      switch (colorspace) {
      case JCS_GRAYSCALE: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  const JSAMPLE y =GETJSAMPLE(*in_ptr++);
	  out_ptr->r = y;
	  out_ptr->g = y;
	  out_ptr->b = y;
	  out_ptr->alpha = 0;
	}
	break;
      }
	
      case JCS_RGB: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  out_ptr->r = GETJSAMPLE(*in_ptr++);
	  out_ptr->g = GETJSAMPLE(*in_ptr++); 
	  out_ptr->b = GETJSAMPLE(*in_ptr++); 
	  out_ptr->alpha = 0; 
	}
	break;
      }
	
      case JCS_YCbCr: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  const int y = GETJSAMPLE(*in_ptr++);
	  const int u = GETJSAMPLE(*in_ptr++); 
	  const int v = GETJSAMPLE(*in_ptr++); 
	  VW::ImageConversions::YUV2RGB(y,u,v,
					out_ptr->r, out_ptr->g, out_ptr->b);
	  out_ptr->alpha = 0;
	}
	break;
      }
	
      case JCS_CMYK: {
	for (unsigned int i=0; i < jinfo.output_width; i++, out_ptr++) {
	  const int c = GETJSAMPLE(*in_ptr++);
	  const int m = GETJSAMPLE(*in_ptr++);
	  const int y = GETJSAMPLE(*in_ptr++);
	  const int k = GETJSAMPLE(*in_ptr++);
	  out_ptr->r = (k*c)/maxval;
	  out_ptr->g = (k*m)/maxval;
	  out_ptr->b = (k*y)/maxval;
	  out_ptr->alpha = 0;
	}
	break;
      }
	
      case JCS_YCCK: {
	cerr << "YCCK not supported yet." << endl;
	exit(0);
	break;
      }
	
      default: {
	cerr << "Unknown or unsupported colourspace." << endl;
      }
      }
    }
    
    if (VERBOSE) cerr << "Finished, so deallocate" << endl;
    // Finish decompression and release decompressor memory.
    jpeg_finish_decompress(&jinfo);
   jpeg_destroy_decompress(&jinfo);

  return (true);
}

















/////////////// WRITING ///////////////





static void
SetupOutputJPEG(struct jpeg_compress_struct& jinfo,
		struct jpeg_error_mgr& jerr,
		const int width, const int height, 
		const unsigned int maxval,
		const J_COLOR_SPACE input_fmt, 
		const J_COLOR_SPACE output_fmt,
		FILE* const output_file) 
{  
    int quality;
    int q_scale_factor;

    /* Initialize the JPEG compression object with default error handling. */
    jinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&jinfo);

    jinfo.in_color_space = input_fmt;
    switch (input_fmt) {
    case JCS_RGB:
    case JCS_YCbCr:
      jinfo.input_components = 3;
      break;
    case JCS_GRAYSCALE:
      jinfo.input_components = 1;
      break;      
    default:
      cerr << "SetupOutputJPEG: Illegal input format." << endl;
      exit(0);
    };

    jpeg_set_defaults(&jinfo);

    jinfo.data_precision = BITS_IN_JSAMPLE; 
        /* we always rescale data to this */
    jinfo.image_width = (unsigned int) width;
    jinfo.image_height = (unsigned int) height;

    jinfo.arith_code = 0;
    jinfo.dct_method = JDCT_DEFAULT; // JDCT_ISLOW, JDCT_IFAST, JDCT_FLOAT

    if (VERBOSE) jinfo.err->trace_level = 1;
    else jinfo.err->trace_level = 0;

    jpeg_set_colorspace(&jinfo, output_fmt);
    /* This default will be based on the in_color_space set above */
    jpeg_default_colorspace(&jinfo);

    jinfo.optimize_coding = 0;
    quality = 75; // quality = 75% is default
    q_scale_factor = jpeg_quality_scaling(quality);
    // smoothing left at default
    //cinfoP->smoothing_factor = cmdline.smoothing_factor;

    /* Set quantization tables for selected quality. */
    /* Some or all may be overridden if user specified --qtables. */
    jpeg_set_quality(&jinfo, quality, 0);

    // Some other params
    const bool progressive=false, density_spec=false;
    const unsigned int horiz_density = 100, vert_desnity=100;
    if (progressive) jpeg_simple_progression(&jinfo);

    if (density_spec) {
      jinfo.density_unit = 1; // 1=Dots Per Inch , 2=Dots Per cm
      jinfo.X_density = horiz_density;
      jinfo.Y_density = vert_desnity;
    } else {
      jinfo.density_unit = 0; // Unspecified
    }

    /* Specify data destination for compression */
    jpeg_stdio_dest(&jinfo, output_file);

    /* Start compressor */
    jpeg_start_compress(&jinfo, true);

}

static void
ComputeRescalingArray(JSAMPLE** const rescale_p, 
		      const unsigned int maxval,
		      const struct jpeg_compress_struct jinfo) 
{
/*----------------------------------------------------------------------------
   Compute the rescaling array for a maximum pixval of 'maxval'.
   Allocate the memory for it too.
-----------------------------------------------------------------------------*/
  const long half_maxval = maxval / 2;
  unsigned long val;

  *rescale_p = (JSAMPLE *)
    (jinfo.mem->alloc_small) ((j_common_ptr) &jinfo, JPOOL_IMAGE,
                              (size_t) (((long) maxval + 1L) * 
                                        sizeof(JSAMPLE)));
  for (val = 0; val <= maxval; val++) {
    /* The multiplication here must be done in 32 bits to avoid overflow */
    (*rescale_p)[val] = (JSAMPLE) ((val*MAXJSAMPLE + half_maxval)/maxval);
  }
}

bool VW::WriteJPEGImage(FILE* filepointer,
                      const VW::ImageMono<unsigned char>& image)
{
  if (VERBOSE) cerr << "VW::WriteJPEGImage(mono)" << endl;
  struct jpeg_compress_struct jinfo;
  struct jpeg_error_mgr jerr;
  const unsigned int w = image.GetWidth(), h =image.GetHeight();
  const J_COLOR_SPACE input_fmt = JCS_GRAYSCALE; // input is mono
  const J_COLOR_SPACE output_fmt = JCS_GRAYSCALE; // For mono images output a mono jpg
  const unsigned int maxval=255;  // Max number in source image
  JSAMPLE *translation_table;    /* => maxval-remapping array, or NULL */
    /* This is an array that maps each possible pixval in the input to
       a new value such that while the range of the input values is
       0 .. maxval, the range of the output values is 0 .. MAXJSAMPLE.
       */

  SetupOutputJPEG(jinfo, jerr, 
		  w, h, maxval, 
		  input_fmt, output_fmt,
		  filepointer);

  ComputeRescalingArray(&translation_table, maxval, jinfo);

  // Add comment
  std::string comment("Created by VW::WriteJPEGImage(Mono)");
  jpeg_write_marker(&jinfo, JPEG_COM, (const JOCTET*)comment.c_str(), 
		    comment.size());

  // Now do the copying
  JSAMPARRAY buffer = (*jinfo.mem->alloc_sarray)
    ((j_common_ptr) &jinfo, JPOOL_IMAGE,
     (unsigned int) w * jinfo.input_components, 
     (unsigned int) 1);
    /* Row 0 of this array contains the row of the output image currently 
       being processed, in JPEG compressor input format.  The array only
       has that one row.
    */
  while (jinfo.next_scanline < h) {
    if (VERBOSE) cerr << "Converting Row %d..." << jinfo.next_scanline << endl;
    VW::PixelMono<unsigned char>* in_ptr = image.GetRowStart(jinfo.next_scanline);
    JSAMPLE* out_ptr = buffer[0];
    for (unsigned int column = 0; column < w; column++) {
      *out_ptr++ = translation_table[int(in_ptr++->y)];
    }
    jpeg_write_scanlines(&jinfo, buffer, 1);
  }


  // Done!

  /* Finish compression and release memory */
  jpeg_finish_compress(&jinfo);
  jpeg_destroy_compress(&jinfo);

  /* Program may have exited with non-zero completion code via various function
     calls above.
     */
  return(jerr.num_warnings > 0);
}

bool VW::WriteJPEGImage(FILE* filepointer,
                      const VW::ImageMono<float>& image)
{
  if (VERBOSE) cerr << "VW::WriteJPEGImage(mono)" << endl;
  struct jpeg_compress_struct jinfo;
  struct jpeg_error_mgr jerr;
  const unsigned int w = image.GetWidth(), h =image.GetHeight();
  const J_COLOR_SPACE input_fmt = JCS_GRAYSCALE; // input is mono
  const J_COLOR_SPACE output_fmt = JCS_GRAYSCALE; // For mono images output a mono jpg
  const unsigned int maxval=255;  // Max number in source image
  JSAMPLE *translation_table;    /* => maxval-remapping array, or NULL */
    /* This is an array that maps each possible pixval in the input to
       a new value such that while the range of the input values is
       0 .. maxval, the range of the output values is 0 .. MAXJSAMPLE.
       */

  SetupOutputJPEG(jinfo, jerr, 
		  w, h, maxval, 
		  input_fmt, output_fmt,
		  filepointer);

  ComputeRescalingArray(&translation_table, maxval, jinfo);

  // Add comment
  std::string comment("Created by VW::WriteJPEGImage(Mono)");
  jpeg_write_marker(&jinfo, JPEG_COM, (const JOCTET*)comment.c_str(), 
		    comment.size());

  // Now do the copying
  JSAMPARRAY buffer = (*jinfo.mem->alloc_sarray)
    ((j_common_ptr) &jinfo, JPOOL_IMAGE,
     (unsigned int) w * jinfo.input_components, 
     (unsigned int) 1);
    /* Row 0 of this array contains the row of the output image currently 
       being processed, in JPEG compressor input format.  The array only
       has that one row.
    */
  while (jinfo.next_scanline < h) {
    if (VERBOSE) cerr << "Converting Row %d..." << jinfo.next_scanline << endl;
    VW::PixelMono<float>* in_ptr = image.GetRowStart(jinfo.next_scanline);
    JSAMPLE* out_ptr = buffer[0];
    for (unsigned int column = 0; column < w; column++) {
      *out_ptr++ = translation_table[int(in_ptr++->y)];
    }
    jpeg_write_scanlines(&jinfo, buffer, 1);
  }


  // Done!

  /* Finish compression and release memory */
  jpeg_finish_compress(&jinfo);
  jpeg_destroy_compress(&jinfo);

  /* Program may have exited with non-zero completion code via various function
     calls above.
     */
  return(jerr.num_warnings > 0);
}

bool VW::WriteJPEGImage(FILE* filepointer,
                      const VW::ImageYUV<unsigned char>& image)
{
  if (VERBOSE) cerr << "VW::WriteJPEGImage(YUV)" << endl;
  struct jpeg_compress_struct jinfo;
  struct jpeg_error_mgr jerr;
  const unsigned int w = image.GetWidth(), h =image.GetHeight();
  const J_COLOR_SPACE input_fmt = JCS_YCbCr; // input is YUV
  const J_COLOR_SPACE output_fmt = JCS_YCbCr; // output a YUV jpg
  const unsigned int maxval=255;  // Max number in source image
  JSAMPLE *translation_table;    /* => maxval-remapping array, or NULL */
    /* This is an array that maps each possible pixval in the input to
       a new value such that while the range of the input values is
       0 .. maxval, the range of the output values is 0 .. MAXJSAMPLE.
       */

  SetupOutputJPEG(jinfo, jerr, 
		  w, h, maxval, 
		  input_fmt, output_fmt,
		  filepointer);

  ComputeRescalingArray(&translation_table, maxval, jinfo);

  // Add comment
  std::string comment("Created by VW::WriteJPEGImage(Mono)");
  jpeg_write_marker(&jinfo, JPEG_COM, (const JOCTET*)comment.c_str(), 
		    comment.size());

  // Now do the copying
  JSAMPARRAY buffer = (*jinfo.mem->alloc_sarray)
    ((j_common_ptr) &jinfo, JPOOL_IMAGE,
     (unsigned int) w * jinfo.input_components, 
     (unsigned int) 1);
    /* Row 0 of this array contains the row of the output image currently 
       being processed, in JPEG compressor input format.  The array only
       has that one row.
    */
  while (jinfo.next_scanline < h) {
    if (VERBOSE) cerr << "Converting Row %d..." << jinfo.next_scanline << endl;
    VW::PixelYUV<unsigned char>* in_ptr = image.GetRowStart(jinfo.next_scanline);
    JSAMPLE* out_ptr = buffer[0];
    for (unsigned int column = 0; column < w; column++) {
      *out_ptr++ = translation_table[int(in_ptr->y)];
      *out_ptr++ = translation_table[int(in_ptr->u)];
      *out_ptr++ = translation_table[int(in_ptr->v)];
      ++in_ptr;
    }
    jpeg_write_scanlines(&jinfo, buffer, 1);
  }


  // Done!

  /* Finish compression and release memory */
  jpeg_finish_compress(&jinfo);
  jpeg_destroy_compress(&jinfo);

  /* Program may have exited with non-zero completion code via various function
     calls above.
     */
  return(jerr.num_warnings > 0);
}



bool VW::WriteJPEGImage(FILE* filepointer,
                      const VW::ImageRGB<unsigned char>& image)
{
  if (VERBOSE) cerr << "VW::WriteJPEGImage(RGB)" << endl;
  struct jpeg_compress_struct jinfo;
  struct jpeg_error_mgr jerr;
  const unsigned int w = image.GetWidth(), h =image.GetHeight();
  const J_COLOR_SPACE input_fmt = JCS_RGB; // input is mono
  const J_COLOR_SPACE output_fmt = JCS_YCbCr; // YUV gives best compression
  const unsigned int maxval=255;  // Max number in source image
  JSAMPLE *translation_table;    /* => maxval-remapping array, or NULL */
    /* This is an array that maps each possible pixval in the input to
       a new value such that while the range of the input values is
       0 .. maxval, the range of the output values is 0 .. MAXJSAMPLE.
       */

  SetupOutputJPEG(jinfo, jerr, 
		  w, h, maxval, 
		  input_fmt, output_fmt,
		  filepointer);

  ComputeRescalingArray(&translation_table, maxval, jinfo);

  // Add comment
  std::string comment("Created by VW::WriteJPEGImage(RGB)");
  jpeg_write_marker(&jinfo, JPEG_COM, (const JOCTET*)comment.c_str(), 
		    comment.size());

  // Now do the copying
  JSAMPARRAY buffer = (*jinfo.mem->alloc_sarray)
    ((j_common_ptr) &jinfo, JPOOL_IMAGE,
     (unsigned int) w * jinfo.input_components, 
     (unsigned int) 1);
    /* Row 0 of this array contains the row of the output image currently 
       being processed, in JPEG compressor input format.  The array only
       has that one row.
    */
  while (jinfo.next_scanline < h) {
    if (VERBOSE) cerr << "Converting Row %d..." << jinfo.next_scanline << endl;
    VW::PixelRGB<unsigned char>* in_ptr = image.GetRowStart(jinfo.next_scanline);
    JSAMPLE* out_ptr = buffer[0];
    for (unsigned int column = 0; column < w; column++) {
      *out_ptr++ = translation_table[int(in_ptr->r)];
      *out_ptr++ = translation_table[int(in_ptr->g)];
      *out_ptr++ = translation_table[int(in_ptr->b)];
      ++in_ptr;
    }
    jpeg_write_scanlines(&jinfo, buffer, 1);
  }


  // Done!

  /* Finish compression and release memory */
  jpeg_finish_compress(&jinfo);
  jpeg_destroy_compress(&jinfo);

  /* Program may have exited with non-zero completion code via various function
     calls above.
     */
  return(jerr.num_warnings > 0);
}
