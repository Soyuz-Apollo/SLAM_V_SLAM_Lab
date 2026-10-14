## Makefile.am for GL-Graphics
##
## process with automake to generate Makefile.in

## Event loop for GLUT-based displaying
DisplayGL_HEADERS = Display/glutloop.h Display/glutwindow.h
libVWGL_a_SOURCES += Display/glutloop.cpp Display/glutwindow.cpp

## 2D displays
DisplayGL_HEADERS += Display/displayglcommon.h \
	Display/displayglut.h Display/drawgl.h
libVWGL_a_SOURCES += Display/displayglcommon.cpp \
	Display/displayglut.cpp Display/drawgl.cpp

## 3D displays
DisplayGL_HEADERS += Display/threedtoolglcommon.h \
	Display/threedtoolglut.h 
libVWGL_a_SOURCES += Display/threedtoolglcommon.cpp \
	Display/threedtoolglut.cpp


## Object drawing functions

## Alias WaveFront (AWF) objects
DisplayGL_HEADERS += Display/awfface.h Display/awfedge.h \
	Display/awfmaterial.h Display/awfobject.h
libVWGL_a_SOURCES += Display/awfface.cpp Display/awfedge.cpp \
	Display/awfmaterial.cpp Display/awfobject.cpp

## Cameras
DisplayGL_HEADERS += Display/drawfirei.h Display/drawvl500.h
libVWGL_a_SOURCES += Display/drawfirei.cpp Display/drawvl500.cpp

## Desks
DisplayGL_HEADERS += Display/drawthreequarterdesk.h
libVWGL_a_SOURCES += Display/drawthreequarterdesk.cpp

## Misc hardware
DisplayGL_HEADERS += Display/drawyorick811.h \
	Display/drawgti.h Display/drawwero.h
libVWGL_a_SOURCES += Display/drawyorick811.cpp \
	Display/drawgti.cpp Display/drawwero.cpp



DisplayGLdir = $(includedir)/VWGL/Display

display.h: $(DisplayGL_HEADERS)
	$(BUILDRULE)
