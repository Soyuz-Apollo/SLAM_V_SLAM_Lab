/*  SceneLib: software for real-time SLAM

    sim_or_rob.cpp
    Copyright (C) 2005 University of Oxford

    Author
    Andrew Davison
    ajd@doc.ic.ac.uk

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "models_base.h"
#include "sim_or_rob.h"

using namespace std;

// NULL-версия по умолчанию.
bool Sim_Or_Rob::make_internal_measurement(Internal_Measurement_Model*,
                                           VNL::Vector<double> &, 
					                       const VNL::Vector<double> &, 
					                       const VNL::Matrix<double> &)
{
  cerr << "WARNING: make_internal_measurement not implemented." << endl;

  return false;
}
