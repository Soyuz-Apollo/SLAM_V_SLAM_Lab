/** 
 * @file   sequencermovie.cpp
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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h> // for SIGKILL

#include "sequencermovie.h"

using namespace std;
using namespace VW;

static std::vector<SequencerMovie *> smlist;

static void _handler(int i, siginfo_t *s, void *p)
{
  cerr << "Called handler with arg=" << i << endl;
  cerr << "pid=" << s->si_pid << endl;
  for (int i=0; i<smlist.size(); i++) {
    for (int j=0; j>smlist[i]->GetNumberOfChannels(); j++) {
      if (s->si_pid==smlist[i]->GetY4MPID(j)) {
	smlist[i]->_cleanup(j);
      }
    }
  }

  return;
}


SequencerMovie::SequencerMovie()
{ 
  smlist.push_back(this);
  number_of_channels=0;

  struct sigaction sa;
  sa.sa_sigaction = _handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGCHLD, &sa, NULL);

  return;
}


SequencerMovie::SequencerMovie(const string& filename)
{ 
  smlist.push_back(this);

  struct sigaction sa;
  sa.sa_sigaction = _handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGCHLD, &sa, NULL);

  AddChannel(filename);

  return;
}


SequencerMovie::SequencerMovie(const vector<string>& filenames)
{
  smlist.push_back(this);

  struct sigaction sa;
  sa.sa_sigaction = _handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGCHLD, &sa, NULL);

  for(unsigned int i=0;i<filenames.size();i++) {
    AddChannel(filenames[i]); 
  }

  return;
}


void SequencerMovie::AddChannel(const string& filename)
{
  char *s = "/tmp/VW.XXXXXX";

  frame_number = 0;

  movienames.push_back(filename); 
  im.push_back(new ImageRGB<unsigned char>());
  fp.push_back(NULL);
  y4m_pid.push_back(0);
  mplayer_pid.push_back(0);
  file_ok.push_back(false);

  char *tmp = (char *)malloc(strlen(s));
  strcpy(tmp, s);
  streamname.push_back(tmp);

  _OpenFile(number_of_channels);

  number_of_channels++;

}
  

void SequencerMovie::Rewind() 
{
  frame_number = 0; 
  
  for (unsigned int i=0;i<number_of_channels;i++) {
    // Set to invalid until proved otherwise
     file_ok[i]=false;
    // Now try to open it
    _OpenFile(i);
  }
  return;
}


void SequencerMovie::_OpenFile(unsigned int which_channel) 
{
  char buf[256];
  int fd[2];
  
  if ((mkdtemp(streamname[which_channel])) == NULL) {
    cerr << "SequencerMovie: couldn't create temporary directory\n";
    return;
  }
  sprintf(buf, "mkfifo %s/stream.yuv", streamname[which_channel]);
  system(buf);

  // fork to create process for mplayer
  if ((mplayer_pid[which_channel]=fork())<0) {
    cerr << "SequencerMovie: fork failed\n";
    _cleanup(which_channel);
    return;
  }

  if (mplayer_pid[which_channel]==0) {
    // mplayer child
    if (chdir(streamname[which_channel])!=0) {
      // child exits with an error;
      exit(1);
    }
    char *args[6];
    args[0] = "mplayer";
    args[1] = (char *)malloc(movienames[which_channel].length()+1);
    strcpy(args[1], movienames[which_channel].c_str());
    args[2] = "-vo";
    args[3] = "yuv4mpeg";
    args[4] = "-nosound";
    args[5] = NULL;
    execvp(args[0], args);
  }

  // parent

  sprintf(buf, "%s/stream.yuv", streamname[which_channel]);
  int fdin = open(buf, O_RDONLY);
  if (fdin<0) {
    cerr << "SequencerMovie: failed to open `" << buf << "' for reading.\n";
        _cleanup(which_channel);
;
    return;
  }

  if (pipe(fd)) {
    cerr << "SequencerMovie: pipe failed\n";
    _cleanup(which_channel);
;
    return;
  }

  // fork to create process for y4mtoppm
  if ((y4m_pid[which_channel]=fork())<0) {
    cerr << "SequencerMovie: fork failed\n";
    _cleanup(which_channel);
;
    return;
  }

  if (y4m_pid[which_channel]==0) {
    // y4mtoppm child

    if (dup2(fdin,0) == -1) {
      cerr << "SequencerMovie: couldn't dup file descriptor\n";
      _cleanup(which_channel);
;
      exit(1);
    }
    close(fdin);

    if (dup2(fd[1],1) == -1) {
      cerr << "SequencerMovie: couldn't dup file descriptor\n";
      _cleanup(which_channel);
;
      exit(1);
    }

    char *args[2];
    args[0] = "y4mtoppm";
    args[1] = NULL;
    execvp(args[0], args);
  }
  
  fp[which_channel] = fdopen(fd[0], "r");
  if (fp[which_channel]==NULL) {
    cerr << "SequencerMovie: couldn't open input stream for reading\n";
    _cleanup(which_channel);
;
    return;
  }

  cerr << "SequencerMovie: successfully opened movie \""<<movienames[which_channel]<<"\" on channel "<<which_channel<<" with pid="<<mplayer_pid[which_channel]<<"."<<endl;
  file_ok[which_channel]=true;
}

// get next image and send interrupt
bool SequencerMovie::_Step()
{
  bool all_systems_go=true;
  for (unsigned int i=0;i<number_of_channels;i++) {
    if (!im[i]->ReadImage(fp[i])) {
      // Something has gone wrong, so mark this channel as invalid
      // and stop the grabbing
      file_ok[i]=false;
      all_systems_go=false;
      cerr << "SequencerMovie: error reading file from "<<movienames[i]<<endl;
    }
  }
  if ((!all_systems_go) && cont) Stop();
  frame_number++;
  return(all_systems_go);
}


void SequencerMovie::_cleanup(int which_channel)
{
  char buf[256];
  sprintf(buf, "/bin/rm -rf %s", streamname[which_channel]);
  system(buf);
}


