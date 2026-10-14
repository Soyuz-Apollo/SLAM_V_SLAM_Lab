/** @file imageio_linux.cpp
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



// DO NOT COMPILE THIS FILE DIRECTLY - IT IS INCLUDED BY IMAGEIO.CPP



extern "C" {
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h> // for close(fd)
}


#include <string>
#include <iostream>

#include "imageio.h"
#include "imageio_ppm.tpp"


#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false


using namespace std;
using namespace VW;

enum ImageFileType { 
  NONE=0,
  PGM, 
  PPM, 
  JPEG, 
  PNG, 
  GIF, 
  MIT, 
  TIFF, 
  POSTSCRIPT, 
  UYVY
};



class IOFilter {
 public:
  IOFilter(char* suffix,
	   char* inputfilter, 
	   char* outputfilter,
	   ImageFileType filetype) 
    : _suffix(suffix), 
      _infilter(inputfilter), 
      _outfilter(outputfilter),
      _filetype(filetype)
  {
  }

  char *_suffix, *_infilter, *_outfilter;
  ImageFileType _filetype;
};


static const IOFilter iofilters[] = {
  IOFilter( "pgm",        NULL,        NULL, PGM),
  IOFilter( "ppm",        NULL,        NULL, PPM),
  IOFilter("jpeg", "jpegtopnm", "ppmtojpeg", JPEG),
  IOFilter( "jpg", "jpegtopnm", "ppmtojpeg", JPEG),
  IOFilter( "png",  "pngtopnm",  "pnmtopng", PNG),
  IOFilter( "gif",  "giftopnm",  "ppmtogif", GIF),
  IOFilter("tiff", "tifftopnm", "pnmtotiff", TIFF),
  IOFilter( "tif", "tifftopnm", "pnmtotiff", TIFF),
  IOFilter( "mit",   "mit2pnm",  "pnmtomit", MIT),
  IOFilter(  "ps",    "ps2pnm",   "pnmtops", POSTSCRIPT),
  IOFilter( "eps",    "ps2pnm",   "pnmtops", POSTSCRIPT),
  IOFilter("epsi",    "ps2pnm",   "pnmtops", POSTSCRIPT),
  IOFilter( "yuv",        NULL,        NULL, UYVY),
  IOFilter(NULL,          NULL,        NULL, NONE)
};


// Case insensitive comparison of strings, from Stroustrup p591
static int CaseInsensitiveCompare(const string& s1, const string& s2)
{
  string::const_iterator p1 = s1.begin();
  string::const_iterator p2 = s2.begin();
  const string::const_iterator pe1 = s1.end();
  const string::const_iterator pe2 = s2.end();
  while (p1!=pe1 && p2!=pe2) {
    const char c1 = toupper(*p1);
    const char c2 = toupper(*p2);
    if (c1 != c2) return (c1<c2) ? -1 : 1;
    ++p1;
    ++p2;
  }
  const unsigned int sz1 = s1.size();
  const unsigned int sz2 = s2.size();
  return (sz2==sz1) ? 0 : (sz1<sz2) ? -1 : 1;
}


/** Find the file extension (suffix) of a file, if one exists. */
string _FindSuffix(const string& filename)
{
 unsigned int len = filename.size();
  // scan backwards for the '.'
  bool found=false;
  unsigned int pos=0;
  for (pos=len-1;pos>0;pos--) {
    if (filename[pos]=='.') {
      found = true;
      break;
    }
  }

  // now check for problems
  if (!found) {
    return(""); // Dot did not exist
  }
  // Was found, but is it empty?
  const int num_chars = len-pos-1;
  if (num_chars <= 0) {
      return(""); // dot was last char, so return an empty string
  }

  // It was OK, so return the remainder
  return(filename.substr(pos+1,num_chars));

}

/** Use the args supplied to determine if conversion/decompression is required
 * and return a file descriptor for reading from the decrypted output.
 * (in the case that the input file is a pnm image, the pointer is left
 * unchanged).
 */
