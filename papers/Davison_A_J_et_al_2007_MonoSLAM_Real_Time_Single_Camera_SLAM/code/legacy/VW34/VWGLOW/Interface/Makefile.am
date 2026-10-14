## Interfaces for GLOW Graphics
##
## include this file in parent directory Makefile.am


##---Sequencer interfaces--
InterfaceGLOW_HEADERS = Interface/sequencerfileglow.h

libVWGLOW_a_SOURCES += Interface/sequencerfileglow.cpp

if FIREWIRE
InterfaceGLOW_HEADERS += 	Interface/sequencerfirewireglow.h
libVWGLOW_a_SOURCES +=  	Interface/sequencerfirewireglow.cpp
endif

InterfaceGLOWdir = $(includedir)/VWGLOW/Interface

interface.h: $(InterfaceGLOW_HEADERS)
	$(BUILDRULE)

