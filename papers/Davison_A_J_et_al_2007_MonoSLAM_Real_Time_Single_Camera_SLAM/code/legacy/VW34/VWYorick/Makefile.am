## VWYorick creates libVYorick.a for controlling Yorick via PMAC
##
## process file with automake

lib_LIBRARIES = libVWYorick.a

INCLUDES = -I../VW

Yorickdir = $(includedir)/VWYorick

##---files--
Yorick_HEADERS = 	yorickmain.h \
			yorickpmac.h \
			yorickpmacaddress.h
libVWYorick_a_SOURCES = yorickmain.cpp \
			yorickpmac.cpp


## Construct the meta-include file
../vwyorick.h: $(Yorick_HEADERS)
	( echo "#ifndef _`echo $@ | sed s/\\\./_/`_"; \
	    echo "#define _`echo $@ | sed s/\\\./_/`_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VWYorick/$$i\""); done; \
	    echo; \
	    echo "#endif //_`echo $@ | sed s/\\\./_/`_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vwyorick.h
