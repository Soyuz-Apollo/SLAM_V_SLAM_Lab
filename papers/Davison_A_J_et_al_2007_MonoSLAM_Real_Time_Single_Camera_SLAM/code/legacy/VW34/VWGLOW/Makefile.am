## VWWx creates libVWWx.a
##
## process file with automake

VWGLOWdir = $(includedir)/VWGLOW

lib_LIBRARIES = libVWGLOW.a

INCLUDES = -I../VNL -I../VW @GLOW_CFLAGS@

libVWGLOW_a_SOURCES =

include Display/Makefile.am
include Interface/Makefile.am

## stuff to build composite include files
include ../Make/Makefile.inc
BUILT_SOURCES = display.h interface.h
VWGLOW_HEADERS = $(BUILT_SOURCES)


## Construct the meta-include file
../vwglow.h: $(BUILT_SOURCES)
	( echo "#ifndef _`echo $@ | sed s/\\\./_/`_"; \
	    echo "#define _`echo $@ | sed s/\\\./_/`_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VWGLOW/$$i\""); done; \
	    echo; \
	    echo "#endif //_`echo $@ | sed s/\\\./_/`_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vwglow.h
