/** @file   miscioutils.h 
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

#ifndef _miscioutils_h_
#define _miscioutils_h_

#include <vector>
#include <string>
#include <iostream>


namespace VW {

  /** Determine whether the specified file exists. */
  bool FileExists(const std::string& filename);

  /** Find the size of a file.
   * Returns size in bytes, or zero if no file found.
   */
  int FindFileSize(const std::string& filename);

    /** Is this a directory (true) or a file (false). */
    bool IsADirectory(const std::string& name);

    /** Get the list of files in a directory. */
  bool GetFileList(const std::string& directoryname,
		   std::vector<std::string>& filenames,
		   bool fullpaths = true);

  /** Get the list of subdirectories of a directory. */
  bool GetSubdirectoryList(const std::string& directoryname,
			std::vector<std::string>& subdirectories,
			bool fullpaths = true);


  // Some filename manipulators
  // A filename can be split into path+'/'+base+'.'+extension

  /** Split out the path from a fully qualified filename (not including
   * the final '/').
   */
  std::string GetFilePath(const std::string& filename);

  /** Recover the base of a filename (ie everything between the last '/'
   * and the last dot).
   */
  std::string GetFileBasename(const std::string& filename);

  /** Recover the suffix of a filename (ie everything up to the last dot). 
   * If the dot is the last character, or doesn't exist, an empty string 
   * is returned.
   */
  std::string GetFileExtension(const std::string& filename);

  /** Remove the path component of the filename. */
  std::string StripPath(const std::string& filename);


  /** Split a filename into path+'/'+base+'.'+extension. */
  void SplitFilename(const std::string& filename,
		     std::string& path,
		     std::string& base,
		     std::string& extension);
  /** Refrom a filename from its path, base and extension. */
  std::string ReformFilename(const std::string& path,
			     const std::string& base,
			     const std::string& extension);


  /** Copy a file from one location to another (binary). */
  bool CopyFile(const std::string& sourcefile,
		const std::string& destfile);
		


  /** Convert a standard type to a binary string */
    std::ostream& WriteBinary(std::ostream& stream, const char& value);
    std::ostream& WriteBinary(std::ostream& stream, const unsigned char& value);
    std::ostream& WriteBinary(std::ostream& stream, const int& value);
    std::ostream& WriteBinary(std::ostream& stream, const unsigned int& value);
    std::ostream& WriteBinary(std::ostream& stream, const float& value);
   std::ostream& WriteBinary(std::ostream& stream, const double& value);
  /** Write an array of n entries */
  std::ostream& WriteBinary(std::ostream& stream, const double* values, unsigned int n);
  std::ostream& WriteBinary(std::ostream& stream, const float* values, unsigned int n);

  /** Read a binary string and convert to a standard type */
   std::istream& ReadBinary(std::istream& stream, char& value);
   std::istream& ReadBinary(std::istream& stream, unsigned char& value);
   std::istream& ReadBinary(std::istream& stream, int& value);
   std::istream& ReadBinary(std::istream& stream, unsigned int& value);
   std::istream& ReadBinary(std::istream& stream, float& value);
   std::istream& ReadBinary(std::istream& stream, double& value);
   std::istream& ReadBinary(std::istream& stream, double* values, unsigned int n);
   std::istream& ReadBinary(std::istream& stream, float* values, unsigned int n);




}
#endif
