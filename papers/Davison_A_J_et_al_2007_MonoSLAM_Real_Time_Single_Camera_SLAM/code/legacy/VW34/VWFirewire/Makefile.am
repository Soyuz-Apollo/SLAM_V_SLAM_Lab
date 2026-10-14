## VWFirewire libVWFirewire.a
##
## process file with automake

lib_LIBRARIES = libVWFirewire.a

INCLUDES = -I../VW

##---files--
libVWFirewire_a_SOURCES = firewirecamera.cpp sequencerfirewire.cpp

Firewire_HEADERS = firewirecamera.h sequencerfirewire.h

Firewiredir = $(includedir)/VWFirewire


## Construct the meta-include file
../vwfirewire.h: $(Firewire_HEADERS)
	( echo "#ifndef _vwfirewire_h_"; \
	    echo "#define _vwfirewire_h_"; \
	    echo; \
	    for i in $^; do (echo "#include \"VWFirewire/$$i\""); done; \
	    echo; \
	    echo "#endif //_vwfirewire_h_" ) > $@

TOPdir = $(includedir)
TOP_HEADERS = ../vwfirewire.h

