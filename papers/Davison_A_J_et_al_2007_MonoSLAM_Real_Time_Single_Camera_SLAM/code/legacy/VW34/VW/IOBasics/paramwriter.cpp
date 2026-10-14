/** 
 * @file   paramwriter.cc 
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


#ifndef _paramwriter_cc_
#define _paramwriter_cc_

#include "paramwriter.h"
#include <iostream>
#include <fstream>
#include <assert.h>

using namespace std;

/** ParameterWriter
 *
 * Write out a parameter list definition to a parameter file, as defined
 * in paramfiles.README
 */

#define TEMP_FILE_NAME "/tmp/vw_paramwriter_temp.xml"

// {{{ ParameterWriter (compute_obj, filename)

VW::ParameterWriter::ParameterWriter (string compute_obj,
				      string filename) :
  _parser (compute_obj, filename),
  _compute_obj_name (compute_obj),
  _filename (filename),
  _Ok(_parser._file != 0)
{
  // If the file exists but it couldn't be parsed, it is ill-formed and we
  // shouldn't be messing with it in case we make it worse
  if (!_parser && _Ok) {
    cout << "ParameterWriter: File is ill-formed, file write abandoned\n";
    _Ok = false;
  }
  else {
    _Ok = true;
  }
}

// }}}


// {{{ void VW::ParameterWriter::WriteOut()

void VW::ParameterWriter::WriteOut() {
  if (!_Ok) return;
  
  ostringstream _temp_file;

  // If file exists
  if (_parser) {

    // Reset parser to point to start of file and copy to where we should
    // start replacing
    _parser._file->seekg (0, ios::beg);
    try {
      while (get_next_label () != _compute_obj_name) {};
    }
    catch (VW::ParameterParser::EOFErr) {
      // This is cool, the file exists, but the parameter list doesn't
    }

    // Work out where we're at
    streampos pos = _parser._file->tellg ();

    // Copy up to this point
    copy_file_to_stream (_filename, 0, pos, _temp_file);

    // Write out parameter list
    _temp_file << "\n";
    write_param_list(_temp_file);
    _temp_file << "</" << _compute_obj_name << ">";

    // Search to end of parameter list in input file
    try {
      while (get_next_label () != "/" + _compute_obj_name) {};
    }
    catch (VW::ParameterParser::EOFErr) {};

    // Copy from this point
    pos = _parser._file->tellg ();
    _parser._file->seekg (0, ios::end);
    streampos pos_end = _parser._file->tellg();
    copy_file_to_stream (_filename, pos, pos_end, _temp_file);
  }

  // If file doesn't exist
  else {
    // Write out parameter list
    _temp_file << "<" << _compute_obj_name << ">\n";
    write_param_list (_temp_file);
    _temp_file << "</" << _compute_obj_name << ">\n";
  }

  if (dynamic_cast<ifstream*>(_parser._file)) 
    dynamic_cast<ifstream*>(_parser._file)->close();


  // Now replace old file with temporary string
  cerr << "Hurrah. Now this is the file contents:\n" << _temp_file.str() << endl;
  ofstream real_output(_filename.c_str());
  if (!real_output) {
    cerr << "ParamWriter: Failed to open file for output." << endl;
  } else {
    real_output << _temp_file.str();
    real_output.close();
  }
 
  return;
}

// }}}


// {{{ void VW::ParameterWriter::write_param_list (ostream& output_stream)

void VW::ParameterWriter::write_param_list (ostream& output_stream) {
  cerr << "Writing out "<<_parameters.size()<<" params." << endl;

  for (map<string,string>::const_iterator i=_parameters.begin();
       i!=_parameters.end();
       i++) {
    // An empty string implies that surrounding tags are not required
    // Note the double-space gives default indentation.
    cerr << "Size(second)=" << i->first.size() << endl;
    string s="a";
    cerr << "Size(second)=" << s.size() << endl;
    if (i->first.size()>0) output_stream << "  " << "<" << i->first << "> ";
    output_stream << i->second;
    if (i->first.size()>0) output_stream << " </" << i->first << ">\n";
  }


}

// }}}



// {{{ void VW::ParameterWriter::copy_file_to_stream(..)

