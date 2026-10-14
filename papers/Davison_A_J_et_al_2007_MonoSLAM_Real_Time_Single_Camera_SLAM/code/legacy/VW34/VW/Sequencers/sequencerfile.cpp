/** 
 * @file   sequencerfile.cc
 * @author ID Reid, BJ Tordoff
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


#include <iostream>
#include <sstream>
#include <stdio.h>

#include "sequencerfile.h"

using namespace std;
using namespace VW;

// Default constructor
SequencerFile::SequencerFile() {
  cerr << "Init" << endl;
  number_of_channels=0;
  frame_number = 0; 
  // Set up everything else, even though there aren't any files
  _InitialiseFiles();
  cerr << "Done." << endl;
}

// Single file constructor
SequencerFile::SequencerFile(const string& file_name) {
  number_of_channels=1;
  first_file_name.push_back(file_name);
  _InitialiseFiles();
  file_ok[0]=_DetermineFilenamePatterns(0);
  frame_number = 0; 
}
// Multiple file constructor
SequencerFile::SequencerFile(const vector<string>& file_names) {
  number_of_channels=file_names.size();
  first_file_name=file_names;
  _InitialiseFiles();
  for (unsigned int i=0;i<number_of_channels;i++) {
    file_ok[i]=_DetermineFilenamePatterns(i);
  }
  frame_number = 0; 
}

// Add a new file-sequence to the list
void SequencerFile::AddChannel(const string& file_name) {
  number_of_channels++;
  first_file_name.push_back(file_name);
  format_string.push_back("<no file>");
  frame_num.push_back(0);
  initial_frame_num.push_back(0);
  play_speed.push_back(1);
  file_ok.push_back(false);
  im.push_back(new ImageRGB<unsigned char>());
  file_ok[number_of_channels-1]=_DetermineFilenamePatterns(number_of_channels-1);
}


// get next image
bool SequencerFile::_Step()
{
  bool all_systems_go=true;
  for (unsigned int i=0;i<number_of_channels;i++)
  {
    char filename[512];
    access_lock.Lock(); // to protect frame numbers (rewind etc can change them)
    sprintf(filename, 
            format_string[i].c_str(),
            frame_num[i]);
   access_lock.Unlock();
   //      cout << "Read file \"" <<filename<<"\" (play speed="<<play_speed[i]<<")"<<endl; 
    file_ok[i]=true;
    if (!im[i]->ReadImage(filename))
    {
      // Couldn't read next image.
      file_ok[i]=false;
      
      switch(_loop)
      {
      default:
      case NO_LOOPING:
        break;
      case LOOP:
        // If loop is on, try rewinding
        access_lock.Lock();
        frame_num[i]=initial_frame_num[i];
        sprintf(filename,
          format_string[i].c_str(),
          frame_num[i]);
        file_ok[i] = im[i]->ReadImage(filename);
        access_lock.Unlock();
        break;
      case BOUNCE:
        // Not implemented yet - should make it go backwards
        access_lock.Lock();
        play_speed[i] *= -1;
        frame_num[i] += play_speed[i];
        sprintf(filename,
          format_string[i].c_str(),
          frame_num[i]);
        file_ok[i] = im[i]->ReadImage(filename);
        access_lock.Unlock();
        break;
      case REPEATLAST:
        // Go back and get the previous frame
        access_lock.Lock();
        frame_num[i]=frame_num[i] - play_speed[i];
        sprintf(filename,
          format_string[i].c_str(),
          frame_num[i]);
        file_ok[i] = im[i]->ReadImage(filename);
        access_lock.Unlock();
        break;
      }
    }
    access_lock.Lock();
    if (file_ok[i] == false)
    {
      // Something has gone wrong, so mark this channel as invalid
      // and stop the grabbing
      cerr << "SequencerFile: Couldn't read image "<<filename<<"."<<endl;
      all_systems_go=false;
    }
    else
    {
      // Move to next
      frame_num[i]=frame_num[i] + play_speed[i];	
    }
    access_lock.Unlock();
  }
  if ((!all_systems_go) && cont)
    Stop();
  frame_number++;
  return(all_systems_go);
}

void SequencerFile::_InitialiseFiles() 
{
  frame_number = 0; 
  cerr << "SequencerFile::InitialiseFiles: Init "<<number_of_channels<<" files."<<endl;
  format_string.resize(number_of_channels);
  frame_num.resize(number_of_channels);
  play_speed.resize(number_of_channels);
  initial_frame_num.resize(number_of_channels);
  for (unsigned int i=0;i<number_of_channels;i++) {
    // Create the data
    im.push_back(new ImageRGB<unsigned char>());
    file_ok.push_back(false);
    play_speed[i]=1;
  }
  _loop = NO_LOOPING;
  return;
  
}

bool SequencerFile::_DetermineFilenamePatterns(unsigned int which_channel) 
{
  // find the last number
  string::size_type start_pos,end_pos;
  string numbers("0123456789");


  // We expect the filename to end in .xxx, so look for the final '.'
  if (first_file_name[which_channel].find_last_of(".")==string::npos) {
    // This means the filename doesn not contain a dot!
    cerr << "SequencerFile: Filename \""<<first_file_name[which_channel]<<"\" does not have an extension!" << endl;
    return(false);
  } else {
    // Got the last dot, so now search everything before it for the last number
    if ((end_pos=first_file_name[which_channel].find_last_of(numbers))==string::npos) {
      // didn't find a number
      cerr << "SequencerFile: Filename \""<<first_file_name[which_channel]<<"\" does not contain numbers before the final dot!" << endl;
      return(false);
    } else {
      // Now scan back from 'end_pos' counting how many numbers there are
      int number=1;
      bool still_numbers=true;
      for (start_pos=end_pos-1;
	   ((start_pos>=0) && (still_numbers));) {
	if (numbers.find(first_file_name[which_channel].at(start_pos))==string::npos) {
	  still_numbers=false;
	  start_pos++;
	} else {
	  number++;
	  start_pos--;
	}
      }
      istringstream numstream(first_file_name[which_channel].substr(start_pos, end_pos-start_pos+1));
      ostringstream formatstream;
      formatstream << first_file_name[which_channel].substr(0,start_pos)  // Prefix
		   << "%0" << number << "d"          // Number
		   << first_file_name[which_channel].substr(end_pos+1,first_file_name[which_channel].size()-end_pos);

      // Now store the result
      format_string[which_channel]=formatstream.str();
      numstream >> initial_frame_num[which_channel];
      frame_num[which_channel] = initial_frame_num[which_channel];

      cerr << "SequencerFile: Channel "<<which_channel<<", Format="<<format_string[which_channel]<<" starting at n="<<initial_frame_num[which_channel]<<endl;

    }
  }
  return(true);

}
void SequencerFile::Rewind() 
{
  access_lock.Lock();
  frame_number = 0; 
  
  for (unsigned int i=0;i<number_of_channels;i++) {
    frame_num[i]=initial_frame_num[i];
    file_ok[i]=true;
  }
  access_lock.Unlock();
  return;
}
void SequencerFile::Skip(int num_frames)
{
  access_lock.Lock();
  frame_number += num_frames;
  for (unsigned int i=0;i<number_of_channels;i++) {
    frame_num[i] += num_frames;
    file_ok[i] = true;
  }
  access_lock.Unlock();
  return;
}
