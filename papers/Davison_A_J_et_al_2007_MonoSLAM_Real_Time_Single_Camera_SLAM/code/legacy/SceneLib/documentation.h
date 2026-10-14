/*  MonoSLAM: Real-Time Single Camera SLAM

    MonoSLAM/monoslam.h
    Copyright (C) 2005 University of Oxford 

    Author
    Paul Smith
    pas@robots.ox.ac.uk
    Scene home page: http://www.robots.ox.ac.uk/~ajd/Scene/

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

///////////////////////////////////////////////////////
// General Doxygen documentation
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// The main title page
/**
@mainpage

\section Introduction
The Scenelib libraries provide a framework for performing
Simultaneous Localisation and Mapping (SLAM), with particular implementations
for performing Single Camera SLAM (%MonoSLAM) for real-time camera localisation.

Three libraries are provided:

  - \ref gScene "Scene" for generic SLAM functionality
  - \ref gSceneImproc "SceneImproc" for image processing functions relevant to
%MonoSLAM
  - \ref gMonoSLAM "MonoSLAM" for single-camera slam applications

\section References

The slides of a talk giving an introduction to single-camera SLAM and the
Scenelib libraries can be found <a
href="http://www.robots.ox.ac.uk/~pas/Research/monoslamtutorial.pdf">here</a>.

The single-camera SLAM system implemented in these libraries and the theory
underlying it was published in the following papers:

  - <a
href="http://www.robots.ox.ac.uk/~lav/Publications/davison_iccv2003/
davison_iccv2003.html">Real-Time Simultaneous Localisation and Mapping with a
Single Camera</a> Andrew J Davison, 9th IEEE International Conference on
Computer Vision (ICCV), Nice, France, October 2003
  - <a
href="http://www.robots.ox.ac.uk/~lav/Publications/davison_etal_iav2004/
davison_etal_iav2004.html">Real-Time 3D SLAM with Wide-Angle Vision</a> Andrew
J. Davison, Yolanda Gonz&aacute;lez Cid and Nobuyuki Kita, 5th Symposium on
Intelligent Autonomous Vehicles (IAV), Lisbon, Portugal, July 2004

*/

///////////////////////////////////////////////////////
// Modules classifying classes and functions

/**
@defgroup gMonoSLAM MonoSLAM library
The %MonoSLAM library provides a set of classes and functions that
extend the Scene library for performing real-time SLAM using a monocular
camera.
*/

/**
@defgroup gScene Scene library
The Scene library provides a set of classes and functions for sequential
estimation, and particularly for Simultaneous Localisation And Mapping (SLAM).
*/

/**
@defgroup gSceneImproc SceneImproc library
The SceneImproc library provides a few basic image processing functions, such
as cross-correlation.
*/

