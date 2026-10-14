## Base graphical support
##
## include this file in parent directory's Makefile.am

Graphicsdir = $(includedir)/VW/Graphics

graphics.h: $(Graphics_HEADERS)
	$(BUILDRULE)

##---Displays--
Graphics_HEADERS = Graphics/displaybase.h \
		Graphics/pscanvas.h

libVW_a_SOURCES += Graphics/displaybase.cpp \
		Graphics/pscanvas.cpp


##---Drawing--
Graphics_HEADERS += Graphics/draw.h \
		Graphics/drawimage.h \
		Graphics/drawps.h \
		Graphics/drawtypes.h

libVW_a_SOURCES += Graphics/draw.cpp \
		Graphics/drawimage.cpp \
		Graphics/drawps.cpp
