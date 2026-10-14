
This directory contains compatibility headers necessary to compile GLOW
under the SGI IRIX MIPSPro 7.3 development environment. It is not needed
if your are not using the MIPSPro 7.3 compiler.

To use this directory, you first need to determine if it is necessary.
Version 7.3 of the MIPSPro development tools require it, but newer
versions may not. If your version of the compilers is newer than 7.3.0,
check to determine if headers with these names already exist-- I would
guess that they'd be located in /usr/include/CC, but they may be in
/usr/include instead. If standard headers with these names already
exist, you should not use the ones in this directory. However, if your
version of MIPSPro does not include these headers, you should copy these
into /usr/include/CC. Alternately, you may move the directory somewhere
reasonable such as /usr/lib, and include it in a -I directive to the CC
command line.

