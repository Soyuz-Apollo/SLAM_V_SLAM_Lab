/** 
 * @file   xmlstuff.cc
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


#ifndef _xmlstuff_cc_
#define _xmlstuff_cc_
/* Misc namespace
 *
 * Random bits of handy code (only for two or three-line functions really)
 */

// System includes
#include <string>
#include <iostream>
//#include <unistd.h>

// Local includes
#include "xmlstuff.h"

#ifdef VERBOSE
#  undef VERBOSE
#endif
#define VERBOSE false

using namespace std;

// A little func for extracting xxx from the NAME="xxx" bit
std::string GetNameFromArgs(const std::string& args)
{
  std::ostringstream result(" ");
  result.clear();


  // First determine if the NAME bit exists
  string::size_type p = args.find_first_of("NAME=");
  if (p!=string::npos) {
    // OK, found one, so see if it has quotes
    p+=5;
    if (args[p]=='\"') {
      p++;
      // Read until close quote
      while (args[p]!='\"') {
	result << args[p];
	p++;
      }
    } else {
      // read until next space, end of tag, end of line etc
      while ((args[p]!=' ') && (args[p]!='>') && (args[p]!=0) && (args[p]!=10) && (args[p]!=13)) {
	result << args[p];
	p++;
      }
    }
  }
  return(result.str());
}

bool VW::FindXMLBlock(const std::string& block_name,
		     std::istream& input_stream,
		     std::string& name) {
  // Search for the start tag (discarding everything in between)
  string foundtag,foundargs;
  bool found=false;
  while (VW::FindNextXMLTag(input_stream,foundtag,foundargs)) {
    if (VERBOSE) cerr << "Found tag \""<<foundtag<<"\" with args "<<foundargs<<" (looking for \""<<block_name<<"\")"<<endl;
    if (foundtag == block_name) {
      found=true;
      name = GetNameFromArgs(foundargs);
      break;
    }
  }
  return(found);
}

bool VW::GetXMLBlock(const std::string& block_name,
		     std::istream& input_stream,
		     std::istringstream& contents,
		     std::string& name) {
    if (VERBOSE) cerr << "VW::GetXMLBlock"<<endl;
  string end_tag="/"+block_name;

  contents.clear();

  // Search for the start tag (discarding everything in between)
  string foundtag,foundargs;
  bool found=false;
  if (VERBOSE) cerr << "VW::GetXMLBlock - search for start tag ("<<block_name<<")"<<endl;
  while (VW::FindNextXMLTag(input_stream,foundtag,foundargs)) {
    if (VERBOSE) cerr << "Found tag "<<foundtag<<" with args "<<foundargs<<endl;
    if (foundtag == block_name) {
      found=true;
      // Copy the NAME into name
      name = GetNameFromArgs(foundargs);
      break;
    }
  }

  if (!found)
    return(false);
  else {
  // Now read until the close tag
  // Scan until a label is found
  char c;
  bool at_end=false;
  ostringstream read_contents;
  while (!at_end) {
    // Run until the next label
    while ((c = input_stream.get ()) != '<') {
      read_contents << c;
      if (input_stream.eof()) return(false);
    }
    // Read the label
    ostringstream ost;
    while ((c = input_stream.get()) != '>') {
      if (input_stream.eof()) return(false);
      ost << c;
    }
    if (ost.str()==end_tag) {
      // Got the end token
      at_end=true;
    } else {
      // Just another token, store it and continue
      read_contents << "<" << ost.str() << ">";
    }
    if (VERBOSE) cout << " string so far=" << read_contents.str() << endl;
  }
  contents.str(read_contents.str());
  }
  if (VERBOSE) cerr << "Contents="<<contents.str()<<endl;
  return(true);
}

bool VW::GetXMLBlock(const std::string& block_name,
		     const std::string& input_string,
		     std::string& contents,
		     std::string& name)
{
    if (VERBOSE) cerr << "VW::GetXMLBlock (string version)"<<endl;
  istringstream i_stream(input_string);
  istringstream contents_stream;
  bool result = VW::GetXMLBlock(block_name,i_stream,contents_stream,name);
  if (!result)
    return (false);
  else {
    contents=contents_stream.str();
  }
  return ( true );
}

