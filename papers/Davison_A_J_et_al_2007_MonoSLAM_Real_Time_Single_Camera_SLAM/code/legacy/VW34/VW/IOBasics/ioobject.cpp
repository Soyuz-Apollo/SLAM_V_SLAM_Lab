/** @file   ioobject.cpp
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

#include "ioobject.h"

// System includes
#include <fstream>

using namespace std;



/** Open a file and output as plain ASCII
 * (overwriting any existing contents) .
 *
 * Once the file is open, the child class's WriteASCII function is used to
 * create the content.
 *
 * @param filename the name of the file to create (or overwrite if it exists)
 * @return true for success, false if file cannot be opened or the data 
 *         can't be written.
 */
bool VW::IOObject::WriteASCIIFile(const std::string& filename) const
{
  ofstream file(filename.c_str());
  if (!file) {
    cerr << "VW::IOObject::WriteASCIIFile - Could not open file \""
	 << filename << "\"." << endl;
    return(false);
  }
  this->WriteASCII(file);
  file.close();
  return(true);
}



/** Open a file and output as XML-style ASCII
 * (overwriting any existing contents) .
 *
 * Once the file is open, the child class's WriteXML function is used to
 * create the content.
 *
 * @param filename the name of the file to create (or overwrite if it exists)
 * @param name     the tag parameter, if required
 * @return true for success, false if file cannot be opened or the data 
 *         can't be written.
 */
bool VW::IOObject::WriteXMLFile(const std::string& filename,
			    const std::string& name) const
{
  ofstream file(filename.c_str());
  if (!file) {
    cerr << "VW::IOObject::WriteXMLFile - Could not open file \""
	 << filename << "\"." << endl;
    return(false);
  }
  this->WriteXML(file,name);
  file.close();
  return(true);
}



/** Open a file and output as binary
 * (overwriting any existing contents). 
 *
 * Once the file is open, the child class's WriteBinary function is used to
 * create the content.
 *
 * @param filename the name of the file to create (or overwrite if it exists)
 * @return true for success, false if file cannot be opened or the data 
 *         can't be written.
 */
bool VW::IOObject::WriteBinaryFile(const std::string& filename) const
{
  ofstream file(filename.c_str());
  if (!file) {
    cerr << "VW::IOObject::WriteBinaryFile - Could not open file \""
	 << filename << "\"." << endl;
    return(false);
  }
  this->WriteBinary(file);
  file.close();
  return(true);
}



/** Open a file and read as plain ASCII.
 *
 * Once the file is open, the child class's ReadASCII function is used to
 * read the content.
 *
 * @param filename the name of the file to open
 * @return true for success, false if file cannot be opened or the data 
 *         isn't read successfully.
 */
bool VW::IOObject::ReadASCIIFile(const std::string& filename)
{
  ifstream file(filename.c_str());
  if (!file) {
    cerr << "VW::IOObject::ReadASCIIFile - Could not open file \""
	 << filename << "\"." << endl;
    return(false);
  }
  this->ReadASCII(file);
  file.close();
  return(true);
}



/** Open a file and read as XML-style ASCII.
 *
 * Once the file is open, the child class's ReadXML function is used to
 * read the content.
 *
 * @param filename the name of the file to open
 * @param name     the optional name tag-attribute to match
 *                 ie. data must start <MyClass Name="name">
 * @return true for success, false if file cannot be opened or the data 
 *         isn't read successfully.
 */
bool VW::IOObject::ReadXMLFile(const std::string& filename,
			       const std::string& name)
{
  ifstream file(filename.c_str());
  if (!file) {
    cerr << "VW::IOObject::ReadXMLFile - Could not open file \""
	 << filename << "\"." << endl;
    return(false);
  }
  this->ReadXML(file,name);
  file.close();
  return(true);
}



/** Open a file and read as binary.
 *
 * Once the file is open, the child class's ReadBinary function is used to
 * read the content.
 *
 * @param filename the name of the file to open
 * @return true for success, false if file cannot be opened or the data 
 *         isn't read successfully.
 */
bool VW::IOObject::ReadBinaryFile(const std::string& filename)
{
  ifstream file(filename.c_str());
  if (!file) {
    cerr << "VW::IOObject::ReadBinaryFile - Could not open file \""
	 << filename << "\"." << endl;
    return(false);
  }
  this->ReadBinary(file);
  file.close();
  return(true);
}