static int _iolin_StartInputFilter(int fdin, char * const args[], int& pid)
{
  int fd[2];
	
  if (args==NULL || args[0]==NULL) {
    if (VERBOSE) cerr << "StartInputFilter(): no args." << endl;
    return fdin;
  }
	
	
  if (pipe(fd)) {
    if (VERBOSE) cerr << "StartInputFilter(): pipe failed\n";
    close(fdin);
    return -1;
  }
	
  if ((pid=fork())<0) {
    if (VERBOSE) cerr << "StartInputFilter(): fork failed\n";
    close(fdin);
    return -1;
  }
    
  if (!pid) {
    /* child */
    if (fdin != -1) {
      if (dup2(fdin,0) == -1) {
	if (VERBOSE) cerr << "Couldn't dup file descriptor for image input filter\n";
	std::exit(1);
      }
      close(fdin);
    }
		
    if (dup2(fd[1],1) == -1) {
      if (VERBOSE) cerr << "Couldn't dup file descriptor for image input filter\n";
      std::exit(1);
    }
    close(fd[0]);
    if (VERBOSE) cerr << "Starting input filter: "<<args[0]<<endl;
    execvp(args[0], args);
  }
    
  /*parent*/
  close(fd[1]);
  close(fdin);
  return fd[0];	
}

static int _iolin_StartOutputFilter(int fdout, char * const args[], int& pid)
{
  int fd[2];
	
  if (args==NULL || args[0]==NULL) {
    if (VERBOSE) cerr << "ImageIO::StartOutputFilter(): no filter required." << endl;
    return fdout;
  }
	
  if (pipe(fd))
    return -1;
  if ((pid=fork())<0) 
    return -1;
    
  if (!pid) {
    /* child */
    if (dup2(fdout,1) == -1) {
      if (VERBOSE) cerr << "ImageIO::StartOutputFilter(): Couldn't dup file descriptor for image output filter\n";
      return -1;
    }
    close(fdout);
		
    if (dup2(fd[0],0) == -1) {
      if (VERBOSE) cerr << "ImageIO::StartOutputFilter(): Couldn't dup file descriptor for image output filter\n";
      return -1;
    }
    close(fd[1]);
    if (VERBOSE) cerr << "Doing exec of filter\n";
    if (execvp(args[0], args) == -1) {
      cerr << "ImageIO::StartOutputFilter: Exec of filter failed\n";
      std::exit(1);
    }
  }
    
  /*parent*/
  close(fd[0]);
  close(fdout);
  return fd[1];
}



static FILE* _iolin_OpenInputFile(const std::string& filename)
{
  FILE *fp;
  int fd, pid;
  char* args[2];

  // Flags are OS dependant
  const int fileflags = O_RDONLY;
	
  // Open file
  if ( ( fd = open ( filename.c_str(), fileflags ) ) == -1 ) {
    if (VERBOSE) cerr << "ImageIO::OpenInputFile(): Cannot read from file " 
                      << filename << "\n";
    return NULL;
  }
  // File is open, so call the appropriate function then close it
  string suffix = _FindSuffix(filename);
  if ((CaseInsensitiveCompare(suffix,"gz")==0)
      || (CaseInsensitiveCompare(suffix,"Z")==0)) {
    if (VERBOSE) cerr << "ImageIO: decompressing using zcat (suffix is \""<<suffix<<"\")." << endl;
    args[0] = "zcat";
    args[1] = NULL;
    if ((fd=_iolin_StartInputFilter(fd, args, pid)) == -1) {
      if (VERBOSE) cerr << "ImageIO::OpenInputFile(): couldn't start decompression filter\n";
      return NULL;
    }
  }
  suffix = _FindSuffix(filename);

  // check list of known suffixes and start appropriate input filter
  int i=0;
  for (i=0; iofilters[i]._suffix!=NULL; i++) {
    if ((CaseInsensitiveCompare(suffix, iofilters[i]._suffix)==0))
      break;
  }
    if (VERBOSE) cerr << "ImageIO: got filter "<<iofilters[i]._suffix<<"." << endl;
  if (iofilters[i]._suffix==NULL) {
    if (VERBOSE) cerr << "ImageIO::OpenInputFile: Could not match file extension '" << suffix << "' to a known file type, possibilities are:\n\t";
    for (i=0; iofilters[i]._suffix!=NULL; i++) {
      cerr << iofilters[i]._suffix << ", ";    
    }
    cerr << endl;    
    close(fd);
    return NULL;
  }
  args[0] = iofilters[i]._infilter;
  args[1] = NULL;
  if (VERBOSE) cerr << "ImageIO::OpenInputFile(): starting filter with args " << args[0] << "\n";
  if ((fd=_iolin_StartInputFilter(fd, args, pid)) == -1) {
    if (VERBOSE) cerr << "ImageIO::OpenInputFile(): couldn't start input filter\n";
    return NULL;
  }
	
  // And now get a stream for this file descriptor for easier IO
  if ((fp=fdopen(fd, "r")) == NULL) {
    if (VERBOSE) cerr << "ImageIO::OpenInputFile(): Cannot get stream from file descriptor for " << filename << "\n";
    close(fd);
    return NULL;
  }
  return fp;
}

