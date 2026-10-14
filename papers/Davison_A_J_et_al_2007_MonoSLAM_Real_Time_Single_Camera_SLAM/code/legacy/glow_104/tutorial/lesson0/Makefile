
#===============================================================================
# testglow (lesson 0) makefile
#===============================================================================

PROG=testglow

  # Make sure you set SRCDIR to point to where you put testglow.cpp
SRCDIR=.
  # Make sure you set GLOWDIR to point to where you installed the
  # glow_src directory
GLOWDIR=../../glow_src


  # The source file to compile is called "testglow.cpp"
MODULES=\
	testglow

  # The GLOW files to compile
GLOWMODULES=\
	glow \
	glowAux \
	glowDebug \
	glowLabelWidget \
	glowPushButtonWidget \
	glowSenderReceiver \
	glowUtilities \
	glowVectorAlgebra \
	glowViewTransform \
	glowWidget


  # These symbols control the compiler. Default settings provided were
  # designed for a Linux 2.0 environment, but should work with minor
  # tweaking for any environment that includes gcc 2.95 or later. A
  # specialiazation is provided for an IRIX 6.5 / MIPSPro 7.3 environment.
COMPILE=gcc
COMPILE_IRIX=CC
SHELL=/bin/sh

  # Libraries to link with the program. Again, default is for gcc/Linux,
  # and a specialization is provided for IRIX/MIPSPro.
LIBS=-lglut -lpthread -lGL -lGLU -lXi -lXmu -lX11 -lm -lstdc++
LIBS_IRIX=-lCio -lglut -lX11 -lXmu -lXext -lGL -lGLU -lm

  # Flag to tell the compiler that we only want to generate dependencies
DEPFLAGS=-MM
DEPFLAGS_IRIX=-M

  # Default CFLAGS and LDFLAGS for Linux/gcc.
CFLAGS=-Wall \
	-DGLOW_PLATFORM_LINUX \
	-DGLOW_PLATFORM_LITTLEENDIAN
NODEBUGCFLAGS=-O2 -DNODEBUG
LDFLAGS=-L/usr/X11R6/lib

  # Specialization for IRIX
  # the Compat-SGI directory is added here with -I. You don't need this
  # directive if you install the headers in your /usr/include/CC.
CFLAGS_IRIX=-fullwarn -no_auto_include \
	-LANG:std \
	-woff 1209,3201,1424,1375 \
	-OPT:Olimit=0 \
	-I../../Compat-SGI \
	-DGLOW_COMPAT_CLIBNOSTDNAMESPACE \
	-DGLOW_PLATFORM_IRIX \
	-DGLOW_PLATFORM_BIGENDIAN
LDFLAGS_IRIX=


  # Do not modify this line
include $(GLOWDIR)/glow_base.Makefile
