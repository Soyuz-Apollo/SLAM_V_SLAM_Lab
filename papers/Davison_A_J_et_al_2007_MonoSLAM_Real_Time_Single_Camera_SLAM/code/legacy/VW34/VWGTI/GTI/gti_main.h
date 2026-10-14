/** 
 * @file   gti_main.h
 * @author ID Reid, BJ Tordoff
 * @date   09/09/2002
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
 * @class   VW::GTI
 * @ingroup Hardware
 * @author  ID Reid, BJ Tordoff
 * @date    09/09/2002
 * @brief   Simple interface class for controlling "Gonad the Invincible".
 *
 * This is just a class implementation of some 
 * stuff that used to be in module.h, but has now been split into 
 * VW::Yorick, VW::SequencerMeteor and VW::GTI.
 *
 */ 

#include <string>

/* device usage flags */
#define USE_PMAC         0x1
#define USE_METEOR       0x2
#define USE_TRANSPUTERS  0x4

namespace VW {

class GTI {
 public:
  /* transputer functions */
  int InitLink(std::string boot_file);
 private:
  int SendChar(char c);
  static const char *status_msg[0x19];

 public:
  GTI(void) {};
  ~GTI(void) {};

  int Rotate(float angle);
  int Drive(float dist);
  int SetVelocity(float velocity);
  int SetSteeringAngle(float angle);
  int GetDriveOdometry(int *count);
};

}; // end namespace VW
