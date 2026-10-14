
#===============================================================================
# glowcalc lesson 7
#===============================================================================

PROG=glowcalc_lesson7

SRCDIR=.
GLOWDIR=../../glow_src

MODULES=\
	calcengine \
	calcui \
	glowcalc

GLOWMODULES=\
	glow \
	glowAux \
	glowCheckBoxWidget \
	glowDebug \
	glowLabelWidget \
	glowMessageWindow \
	glowPanelWidget \
	glowPushButtonWidget \
	glowRadioButtonWidget \
	glowSenderReceiver \
	glowSeparatorWidget \
	glowUtilities \
	glowWidget

COMPILE=gcc
COMPILE_IRIX=CC
SHELL=/bin/sh

LIBS=-lglut -lpthread -lGL -lGLU -lXi -lXmu -lX11 -lm -lstdc++
LIBS_IRIX=-lCio -lglut -lX11 -lXmu -lXext -lGL -lGLU -lm

DEPFLAGS=-MM
DEPFLAGS_IRIX=-M

CFLAGS=-Wall \
	-DGLOW_PLATFORM_LINUX \
	-DGLOW_PLATFORM_LITTLEENDIAN
NODEBUGCFLAGS=-O2 -DNODEBUG
LDFLAGS=-L/usr/X11R6/lib

CFLAGS_IRIX=-fullwarn -no_auto_include \
	-LANG:std \
	-woff 1209,3201,1424,1375 \
	-OPT:Olimit=0 \
	-I../../Compat-SGI \
	-DGLOW_COMPAT_CLIBNOSTDNAMESPACE \
	-DGLOW_PLATFORM_IRIX \
	-DGLOW_PLATFORM_BIGENDIAN
LDFLAGS_IRIX=


include $(GLOWDIR)/glow_base.Makefile
