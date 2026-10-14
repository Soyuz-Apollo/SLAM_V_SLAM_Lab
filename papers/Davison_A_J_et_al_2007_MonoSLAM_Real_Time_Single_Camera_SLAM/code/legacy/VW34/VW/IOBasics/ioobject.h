/** @file   ioobject.h 
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
/**
 * @class   VW::IOObject 
 * @ingroup IOBasics 
 * @author  BJ Tordoff
 * @date    22/10/02
 * @brief   Abstract base class for objects that can be input/output in a 
 *          standard (streaming) way.
 *
 * This abstract class forces inheriting sub-classes to implement
 * a set of streaming functions, providing input and output in binary, ASCII
 * and XML-style ASCII formats. In addition it provides basic file versions
 * of these functions that do not need to be overloaded, and maps the standard
 * streaming operation to the ASCII functions.
 */


// NB: Detailed comments in the .cc file


#ifndef _ioobject_h_
#define _ioobject_h_

// System includes
#include <string>
#include <iostream>


namespace VW {

class IOObject
{
public:
  virtual ~IOObject() {}

  ///// OUTPUT /////

  /** Open a file and output as plain ASCII 
   * (overwriting any existing contents) */
  bool WriteASCIIFile(const std::string& filename) const;

  /** Open a file and output as XML-style ASCII 
   * (overwriting any existing contents). */
  bool WriteXMLFile(const std::string& filename,
		    const std::string& name="") const;

  /** Open a file and output as binary
   * (overwriting any existing contents) */
  bool WriteBinaryFile(const std::string& filename) const;



  /** Stream as plain ASCII. 
   * @param stream the output stream to write to
   * @return true if successfully written, false if there are problems
   */
  virtual 
  bool WriteASCII(std::ostream& stream) const = 0; // Equivelent to '<<'

  /** Stream as XML style ASCII.
   * @param stream the output stream to write to
   * @param name if specified, the class will be given a NAME= parameter
   * @return true if successfully written, false if there are problems
   */
  virtual 
  bool WriteXML(std::ostream& stream, 
		const std::string& name="") const = 0;

  /** Stream as binary.
   * @param stream the output stream to write to
   * @return true if successfully written, false if there are problems
   */
  virtual 
  bool WriteBinary(std::ostream& stream) const = 0;





  ///// INPUT /////



  /** Open a file and input as plain ASCII. */
  virtual 
  bool ReadASCIIFile(const std::string& filename);

  /** Open a file and input as XML style ASCII. */
  virtual 
  bool ReadXMLFile(const std::string& filename, 
			    const std::string& name="");
  /** Open a file and input as binary. */
  virtual 
  bool ReadBinaryFile(const std::string& filename);



  /** Stream as plain ASCII.
   * @param stream the input stream to read from
   * @return true if successfully read, false if there are problems
   */
  virtual 
  bool ReadASCII(std::istream& stream) = 0; // Equivelent to '<<'

  /** Stream as XML style ASCII.
   * @param stream the input stream to read from
   * @param name if specified, the found tag must have a NAME= parameter
   *             that matches
   * @return true if successfully read, false if there are problems
   */
  virtual 
  bool ReadXML(std::istream& stream, 
	       const std::string& name="") = 0;
  /** Stream as binary.
   * @param stream the input stream to read from
   * @return true if successfully read, false if there are problems
   */
  virtual 
  bool ReadBinary(std::istream& stream) = 0;
  

protected:

private:
};

}; // End of VW namespace


namespace std {

inline
std::ostream& operator<<(std::ostream& stream, const VW::IOObject& obj)
{
  obj.WriteASCII(stream);
  return (stream);
}

inline
std::istream& operator>>(std::istream& stream, VW::IOObject& obj)
{
  obj.ReadASCII(stream);
  return (stream);
}

}; // End of std namespace


#endif // #ifndef _ioobject_h_
