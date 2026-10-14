## VWCore creates libVW.a
##
## process the file wih automake to generate Makefile.in

lib_LIBRARIES = libVW.a

VWdir = $(includedir)/VW

INCLUDES = -I../VNL

libVW_a_SOURCES = 
VW_HEADERS =

include Image/Makefile.am
include MathBits/Makefile.am
include Utils/Makefile.am
include IOBasics/Makefile.am

include GeomObjects/Makefile.am
include Events/Makefile.am
include Graphics/Makefile.am

include Sequencers/Makefile.am
include GeomCompute/Makefile.am
include Improc/Makefile.am

include Pixmaps/Makefile.am

## stuff to build meta-include files
include ../Make/Makefile.inc

BUILT_SOURCES = image.h \
	mathbits.h \
	utils.h \
	iobasics.h \
	events.h \
	sequencers.h \
	geomobjects.h \
	geomcompute.h \
	improc.h \
	graphics.h \
	pixmaps.h 


VW_HEADERS += $(BUILT_SOURCES)

## Construct the meta-meta-include file
../vw.h: $(BUILT_SOURCES)
	( echo "#ifndef _`echo $@ | sed s/\\\./_/`_"; \
	    echo "#define _`echo $@ | sed s/\\\./_/`_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VW/$$i\""); done; \
	    echo; \
	    echo "#endif //_`echo $@ | sed s/\\\./_/`_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vw.h