bool VW::GetXMLBlockAsNumber(const std::string& block_name,
			     std::istream& input_string,
			     double& contents,
			     std::string& name)
{
    if (VERBOSE) cerr << "VW::GetXMLBlock (number version)"<<endl;
  istringstream contents_stream;
  bool result = VW::GetXMLBlock(block_name,
				input_string,
				contents_stream,
				name);
 if (VERBOSE)  cout << "VW::GetXMLBlockAsNumber: \""<<contents_stream.str()<<"\""<<endl;
  if (!result)
    return (false);
  else {
    contents_stream >> contents;
  }
  return ( true );
}


bool VW::GetXMLBlockAsNumber(const std::string& block_name,
			     std::istream& input_string,
			     float& contents,
			     std::string& name)
{
    if (VERBOSE) cerr << "VW::GetXMLBlock (number version)"<<endl;
  istringstream contents_stream;
  bool result = VW::GetXMLBlock(block_name,
				input_string,
				contents_stream,
				name);
  if (VERBOSE) cout << "VW::GetXMLBlockAsNumber: \""<<contents_stream.str()<<"\""<<endl;
  if (!result)
    return (false);
  else {
    contents_stream >> contents;
  }
  return ( true );
}


bool VW::GetXMLBlockAsNumber(const std::string& block_name,
			     std::istream& input_string,
			     int& contents,
			     std::string& name)
{
    if (VERBOSE) cerr << "VW::GetXMLBlock (number version)"<<endl;
  istringstream contents_stream;
  bool result = VW::GetXMLBlock(block_name,
				input_string,
				contents_stream,
				name);
  if (VERBOSE) cout << "VW::GetXMLBlockAsNumber: \""<<contents_stream.str()<<"\""<<endl;
  if (!result)
    return (false);
  else {
    contents_stream >> contents;
  }
  return ( true );
}


bool VW::GetXMLBlockAsNumber(const std::string& block_name,
			     std::istream& input_string,
			     unsigned int& contents,
			     std::string& name)
{
    if (VERBOSE) cerr << "VW::GetXMLBlock (number version)"<<endl;
  istringstream contents_stream;
  bool result = VW::GetXMLBlock(block_name,
				input_string,
				contents_stream,
				name);
  if (VERBOSE) cout << "VW::GetXMLBlockAsNumber: \""<<contents_stream.str()<<"\""<<endl;
  if (!result)
    return (false);
  else {
    contents_stream >> contents;
  }
  return ( true );
}


bool VW::GetMultipleXMLBlocks(const std::string& block_name,
			      std::istream& input_stream,
			      std::vector<std::string>& contents,
			      std::vector<std::string>& names) 
{
//   contents.clear();
  
//   string start_tag="<"+block_name+">";
//   string end_tag="</"+block_name+">";
  
//   // Start and end of current search
//   string::size_type s=0;
//   string::size_type e=0;
//   bool finished=false;
//   while (!finished) {
//     s=input_stream.find(start_tag,e)+start_tag.size();
//     // for some reason, this sometimes produces arse results
//     if (s<e) s=string::npos;
//     e=input_stream.find(end_tag,s);
//     // for some reason, this sometimes produces arse results
//     if (e<s) e=string::npos;
//     if ((s==string::npos) || (e==string::npos)) {
//       finished=true;
//     } else {
//       contents.push_back(input_stream.substr(s,e-s));
//       e+=end_tag.size();
//     }
//   }
  return(true);
}

bool VW::FindNextXMLTag(istream& input_file,
			string& tagname,
			string& arguments)
{
  // Find next bracket
  while (input_file.get () != '<') {
    // File may legitimately end before a tag is found
    if (input_file.eof()) return(false);
  }

  // Read contents
  ostringstream name,tagargs;
  char c;
  bool isname=true;
  while ((c = input_file.get()) != '>') {
    // If file ends during a tag, that's a genuine error
    if (input_file.eof()) {
      std::cerr << "VW::FindNextXMLTag - End of file whilst reading tag" 
		<< std::endl;
      return(false);
    }
    // If we hit a space, the rest are args
    if (c==' ') isname=false;
    else {
      if (isname)
	name << c;
      else 
	tagargs << c;
    }
  }

  tagname = name.str();
  arguments = tagargs.str();

  return(true);
}






#endif // #ifndef _xmlstuff_cc_
