/** 
 * @file   xmlstuff.h 
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


#ifndef _xmlstuff_h_
#define _xmlstuff_h_

//#include <sys/time.h>

#include <sstream>
#include <string>
#include <vector>




namespace VW {

/* The misc namespace - a namespace for miscalleaneous objects (no really).
 *
 * Random bits of handy code, including XML style file IO and some 
 * random number stuff.
 */


  /** Find an XML block and leave the stream pointing to its contents.
   * The input stream is scanned for the <name> block
   * but the contents are not read. Use this where the block contents
   * may be large, otherwise just read the block into a string using
   * the functions which follow, as this gives better error protection.
   */
  bool FindXMLBlock(const std::string& block_name,
		      std::istream& input_stream,
		      std::string& name);

  /** Find an XML block and read its contents into a string.
   * The input stream is scanned for a <name>...</name> block
   * and the contents returned. Note that if the block contents are
   * likely to be large, the VW::FindXMLBlock function should be used.
   */
  bool GetXMLBlock(const std::string& block_name,
		   std::istream& input_stream,
		   std::istringstream& contents,
		   std::string& name);

  /** Find an XML block and return its contents.
   * The input string is scanned for a <name>...</name> block
   * and the contents returned. This version takes a string as input,
   * but otherwise behaves exactly as the stream version. Note that if 
   * the block contents are likely to be large, the VW::FindXMLBlock 
   * function should be used.
   */
  bool GetXMLBlock(const std::string& block_name,
		   const std::string& input_string,
		   std::string& contents,
		   std::string& name);

  /** Find an XML block and return its contents as a double precision number.
   */
  bool GetXMLBlockAsNumber(const std::string& block_name,
			   std::istream& input_string,
			   double& contents,
			   std::string& name);

  /** Find an XML block and return its contents as a floating point number.
   */
  bool GetXMLBlockAsNumber(const std::string& block_name,
			   std::istream& input_string,
			   float& contents,
			   std::string& name);

  /** Find an XML block and return its contents as an unsigned integer number.
   */
  bool GetXMLBlockAsNumber(const std::string& block_name,
			   std::istream& input_string,
			   int& contents,
			   std::string& name);

  /** Find an XML block and return its contents as an unsigned integer number.
   */
  bool GetXMLBlockAsNumber(const std::string& block_name,
			   std::istream& input_string,
			   unsigned int& contents,
			   std::string& name);

  /** Find all XML blocks matching the given tag and return their contents.
   * The input string is scanned for any <name>...</name> blocks
   * and the contents returned for each along with the name (if any).
   */
  bool GetMultipleXMLBlocks(const std::string& block_name,
			    std::istream& input_string,
			    std::vector<std::string>& contents,
			    std::vector<std::string>& names);



  /** Skip forwards to the next XML-style tag.
   * Intermediate material is discarded, and if no tag is found before
   * the end of the input the result is false. When a tag is found, the 
   * result is true, the tag name is returned in 'tagname' and any arguments
   * found are returned in 'arguments'.
   */
  bool FindNextXMLTag(std::istream& input_file,
		      std::string& tagname,
		      std::string& arguments);



  // Old stuff



  /** Read a string into a value.
   * For simple values this is just a << operation, but can be
   * overloaded for complex data-types to include parsing of
   * XML marked members.
   */
  template <class T>
  bool ReadXMLString(const std::string& str, T& data) {
    std::istringstream ist (str);
    ist >> data;
    // Should check for errors and return success, but don't know how
    return(true);
  }

  /** Generate an XML string for a type.
   * For simple values this is just a >> operation, but can be
   * overloaded for complex data-types to include creation of
   * XML marked member-types.
   */
  template <class T> 
  std::string GenerateXMLString(const T& data, unsigned int indentation=0) {
    std::ostringstream s;
    for (unsigned int i=0;i<indentation;i++) s << ' ';
    s << data;
    return(s.str());
  }



}; // End of namespace VW



  
#endif // #ifndef _xmlstuff_h_
