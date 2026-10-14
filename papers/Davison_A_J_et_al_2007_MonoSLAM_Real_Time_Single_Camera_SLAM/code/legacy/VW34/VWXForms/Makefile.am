## VWXForms creates libVWXForms.a
##
## process file with automake


VWXFormsdir = $(includedir)/VWXForms

lib_LIBRARIES = libVWXForms.a

libVWXForms_a_SOURCES =

INCLUDES = -I../VW -I../VNL


include Display/Makefile.am
include Interface/Makefile.am

## stuff to build composite include files
include ../Make/Makefile.inc
BUILT_SOURCES = display.h
VWXForms_HEADERS = $(BUILT_SOURCES)



## Construct the meta-include file
../vwxforms.h: $(BUILT_SOURCES)
	( echo "#ifndef _`echo $@ | sed s/\\\./_/`_"; \
	    echo "#define _`echo $@ | sed s/\\\./_/`_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VWXForms/$$i\""); done; \
	    echo; \
	    echo "#endif //_`echo $@ | sed s/\\\./_/`_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vwxforms.h