/** 
 * @file   paramparser.h  
 * @author JGH Knight
 * @date   02/08/02
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
 * @class ParameterParser
 * @author JGH Knight
 * @date   02/08/02
 *
 * @brief A simple XML-style parser for interpreting files of parameter data
 * for compute objects like corner detectors and RANSAC. 
 *
 * I expect it will
 * be modified a lot, possibly using libraries for proper XML parsing,
 * but hopefully the same essential structure of the files will remain.
 * see paramfiles.README.
 */


#ifndef _paramparser_h_
#define _paramparser_h_

// System includes
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

// Local includes
#include "xmlstuff.h"

namespace VW {


// Forward declaration needed
class ParameterWriter;

class ParameterParser
{
  // Friend class
  friend class ParameterWriter;

  // Exceptions
  class ParseErr {
  public:
    ParseErr (std::string err_string = "") : _error_string (err_string) {}
    std::string _error_string;
  };
  class EOFErr : public ParseErr { };


public:
  /** Constructor for reading just the low-level block 'compute_obj' from a file. */
  ParameterParser (std::string compute_obj, std::string filename = ".params");
  /** Constructor for reading just the low-level block 'compute_obj' from a file. */
  ParameterParser (std::string compute_obj, std::istream& file_stream);
  template <class T>
  ParameterParser (std::string compute_obj, const std::string parameter[],
		   T value[], unsigned int num_params,
		   std::string filename = ".params");

  ~ParameterParser();

  // Extract info from parsed file
  //   Extract arrays that match provided parameter names, unchanged if not there
  template <class T>
  void Extract (const std::string parameter[], 
		T value[], 
		unsigned int num_params) const;

  //   Get hold of requested parameter.  Returns false if parameter isn't there,
  //   and leaves it unchanged
  template <class T>
  inline bool GetValue (std::string parameter,
			T& value) const;

  //   Read low-level block into this parameter.  Returns false if parameter isn't there,
  //   and leaves it unchanged
  template <class T>
  inline bool GetValue (T& value) const;

  //   Check using `if (parser_object)' -- returns false if file ill-formed
  operator bool () const {return _Ok;}

  // Get Data directly
  const std::map<std::string, std::string>& Parameters () 
    {
      return _parameters;
    }

  // Set Verbose to true to locate file parsing errors
  bool Verbose;

protected:
  // Data
  std::istream *_file;
  std::string _block_contents;
  bool _Ok;
  bool _I_opened_this_stream;
  std::map<std::string, std::string> _parameters;

private:
  // Utilities
  //   Parse levels
  bool parse_file (std::istream& input_file, std::string compute_obj_name);
  bool parse_parameter_list (std::istream& input_file, 
			     std::string compute_obj_name);
  void parse_parameter (std::istream& input_file, 
			const std::string param_name);

  /** Return everything between <elementname> ... </elementname>. */
  std::string parse_basic(std::istream& input_file, 
				      const std::string elementname);

  // Fundamental
  std::string get_next_label (std::istream& input_file);  // Get next label (between < and >)
  void print_parse_error (std::istream& input_file, 
			  ParseErr error);

};

}; // End of VW namespace



//   Read low-level block into this parameter.
template <class T>
bool VW::ParameterParser::GetValue (T& value) const {
  return (VW::ReadXMLString<T>(_block_contents, value));
}

#endif // #ifndef _paramparser_h_
