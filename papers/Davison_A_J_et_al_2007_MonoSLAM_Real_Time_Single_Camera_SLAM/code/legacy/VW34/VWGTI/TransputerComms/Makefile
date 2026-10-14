###Makefile for transputer comms

LIBNAME = VWGTI
MODULE = GTITransputer

#---files--
OFILES =  	b004link.o \
		channel.o \
		filec.o	\
		hostc.o \
		iserver.o \
		serverc.o
HFILES =  	inmos.h \
		channel.h \
		iserver.h \
		iserv.h \
		pack.h

TARGETS = headers libs

include ../../Make/Makefile.generic

iserver: iserver.o
	gcc -DISERVER_EXECUTABLE -o iserver iserver.o serverc.o b004link.o channel.o filec.o hostc.o;
	cp iserver ../../bin;

