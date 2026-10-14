## Generic support for xml parameter files
##
## include this file in parent directory's Makefile.am

IOBasicsdir = $(includedir)/VW/IOBasics

iobasics.h: $(IOBasics_HEADERS)
	$(BUILDRULE)

##---files--
libVW_a_SOURCES += IOBasics/paramparser.cpp IOBasics/paramwriter.cpp  \
	 IOBasics/xmlstuff.cpp IOBasics/miscioutils.cpp \
	 IOBasics/ioobject.cpp

IOBasics_HEADERS = IOBasics/paramfiles_general.h IOBasics/paramparser.h \
	 IOBasics/paramwriter.h  \
	 IOBasics/xmlstuff.h IOBasics/miscioutils.h IOBasics/ioobject.h