void VW::ParameterWriter::copy_file_to_stream(string filename,
					      streampos start_position,
					      streampos end_position,
					      ostream& output_stream) {
  char c;
  ifstream f(filename.c_str());
  assert (f);
  f.seekg(start_position, ios::beg);
  while(f.tellg() != end_position) {
    f.get(c);
    output_stream << c;
  }
  f.close();

}

// }}}

// Fundamental
// {{{ string VW::ParameterWriter::get_next_label ()

string VW::ParameterWriter::get_next_label ()
{
  assert (_Ok);

  // Find next bracket
  while (_parser._file->get () != '<') {
    if (_parser._file->eof()) throw EOFErr ();
  }

  // Read contents
  ostringstream ost;
  char c;
  while ((c = _parser._file->get()) != '>') {
    if (_parser._file->eof()) throw EOFErr ();

    ost << c;
  }

  return ost.str ();
}

// }}}
// {{{ void VW::ParameterWriter::print_writer_error (ParseErr exception)

void VW::ParameterWriter::print_writer_error (WriterErr error)
{
  assert (_Ok);

  if (_parser._file->eof()) {
    cout << "ParameterWriter: Parse Error at end of file: ";
  }
  else {
    // Work out what line we're on
    streampos pos = _parser._file->tellg();
    _parser._file->seekg (0, ios::beg);
    int line_num = 1;
    do {
      while (_parser._file->get() != '\n') if (_parser._file->eof()) break;
      if (_parser._file->eof()) break;
      line_num++;
    } while (_parser._file->tellg() < pos);

    cout << "ParameterWriter: Parse Error on line " << line_num << ": ";
  }
  cout << error._error_string << endl;
}

// }}}



// Force a few instantiations
template VW::ParameterWriter::ParameterWriter (string, const string[], const double[], unsigned int,string);
template VW::ParameterWriter::ParameterWriter (string, const string[], const float[], unsigned int,string);
template VW::ParameterWriter::ParameterWriter (string, const string[], const int[], unsigned int,string);
template VW::ParameterWriter::ParameterWriter (string, const string[], const char[], unsigned int,string);
template VW::ParameterWriter::ParameterWriter (string, const string[], const unsigned int[], unsigned int,string);
template VW::ParameterWriter::ParameterWriter (string, const string[], const unsigned char[], unsigned int,string);
template VW::ParameterWriter::ParameterWriter (string, const string[], const string[], unsigned int,string);

template void VW::ParameterWriter::AddChildParameters (const string[], const double[], unsigned int);
template void VW::ParameterWriter::AddChildParameters (const string[], const float[], unsigned int);
template void VW::ParameterWriter::AddChildParameters (const string[], const int[], unsigned int);
template void VW::ParameterWriter::AddChildParameters (const string[], const char[], unsigned int);
template void VW::ParameterWriter::AddChildParameters (const string[], const unsigned int[], unsigned int);
template void VW::ParameterWriter::AddChildParameters (const string[], const unsigned char[], unsigned int);
template void VW::ParameterWriter::AddChildParameters (const string[], const string[], unsigned int);

template void VW::ParameterWriter::AddChildParameter (const string&, const double&);
template void VW::ParameterWriter::AddChildParameter (const string&, const float&);
template void VW::ParameterWriter::AddChildParameter (const string&, const int&);
template void VW::ParameterWriter::AddChildParameter (const string&, const char&);
template void VW::ParameterWriter::AddChildParameter (const string&, const unsigned int&);
template void VW::ParameterWriter::AddChildParameter (const string&, const unsigned char&);
template void VW::ParameterWriter::AddChildParameter (const string&, const string&);

template void VW::ParameterWriter::AddParameter (const double&);
template void VW::ParameterWriter::AddParameter (const float&);
template void VW::ParameterWriter::AddParameter (const int&);
template void VW::ParameterWriter::AddParameter (const char&);
template void VW::ParameterWriter::AddParameter (const unsigned int&);
template void VW::ParameterWriter::AddParameter (const unsigned char&);
template void VW::ParameterWriter::AddParameter (const string&);



#endif /* _paramwriter_cc_ */
