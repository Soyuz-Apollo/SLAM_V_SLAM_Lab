## VWGL creates libVWGL.a
##
## process file with automake

VWGLdir = $(includedir)/VWGL

lib_LIBRARIES = libVWGL.a


INCLUDES = -I../VW -I../VNL

libVWGL_a_SOURCES = 

include Display/Makefile.am
include Interface/Makefile.am

## stuff to build composite include files
include ../Make/Makefile.inc
BUILT_SOURCES = display.h interface.h
VWGL_HEADERS = $(BUILT_SOURCES)


## Construct the meta-include file
../vwgl.h: $(BUILT_SOURCES)
	( echo "#ifndef _`echo $@ | sed s/\\\./_/`_"; \
	    echo "#define _`echo $@ | sed s/\\\./_/`_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VWGL/$$i\""); done; \
	    echo; \
	    echo "#endif //_`echo $@ | sed s/\\\./_/`_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vwgl.h