## VWX creates libVWX.a
##
## process file with automake

VWXdir = $(includedir)/VWX

INCLUDES = -I../VW -I../VNL

lib_LIBRARIES = libVWX.a
libVWX_a_SOURCES = 

include Display/Makefile.am
include Interface/Makefile.am

## stuff to build composite include files
include ../Make/Makefile.inc
BUILT_SOURCES = display.h interface.h
VWX_HEADERS = $(BUILT_SOURCES)


## Construct the meta-include file
../vwx.h: $(BUILT_SOURCES)
	( echo "#ifndef _`echo $@ | sed s/\\\./_/`_"; \
	    echo "#define _`echo $@ | sed s/\\\./_/`_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VWX/$$i\""); done; \
	    echo; \
	    echo "#endif //_`echo $@ | sed s/\\\./_/`_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vwx.h