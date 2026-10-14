## Interfaces for GTK Graphics
##
## include this file in parent directory Makefile.am

##---The parent interface class interfaces--
InterfaceGTK_HEADERS = Interface/interfacegtk.h
libVWGTK_a_SOURCES += Interface/interfacegtk.cpp

##---Parameter interfaces--
InterfaceGTK_HEADERS += Interface/classifieruvgtk.h
libVWGTK_a_SOURCES += Interface/classifieruvgtk.cpp

##---Sequencer interfaces--
InterfaceGTK_HEADERS += Interface/sequencermoviegtk.h \
			Interface/sequencerfilegtk.h
libVWGTK_a_SOURCES += Interface/sequencermoviegtk.cpp \
			Interface/sequencerfilegtk.cpp

##---Firewire interfaces (need firewire headers)---
if FIREWIRE
  InterfaceGTK_HEADERS += 	Interface/firewirecameragtk.h \
				Interface/sequencerfirewiregtk.h
  libVWGTK_a_SOURCES += 	Interface/firewirecameragtk.cpp \
				Interface/sequencerfirewiregtk.cpp
endif

##---Yorick interface
InterfaceGTK_HEADERS += Interface/yorickgtk.h
libVWGTK_a_SOURCES += Interface/yorickgtk.cpp



InterfaceGTKdir = $(includedir)/VWGTK/Interface

interface.h: $(InterfaceGTK_HEADERS)
	$(BUILDRULE)

