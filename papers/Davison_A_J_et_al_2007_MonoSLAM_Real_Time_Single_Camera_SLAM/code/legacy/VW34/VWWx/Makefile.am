## VWWx creates libVWWx.a
##
## process file with automake

VWWxdir = $(includedir)/VWWx

lib_LIBRARIES = libVWWx.a

INCLUDES = -I../VNL -I../VW `wxgtk-2.4-config --cflags` `wxgtk2-2.4-config --cflags`

libVWWx_a_SOURCES =

include Display/Makefile.am
include Interface/Makefile.am

## stuff to build composite include files
include ../Make/Makefile.inc
BUILT_SOURCES = display.h interface.h
VWWx_HEADERS = $(BUILT_SOURCES)


## Construct the meta-include file
../vwwx.h: $(BUILT_SOURCES)
	( echo "#ifndef _`echo $@ | sed s/\\\./_/`_"; \
	    echo "#define _`echo $@ | sed s/\\\./_/`_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VWWx/$$i\""); done; \
	    echo; \
	    echo "#endif //_`echo $@ | sed s/\\\./_/`_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vwwx.h