static FILE* _iolin_OpenOutputFile(const std::string& filename)
{
  FILE *fp;
  int fd, pid;
  char* args[2];
	
  // Flags are OS dependant
  const int access_type = O_WRONLY | O_CREAT;
  const int mode_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	
  // Open file
  if ( ( fd = open ( filename.c_str(), access_type, mode_flags ) ) == -1 ) {
    if (VERBOSE) cerr << "ImageIO::OpenOutputFile(): Cannot write to file " << filename << "\n";
    return NULL;
  }
	
  // strip suffix and test to see if file it suggests file should be compressed
  string suffix = _FindSuffix(filename);
  if ((CaseInsensitiveCompare(suffix,"gz")==0)
       || (CaseInsensitiveCompare(suffix,"Z")==0)) {
    args[0] = "gzip";
    args[1] = NULL;
    fd = _iolin_StartOutputFilter(fd, args, pid);
    suffix = _FindSuffix(filename);
  }
    
  // check list of known suffixes and start appropriate output filter
  int i=0;
  for (i=0; iofilters[i]._suffix!=NULL; i++) {
    if (CaseInsensitiveCompare(suffix, iofilters[i]._suffix)==0)
      break;
  }
  if (iofilters[i]._suffix==NULL) {
    if (VERBOSE) cerr << "ImageIO::OpenOutputFile(): Could not match file extension '" << suffix << "' to a known file type\n";
    return NULL;
  }
	
  args[0] = iofilters[i]._outfilter;
  args[1] = NULL;
  fd = _iolin_StartOutputFilter(fd, args, pid);
	
  // And now get a stream for this file descriptor for easier IO
  if ((fp=fdopen(fd, "w")) == NULL) {
    if (VERBOSE) cerr << "ImageIO::OpenOutputFile(): Cannot get stream from file descriptor for " << filename << "\n";
    return NULL;
  }
	
  return fp;
}


static void CloseFile(FILE* file_pointer)
{
    fclose(file_pointer);
}


// Now the real stuff!
template <class ImType>
bool VW::ReadImage(FILE *file, ImType& image)
{
  bool success;
  success = VW::ReadPPMImage(file,image);
  return success;
}


template <class ImType>
bool VW::ReadImage(const std::string& filename,
		   ImType& image)
{
  FILE* file;
  bool success = true;
  
  if (!(file = _iolin_OpenInputFile(filename))) {
    success = false;
  } else {
    success = VW::ReadPPMImage(file,image);
    CloseFile(file);
  }

  return success;
}
  

template <class ImType>
bool VW::WriteImage(FILE *file,
		    const ImType& image)
{
  bool success;
  success = VW::WritePPMImage(file,image);
  return success;
}


template <class ImType>
bool VW::WriteImage(const std::string& filename,
		    const ImType& image)
{
 FILE* file;
  bool success = true;
  
  if (!(file = _iolin_OpenOutputFile(filename))) {
    success = false;
  } else {
     success = VW::WritePPMImage(file,image);
    CloseFile(file);
 
  }
  return success;
}
