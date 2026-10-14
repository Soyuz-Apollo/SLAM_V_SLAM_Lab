/** 
 * @file   paramwriter.h
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

#ifndef _paramwriter_h_
#define _paramwriter_h_

#include <map>
#include <string>
#include <sstream>
//#include "misc.h"
#include "paramparser.h"

/** ParameterWriter
 *
 * Write out a parameter list definition to a parameter file, as defined
 * in paramfiles.README
 */

namespace VW {

class ParameterWriter
{
  // Exceptions
  class WriterErr {
  public:
    WriterErr (std::string err_string = "") : _error_string (err_string) {}
    std::string _error_string;
  };
  class EOFErr : public WriterErr { };


public:
  /** Basic constructor.
   * Compute_obj name determines the name of the lowest-level block 
   * to find in the file. Once found, parameters can be added to be
   * either written directly into the block (AddParameter), or
   * added between a <param>...</param> pair (AddChildParam).
   */
  ParameterWriter (std::string compute_obj, 
		   std::string filename = ".params");

  /** All-in-one constructor.
   * Compute_obj name determines the name of the lowest-level block 
   * to find in the file. Once found, the parameters listed in 
   * parameter[] are added as children (ie. between <param>...</param> pairs).
   * In addition, the file is written to disk, allowing one-line
   * parameter writing.
   */
  template <class T>
  ParameterWriter (std::string compute_obj, 
		   const std::string parameter[],
		   const T value[], 
		   unsigned int num_params,
		   std::string filename = ".params");
  
  // Methods for adding parameters:

  /** Add a value to the base block. 
   * The parameter value is <B>NOT</B> enclosed by a <parameter> ... 
   * </parameter> pair, and the GenerateXMLString(T) is called at
   * the lowest level.
   */
  template <class T>
  void AddParameter (const T& value);

  /** Add a single parameter/value pair to the list. 
   * The parameter value is enclosed by a <parameter> ... </parameter>
   * pair.
   */
  template <class T>
  void AddChildParameter (const std::string& parameter, 
			  const T& value);

  /** Add multiple parameter/value pairs (of the same type) to the list.
   * Each parameter value is enclosed by a <parameter> ... </parameter>
   * pair.
   */
  template <class T>
  void AddChildParameters (const std::string parameter[], 
		      const T value[],
		      unsigned int num_params);

  /** Flush the stored parameters to disk. */
  void WriteOut ();

  //   Check using `if (writer_object)' -- returns false if file ill-formed
  operator bool () const {return _Ok;}

protected:
  // FUNCTIONS
  void write_param_list (std::ostream& output_stream);
  // Fundamental
  std::string get_next_label ();  // Get next label (between < and >)
  void print_writer_error (WriterErr exception);
  void copy_file_to_stream(std::string filename,
			   std::streampos start_position,
			   std::streampos end_position,
			   std::ostream& output_stream);

  // DATA
  // A parser object to find the current parameter list definition
  ParameterParser _parser;

  std::map<std::string, std::string> _parameters;
  std::string _compute_obj_name;
  std::string _filename;
  bool _Ok;
};

}; // End of VW namespace

















// IMPLEMENTATION & INLINES

// {{{ ParameterWriter (compute_obj, parameter[], value[], num_params, filename)

template <class T>
VW::ParameterWriter::ParameterWriter (std::string compute_obj,
				      const std::string parameter[],
				      const T value[],
				      unsigned int num_params,
				      std::string filename ) :
  _parser (compute_obj, filename),
  _compute_obj_name (compute_obj),
  _filename (filename),
  _Ok (_parser._file != 0)
{
  // If the file exists but it couldn't be parsed, it is ill-formed and we
  // shouldn't be messing with it in case we make it worse
  if (!_parser && _Ok) {
    std::cerr << "ParameterWriter: File is ill-formed, file write abandoned\n";
    _Ok = false;
  }
  else {
    _Ok = true;
    AddChildParameters(parameter, value, num_params);
    WriteOut();
  }
}

// }}}
// {{{ AddParameter (parameter, value)

template <class T>
void VW::ParameterWriter::AddParameter (const T& value) {
  // Generate string with default indentatio (2 spaces)
  std::string s(VW::GenerateXMLString(value,2));
  // An empty string signifies no label.
  _parameters.insert(std::pair<std::string,std::string>("",s));
}

// }}}
// {{{ AddChildParameter (parameter, value)

template <class T>
void VW::ParameterWriter::AddChildParameter (const std::string& parameter, 
					     const T& value) {
    std::string s(VW::GenerateXMLString(value,0));
    _parameters.insert(std::pair<std::string,std::string>(parameter,s));
}

// }}}
// {{{ AddChildParameters (parameter[], value[], num_params)

template <class T>
void VW::ParameterWriter::AddChildParameters (const std::string parameter[], 
					 const T value[],
					 unsigned int num_params) {
  for (unsigned int i=0;i<num_params;i++) {
    std::string s(VW::GenerateXMLString(value[i],0));
    _parameters.insert(std::pair<std::string,std::string>(parameter[i],s));
  }
}

// }}}

#endif // #ifndef _paramwriter_h_
