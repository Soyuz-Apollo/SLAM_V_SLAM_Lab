## Makefile.am for GTK-Graphics
##
## include this file in parent directory Makefile.am


## GTK displays
DisplayGTK_HEADERS = 	Display/displaygtkcommon.h \
			Display/displaygtkwidget.h \
			Display/displaygtk.h \
			Display/drawgtk.h 
libVWGTK_a_SOURCES += 	Display/displaygtkwidget.cpp \
			Display/displaygtk.cpp \
			Display/drawgtk.cpp


## GTKGL displays
if GTKGLMM
  DisplayGTK_HEADERS += 	Display/threedtoolgtkglwidget.h \
	           		Display/displaygtkglwidget.h \
				Display/displaygtkgl.h
  libVWGTK_a_SOURCES +=		Display/threedtoolgtkglwidget.cpp \
	             		Display/displaygtkglwidget.cpp \
				Display/displaygtkgl.cpp
endif


DisplayGTKdir = $(includedir)/VWGTK/Display

display.h: $(DisplayGTK_HEADERS)
	$(BUILDRULE)
