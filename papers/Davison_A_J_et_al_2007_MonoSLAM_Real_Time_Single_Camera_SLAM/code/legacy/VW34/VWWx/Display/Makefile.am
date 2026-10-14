## Makefile.am for Wx-Graphics
##
## include this file in parent directory Makefile.am


## General
DisplayWx_HEADERS = Display/vwdrawevent.h Display/vwmouseevents.h
libVWWx_a_SOURCES += Display/vwdrawevent.cpp Display/vwmouseevents.cpp


## GL reliant stuff

#if WXGL

DisplayWx_HEADERS += 	Display/displaywxglwidget.h \
			Display/drawwxgl.h \
	          	Display/threedtoolwxglwidget.h

libVWWx_a_SOURCES += 	Display/displaywxglwidget.cpp \
	            	Display/threedtoolwxglwidget.cpp

#endif


DisplayWxdir = $(includedir)/VWWx/Display

display.h: $(DisplayWx_HEADERS)
	$(BUILDRULE)
