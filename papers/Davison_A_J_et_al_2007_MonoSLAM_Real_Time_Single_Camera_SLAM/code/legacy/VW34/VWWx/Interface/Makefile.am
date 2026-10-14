## Interfaces for Wx Graphics
##
## include this file in parent directory Makefile.am


##---Sequencer interfaces--
InterfaceWx_HEADERS = 	Interface/evtsequencernewframe.h \
				Interface/sequencerfilewx.h

libVWWx_a_SOURCES += 	Interface/evtsequencernewframe.cpp \
			Interface/sequencerfilewx.cpp 

if FIREWIRE
InterfaceWx_HEADERS += 	Interface/sequencerfirewirewx.h 
libVWWx_a_SOURCES +=  	Interface/sequencerfirewirewx.cpp 
endif

InterfaceWxdir = $(includedir)/VWWx/Interface

interface.h: $(InterfaceWx_HEADERS)
	$(BUILDRULE)

