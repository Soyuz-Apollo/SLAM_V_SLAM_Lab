## Interafces for GL graphics
##
## process with automake


## General gui stuff
InterfaceGL_HEADERS = Interface/label.h \
		Interface/button.h \
		Interface/checkbox.h \
		Interface/colour.h \
		Interface/container.h \
		Interface/hbox.h \
		Interface/vbox.h \
		Interface/frame.h \
		Interface/slider.h \
		Interface/theme.h \
		Interface/widget.h \
		Interface/window.h

libVWGL_a_SOURCES +=	Interface/label.cpp \
		Interface/button.cpp \
		Interface/checkbox.cpp \
		Interface/container.cpp \
		Interface/hbox.cpp \
		Interface/vbox.cpp \
		Interface/frame.cpp \
		Interface/slider.cpp \
		Interface/theme.cpp \
		Interface/widget.cpp \
		Interface/window.cpp


# VW displays
InterfaceGL_HEADERS += Interface/displayglutewidget.h \
	Interface/threedtoolglutewidget.h
libVWGL_a_SOURCES += Interface/displayglutewidget.cpp \
	Interface/threedtoolglutewidget.cpp

# VW widgets
InterfaceGL_HEADERS += Interface/sequencerfileglute.h
libVWGL_a_SOURCES += Interface/sequencerfileglute.cpp
if FIREWIRE
InterfaceGL_HEADERS += Interface/sequencerfirewireglute.h
libVWGL_a_SOURCES += Interface/sequencerfirewireglute.cpp
endif


InterfaceGLdir = $(includedir)/VWGL/Interface

interface.h: $(InterfaceGL_HEADERS)
	$(BUILDRULE)

