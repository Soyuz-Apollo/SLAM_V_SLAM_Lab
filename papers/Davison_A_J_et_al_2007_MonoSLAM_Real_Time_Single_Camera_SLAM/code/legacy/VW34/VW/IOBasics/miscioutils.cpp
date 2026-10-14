/** @file   miscioutils.cc
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

// System includes
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

// Local includes
#include "miscioutils.h"

#include <iostream>
#include <fstream>

using namespace std;

int VW::FindFileSize(const std::string& filename)
{
  int size=-1;
#ifndef _WIN32
	// POSIX version
  // open the file, seek to the end and use ftell to get the approx size
  FILE *fpin; 
  if((fpin=fopen(filename.c_str(),"r"))!=NULL) {
    fseek(fpin, 0, SEEK_END);
    size=ftell(fpin);
    fclose(fpin);
  }
#else
	// Windows only version
	// open a file handle
	WIN32_FIND_DATA FindFileData;
	HANDLE file = FindFirstFile(filename.c_str(), &FindFileData);
	if (file == INVALID_HANDLE_VALUE) {
		return (-1);
	} 

	// Try to obtain hFile's size 
	DWORD dsize = GetFileSize (file, NULL);
	if (dsize == INVALID_FILE_SIZE) {
		size = -1;
	} else {
		size = int(dsize);
	}
#endif
  return(size);
}

bool VW::FileExists(const std::string& filename)
{
  // see if we can open the file
  FILE *fpin; 
  if((fpin=fopen(filename.c_str(),"r"))!=NULL) {
    fclose(fpin);
    return(true);
  }
  return(false);
}

bool VW::IsADirectory(const std::string& name)
{
#ifndef _WIN32
	// POSIX version
    DIR* directory;
    if ((directory = opendir(name.c_str())) != NULL) {
        closedir(directory);
        return true;
    }
#else
	// Windows version
#endif
    return false;
}

bool VW::GetFileList(const std::string& directoryname,
			  std::vector<std::string>& filenames,
			  bool fullpaths)
{
  filenames.clear();

#ifndef _WIN32
	// POSIX version
  struct dirent **namelist;
  int n;
  

  n = scandir(directoryname.c_str(), &namelist, 0, alphasort);
  if (n < 0) {
    std::cerr << "VW::GetDirectoryList: An error occurred" << std::endl;
    return(false);
  } else {
    for (int i=0;i<n;i++) {
      const std::string fullname = directoryname + std::string("/") + std::string(namelist[i]->d_name);
      // Check if it's a directory
      if (VW::IsADirectory(fullname)) {
        } else {
 	if (fullpaths) {
	  filenames.push_back(fullname);
	} else {
	  filenames.push_back(std::string(namelist[i]->d_name));
	}
      }
      free(namelist[i]);
    }
    free(namelist);
  }
#else
	// Windows only version
	// open a file handle
  WIN32_FIND_DATA c_file;
	HANDLE hFile;
	const string searchname = directoryname + "/*.*";
  if( (hFile = FindFirstFile( searchname.c_str(), &c_file )) != INVALID_HANDLE_VALUE ) {
	  if (c_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
	  // Subdirectory?
	} else {
	if (fullpaths) {
	  filenames.push_back(directoryname + "/" + c_file.cFileName);
	} else {
	  filenames.push_back(c_file.cFileName);
	}
   }
    // findnext
    while( FindNextFile( hFile, &c_file ) ){
		if (c_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		// Subdirectory?
		} else {
	if (fullpaths) {
	  filenames.push_back(directoryname + "/" + c_file.cFileName);
	} else {
	  filenames.push_back(c_file.cFileName);
	}
		}
	}
  }
  FindClose( hFile );
#endif
  return(true);
}



bool VW::GetSubdirectoryList(const std::string& directoryname,
			  std::vector<std::string>& subdirectories,
			  bool fullpaths)
{
#ifndef _WIN32
	// POSIX version
  struct dirent **namelist;
  int n;
  
  subdirectories.clear();

  n = scandir(directoryname.c_str(), &namelist, 0, alphasort);
  if (n < 0) {
    std::cerr << "VW::GetDirectoryList: An error occurred" << std::endl;
    return(false);
  } else {
    for (int i=0;i<n;i++) {
     const std::string fullname = directoryname + std::string("/") + std::string(namelist[i]->d_name);
     if (VW::IsADirectory(fullname)) {
	if (fullpaths) {
	  subdirectories.push_back(fullname);
	} else {
	  subdirectories.push_back(std::string(namelist[i]->d_name));
	}
      }
      free(namelist[i]);
    }
    free(namelist);
  }
#else
	// Windows version
#endif
  return(true);
}

std::string VW::GetFileExtension(const std::string& filename)
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

std::string VW::GetFileBasename(const std::string& filename)
{
  unsigned int len = filename.size();
  bool slashfound=false;
  unsigned int slashpos=0;
  bool dotfound=false;
  unsigned int dotpos=len;

  for (unsigned int pos=len-1;pos>0;pos--) {
    if ((!slashfound) && (filename[pos]=='/')) {
      slashfound = true;
      slashpos=pos;
    }
    if ((!dotfound) && (filename[pos]=='.')) {
      dotfound = true;
      dotpos=pos;
    }
  }

  return(filename.substr(slashpos,dotpos-slashpos));
}

std::string VW::GetFilePath(const std::string& filename)
{
  unsigned int len = filename.size();
  // scan backwards for the '/'
  bool found=false;
  unsigned int pos=0;
  for (pos=len-1;pos>0;pos--)
    {
      if (filename[pos]=='/') {
	found = true;
	break;
      }
    }
  if (found) {
    return(filename.substr(0,pos+1));
  } else {
    return(""); // Slash did not exist
  }
}

std::string VW::StripPath(const std::string& filename)
{
  unsigned int len = filename.size();
  // scan backwards for the '/'
  bool found=false;
  unsigned int pos=0;
  for (pos=len-1;pos>0;pos--)
    {
      if (filename[pos]=='/') {
	found = true;
	break;
      }
    }
  if (found) {
    return(filename.substr(pos+1,len-pos-1));
  } else {
    return(""); // Slash did not exist
  }
}


void VW::SplitFilename(const std::string& filename,
		       std::string& path,
		       std::string& base,
		       std::string& extension)
{
  unsigned int len = filename.size();
  bool slashfound=false;
  int slashpos=-1;
  bool dotfound=false;
  int dotpos=len;

  for (unsigned int pos=len-1;pos>0;pos--) {
    if ((!slashfound) && (filename[pos]=='/')) {
      slashfound = true;
      slashpos=pos;
    }
    if ((!dotfound) && (filename[pos]=='.')) {
      dotfound = true;
      dotpos=pos;
    }
  }


  if (slashfound)
    path = filename.substr(0,slashpos);
  else
    path = "";

  base = filename.substr(slashpos+1,dotpos-slashpos-1);

  if (dotfound)
    extension = filename.substr(dotpos+1,len-dotpos-1);
  else
    extension = "";


}

std::string VW::ReformFilename(const std::string& path,
			       const std::string& base,
			       const std::string& extension)
{
  string filename;
  if (path.size()>0) {
    if (extension.size()>0) {
      filename = path+"/"+base+"."+extension;
    } else {
      filename = path+"/"+base;
    }
  } else {
    if (extension.size()>0) {
      filename = base+"."+extension;
    } else {
      filename = base;
    }
  }
  return filename;
}


bool VW::CopyFile(const std::string& sourcefile,
		  const std::string& destfile)
{
  // Example code from the C++ Programming Language (3rd Edition), p638
  std::ifstream from(sourcefile.c_str());
  if (!from) {
    cerr << "VW::CopyFile: Could not open file \""<<sourcefile
	 << "\" for output." << endl;
    return false;
  }

  std::ofstream to(destfile.c_str());
  if (!to) {
    cerr << "VW::CopyFile: Could not open file \""<<destfile
	 << "\" for input." << endl;
    return false;
  }

  char c;
  while(from.get(c)) to.put(c); // Copy as characters (not fast, but safe)

  // If we stopped before the end of file, something odd happened
  if ((!from.eof()) || (!to)) {
    cerr << "VW::CopyFile: Copying stopped before completion - something odd going on (out of disk space?)." << endl;
    return false;
  }

  return true;
}



// S

 std::ostream& VW::WriteBinary(std::ostream& stream, const char& value)
{
  // cast to a char so that we can write a byte at a time
  const char *ptr = &value;
  for (unsigned int i=0;i<sizeof(value);i++)
    stream << *ptr++;
  return(stream);
}
 std::ostream& VW::WriteBinary(std::ostream& stream, const unsigned char& value)
{
  // cast to a char so that we can write a byte at a time
  const char *ptr = reinterpret_cast<const char*>(&value);
  for (unsigned int i=0;i<sizeof(value);i++)
    stream << *ptr++;
  return(stream);
}
 std::ostream& VW::WriteBinary(std::ostream& stream, const int& value)
{
  // cast to a char so that we can write a byte at a time
  const char *ptr = reinterpret_cast<const char*>(&value);
  for (unsigned int i=0;i<sizeof(value);i++)
    stream << *ptr++;
  return(stream);
}
 std::ostream& VW::WriteBinary(std::ostream& stream, const unsigned int& value)
{
  // cast to a char so that we can write a byte at a time
  const char *ptr = reinterpret_cast<const char*>(&value);
  for (unsigned int i=0;i<sizeof(value);i++)
    stream << *ptr++;
  return(stream);
}
 std::ostream& VW::WriteBinary(std::ostream& stream, const float& value)
{
  // cast to a char so that we can write a byte at a time
  const char *ptr = reinterpret_cast<const char*>(&value);
  for (unsigned int i=0;i<sizeof(value);i++)
    stream << *ptr++;
  return(stream);
}
 std::ostream& VW::WriteBinary(std::ostream& stream, const double& value)
{
  // cast to a char so that we can write a byte at a time
  const char *ptr = reinterpret_cast<const char*>(&value);
  for (unsigned int i=0;i<sizeof(value);i++)
    stream << *ptr++;
  return(stream);
}

std::ostream& VW::WriteBinary(std::ostream& stream, const double* values, unsigned int n)
{
  // cast to a char so that we can write a byte at a time
  const unsigned int num_bytes = sizeof(values[0])*n;
  const char *ptr = reinterpret_cast<const char*>(values);
  for (unsigned int i=0;i<num_bytes;i++)
    stream << *ptr++;
  return(stream);
}

std::ostream& VW::WriteBinary(std::ostream& stream, const float* values, unsigned int n)
{
  // cast to a char so that we can write a byte at a time
  const unsigned int num_bytes = sizeof(values[0])*n;
  const char *ptr = reinterpret_cast<const char*>(values);
  for (unsigned int i=0;i<num_bytes;i++)
    stream << *ptr++;
  return(stream);
}




 std::istream& VW::ReadBinary(std::istream& stream, char& value)
{
  // cast to a char so that we can write a byte at a time
  char *ptr = &value;
  char c;
  for (unsigned int i=0;i<sizeof(value);i++) {
    stream.get(c);
    *ptr++=c;
  }
  return(stream);
}
 std::istream& VW::ReadBinary(std::istream& stream, unsigned char& value)
{
  // cast to a char so that we can write a byte at a time
  char *ptr = reinterpret_cast<char*>(&value);
  char c;
  for (unsigned int i=0;i<sizeof(value);i++) {
    stream.get(c);
    *ptr++=c;
  }
  return(stream);
}
 std::istream& VW::ReadBinary(std::istream& stream, int& value)
{
  // cast to a char so that we can write a byte at a time
  char *ptr = reinterpret_cast<char*>(&value);
  char c;
  for (unsigned int i=0;i<sizeof(value);i++) {
    stream.get(c);
    *ptr++=c;
  }
  return(stream);
}
 std::istream& VW::ReadBinary(std::istream& stream, unsigned int& value)
{
  // cast to a char so that we can write a byte at a time
  char *ptr = reinterpret_cast<char*>(&value);
  char c;
  for (unsigned int i=0;i<sizeof(value);i++) {
    stream.get(c);
    *ptr++=c;
  }
  return(stream);
}
std::istream& VW::ReadBinary(std::istream& stream, float& value)
{
  // cast to a char so that we can write a byte at a time
  char *ptr = reinterpret_cast<char*>(&value);
  char c;
  for (unsigned int i=0;i<sizeof(value);i++) {
    stream.get(c);
    *ptr++=c;
  }
  return(stream);
}
std::istream& VW::ReadBinary(std::istream& stream, double& value)
{
  // cast to a char so that we can write a byte at a time
  char *ptr = reinterpret_cast<char*>(&value);
  char c;
  for (unsigned int i=0;i<sizeof(value);i++) {
    stream.get(c);
    *ptr++=c;
  }
  return(stream);
}

std::istream& VW::ReadBinary(std::istream& stream, double* values, unsigned int n)
{
  // cast to a char so that we can write a byte at a time
  const unsigned int num_bytes = sizeof(values[0])*n;
  char *ptr = reinterpret_cast<char*>(values);
  char c;
  for (unsigned int i=0;i<num_bytes;i++) {
    stream.get(c);
    *ptr++=c;
  }
  return(stream);
}
std::istream& VW::ReadBinary(std::istream& stream, float* values, unsigned int n)
{
  // cast to a char so that we can write a byte at a time
  const unsigned int num_bytes = sizeof(values[0])*n;
  char *ptr = reinterpret_cast<char*>(values);
  char c;
  for (unsigned int i=0;i<num_bytes;i++) {
    stream.get(c);
    *ptr++=c;
  }
  return(stream);
}
