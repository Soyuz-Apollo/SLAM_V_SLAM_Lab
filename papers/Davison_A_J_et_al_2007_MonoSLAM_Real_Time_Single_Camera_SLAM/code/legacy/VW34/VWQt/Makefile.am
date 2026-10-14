## VWWx creates libVWWx.a
##
## process file with automake

VWQtdir = $(includedir)/VWQt

lib_LIBRARIES = libVWQt.a

INCLUDES = -I../VNL -I../VW 

libVWQt_a_SOURCES =

include Display/Makefile.am
include Interface/Makefile.am

## stuff to build composite include files
include ../Make/Makefile.inc
BUILT_SOURCES = display.h interface.h
VWQt_HEADERS = $(BUILT_SOURCES)


## Construct the meta-include file
../vwqt.h: $(BUILT_SOURCES)
	( echo "#ifndef _`echo $@ | sed s/\\\./_/`_"; \
	    echo "#define _`echo $@ | sed s/\\\./_/`_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VWQt/$$i\""); done; \
	    echo; \
	    echo "#endif //_`echo $@ | sed s/\\\./_/`_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vwqt.h
