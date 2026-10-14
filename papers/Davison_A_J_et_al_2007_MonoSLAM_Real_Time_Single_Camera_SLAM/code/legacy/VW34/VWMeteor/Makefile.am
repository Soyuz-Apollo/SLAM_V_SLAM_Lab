## VWMeteor creates libVWMeteor.a
##
## process file with automake

lib_LIBRARIES = libVWMeteor.a

INCLUDES = -I../VW

Meteordir = $(includedir)/VWMeteor

##---files--
libVWMeteor_a_SOURCES = sequencermeteor.cpp
Meteor_HEADERS = 	sequencermeteor.h \
			ioctl_meteor.h


## Construct the meta-include file
../vwmeteor.h: $(Meteor_HEADERS)
	( echo "#ifndef _`echo $@ | sed s/\\\./_/`_"; \
	    echo "#define _`echo $@ | sed s/\\\./_/`_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VWMeteor/$$i\""); done; \
	    echo; \
	    echo "#endif //_`echo $@ | sed s/\\\./_/`_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vwmeteor.h