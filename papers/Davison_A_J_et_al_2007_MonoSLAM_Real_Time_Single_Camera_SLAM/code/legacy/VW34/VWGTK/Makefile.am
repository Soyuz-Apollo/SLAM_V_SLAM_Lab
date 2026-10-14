## VWGTK creates libVWGTK.a
##
## process file with automake

VWGTKdir = $(includedir)/VWGTK

lib_LIBRARIES = libVWGTK.a

INCLUDES = -I../VW -I../VNL -I../VWFirewire `gtkmm-config --cflags`

libVWGTK_a_SOURCES = 

include Display/Makefile.am
include Interface/Makefile.am

## stuff to build composite include files
include ../Make/Makefile.inc
BUILT_SOURCES = display.h interface.h
VWGTK_HEADERS = $(BUILT_SOURCES)


## Construct the meta-include file
../vwgtk.h: $(BUILT_SOURCES)
	( echo "#ifndef _`echo $@ | sed s/\\\./_/`_"; \
	    echo "#define _`echo $@ | sed s/\\\./_/`_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VWGTK/$$i\""); done; \
	    echo; \
	    echo "#endif //_`echo $@ | sed s/\\\./_/`_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vwgtk.h