/** 
 * @file   paramparser.cpp
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


#ifndef _paramparser_cc_
#define _paramparser_cc_

// System includes
#include <sstream>
#include <assert.h>

// Local includes
#include "paramfiles_general.h"
#include "paramparser.h"


#define VERBOSE false

using namespace std;

/** 
 * @class ParameterParser
 *
 * A simple XML-style parser for interpreting files of parameter data
 * for compute objects like corner detectors and RANSAC.  I expect it will
 * be modified a lot, possibly using libraries for proper XML parsing,
 * but hopefully the same essential structure of the files will remain.
 * See paramparser.README.
 */


namespace VW {


ParameterParser::ParameterParser (string compute_obj,
				      string filename) :
  Verbose (true)
{
  _file = new ifstream(filename.c_str());
  _I_opened_this_stream=true;
  _Ok = (_file != 0);
  if (!_Ok) {
    if (Verbose) cout << "ParameterParser: File not found\n";
    return;
  }

  _Ok = parse_file (*_file,compute_obj);
}


ParameterParser::ParameterParser (string compute_obj, 
				      istream& input_stream) :
  Verbose (true),
  _file (&input_stream),
  _Ok (_file != 0),
  _I_opened_this_stream(false) // Make sure we don't close the user's stream
{
  if (!_Ok) {
    if (Verbose) cout << "ParameterParser: Stream not valid\n";
    return;
  }

  _Ok = parse_file (*_file,compute_obj);
}


template <class T>
ParameterParser::ParameterParser (string compute_obj, const string parameter[],
				      T value[], unsigned int num_params,
				      string filename) :
  Verbose (false)
{
  _file = new ifstream(filename.c_str());
  _I_opened_this_stream=true;
   _Ok = (_file != 0);
 if (!_Ok) {
    if (Verbose) cout << "ParameterParser: File not found\n";
    return;
  }
  if (!(_Ok = parse_file (*_file,compute_obj))) return;

  Extract (parameter, value, num_params);
}

// Instantiate some constructors that will work
template ParameterParser::ParameterParser (string, const string[], double[],
					       unsigned int, string);
template ParameterParser::ParameterParser (string, const string[], float[],
					       unsigned int, string);
template ParameterParser::ParameterParser (string, const string[], int[],
					       unsigned int, string);
template ParameterParser::ParameterParser (string, const string[], char[],
					       unsigned int, string);
template ParameterParser::ParameterParser (string, const string[], unsigned int[],
					       unsigned int, string);
template ParameterParser::ParameterParser (string, const string[],
					       unsigned char[],
					       unsigned int, string);
template ParameterParser::ParameterParser (string, const string[], string[],
					       unsigned int, string);


ParameterParser::~ParameterParser() {
  // Make sure we don't close the user's stream
  if (_I_opened_this_stream) {
    if (dynamic_cast<ifstream*>(_file)) dynamic_cast<ifstream*>(_file)->close();
    delete(_file);
  }
}

// Extract info from parsed file

template <class T>
void ParameterParser::Extract (const string parameter[], 
				   T value[],
				   unsigned int num_params) const
{
  if (!_Ok) return;
  for (unsigned int i = 0; i < num_params; i++) {
    map<string, string>::const_iterator val = _parameters.find (parameter[i]);
    if (val != _parameters.end()) {
      VW::ReadXMLString<T> (val->second, value[i]);
    }
  }
}

// For strings, things are simpler
template<> 
void ParameterParser::Extract (const string parameter[], 
				   string value[],
				   unsigned int num_params) const {
  if (!_Ok) return;
  for (unsigned int i = 0; i < num_params; i++) {
    map<string, string>::const_iterator val = _parameters.find (parameter[i]);
    if (val != _parameters.end()) {
      value[i] = val->second;
    }
  }
}
// Instantiate the Extract() functions we know will work:
template void ParameterParser::Extract (const string[], double[],
					unsigned int) const;
template void ParameterParser::Extract (const string[], float[],
					unsigned int) const;
template void ParameterParser::Extract (const string[], int[],
					unsigned int) const;
template void ParameterParser::Extract (const string[], char[],
					unsigned int) const;
template void ParameterParser::Extract (const string[], unsigned int[],
					unsigned int) const;
template void ParameterParser::Extract (const string[], unsigned char[],
					unsigned int) const;


template <class T>
bool ParameterParser::GetValue (string parameter, T& value) const
{
  if (!_Ok) return false;

  map<string, string>::const_iterator it = _parameters.find (parameter);
  if (it != _parameters.end()) {
    VW::ReadXMLString<T> (it->second, value);
    if (VERBOSE) cerr << parameter << "=" << value << endl;
  }
  else return false;

  return true;
}

// For a string, things are simpler
template<> bool 
ParameterParser::GetValue (string parameter, string& value) const 
{
  if (!_Ok) return false;

  map<string, string>::const_iterator it = _parameters.find (parameter);
  if (it != _parameters.end()) {
    value = it->second;
    if (VERBOSE) cerr << parameter << "=" << value << endl;
  }
  else return false;

  return true;
}
//   Instantiate some versions that will work
template bool ParameterParser::GetValue (string, double&) const;
template bool ParameterParser::GetValue (string, float&) const;
template bool ParameterParser::GetValue (string, int&) const;
template bool ParameterParser::GetValue (string, char&) const;
template bool ParameterParser::GetValue (string, unsigned int&) const;
template bool ParameterParser::GetValue (string, unsigned char&) const;


// Utilities

bool ParameterParser::parse_file (istream& input_file, 
				      string compute_obj_name)
{
  assert (_Ok);
  using namespace ParameterFiles;

  try {
    while (!parse_parameter_list (input_file, compute_obj_name)) {};
  }
  catch (EOFErr) {
    // Didn't find any parameter lists, that's weird, but OK
    return true;
  }
  catch (ParseErr error) {
    // File is ill-formed, that's not OK
    if (Verbose) print_parse_error (input_file, error);
    return false;
  }
  catch (...) {
    cerr << "OH SHIT!\n";
  }

  // Parsed OK
  return true;
}


bool ParameterParser::parse_parameter_list (istream& input_file, string compute_obj_name)
{
  assert (_Ok);
  using namespace ParameterFiles;

  const string end_tag = "/"+compute_obj_name;
  string label;
  ostringstream _contents;

  try {
 
    // Find name label
    while ((label = get_next_label (input_file)) != compute_obj_name) {}

    // Now read until the close tag
    // Scan until a label is found
    char c;
    bool at_end=false;
    while (!at_end) {
      // Run until the next label
      while ((c = input_file.get ()) != '<') {
	_contents << c;
	if (input_file.eof()) return(false);
      }
      // Read the label
      ostringstream ost;
      while ((c = input_file.get()) != '>') {
	if (input_file.eof()) return(false);
	ost << c;
      }
      if (ost.str()==end_tag) {
	// Got the end token
	at_end=true;
      } else {
	// Just another token, store it and continue
	_contents << "<" << ost.str() << ">";
      }
    }
    _block_contents=_contents.str();

  }
  catch (EOFErr) {
    throw ParseErr ("Unterminated parameter list");
  }
  // re-attach the end tag so that the paramparser knows when to stop
  _contents << "<"<<end_tag<<">\n";
 
  // Now find the parameters in the block
  istringstream string_reader(_contents.str());
  label=get_next_label(string_reader);
  while (label!=end_tag) {
    parse_parameter (string_reader,label);
    label=get_next_label(string_reader);
  } 
  return true;

}


void ParameterParser::parse_parameter (istream& input_file, const string param_name)
{
  assert (_Ok);
  using namespace ParameterFiles;
  if (VERBOSE) cerr << "Parse param "<<param_name<<endl;

  try {
    string label;

    // Already read the start label, so just read the contents and end-label

    // Extract data
    string data = parse_basic(input_file,param_name);

    // Add information to our parameter list
    if (VERBOSE) cerr << "Adding param "<<param_name<<" ="<<data<<endl;
    _parameters.insert (pair<string, string>(param_name, data));

    // parse_contents has read the end label, so no need to search for it

  }
  catch (EOFErr) {
    throw ParseErr ("Unterminated parameter block");
  }
}


string ParameterParser::parse_basic(istream& input_file, 
				    const string elementname) 
{
  assert (_Ok);

  char c;
  bool not_found=true;
  string endtoken="/"+elementname;
  ostringstream contents;

  try {
  // Search from here, stopping only when the end tag is seen
  while (not_found) {
    while ((c = input_file.get ()) != '<') {
      contents << c;
      //     cerr << c << endl;
      if (input_file.eof()) throw ParseErr ("End of file while parsing <"+elementname+">");
    }

    // Found a token, so see if it is the right one
    
    // Read contents
    ostringstream ost;
    char c;
    while ((c = input_file.get()) != '>') {
      if (input_file.eof()) throw ParseErr ("End of file while parsing <"+elementname+">");
      ost << c;
    }
  
    //cerr << ost.str() << "==" << token << "?" << endl;
    if (ost.str()==endtoken) {
      // Got the end token
      not_found=false;
    } else {
      // Just another token, store it and continue
      contents << "<" << ost.str() << ">";
    }
  }
  }
  catch (EOFErr) {
    throw ParseErr ("End of file while parsing <"+elementname+">");
  }
  return (contents.str());
}


// Fundamental

string ParameterParser::get_next_label (istream& input_file)
{
  assert (_Ok);

  // Find next bracket
  while (input_file.get () != '<') {
    if (input_file.eof()) throw EOFErr ();
  }

  // Read contents
  ostringstream ost;
  char c;
  while ((c = input_file.get()) != '>') {
    if (input_file.eof()) throw EOFErr ();

    ost << c;
  }

  return ost.str ();
}


void ParameterParser::print_parse_error (istream& input_file, ParseErr error)
{
  assert (_Ok);

  if (input_file.eof()) {
    cout << "ParameterParser: Parse Error at end of file: ";
  }
  else {
    // Work out what line we're on
    streampos pos = input_file.tellg();
    input_file.seekg (0, ios::beg);
    int line_num = 1;
    do {
      while (input_file.get() != '\n') if (input_file.eof()) break;
      if (input_file.eof()) break;
      line_num++;
    } while (input_file.tellg() < pos);

    cout << "ParameterParser: Parse Error on line " << line_num << ": ";
  }
  cout << error._error_string << endl;
}

}; // End namespace VW

#endif // #ifndef _paramparser_cpp_
