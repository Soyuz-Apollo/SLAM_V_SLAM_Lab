## Makefile.am for X-Graphics
##
## include this in parent directory's Makefile.am

DisplayXdir = $(includedir)/VWX/Display

## Event loop and processing
DisplayX_HEADERS = Display/xloop.h 
libVWX_a_SOURCES += Display/xloop.cpp


## RawX displays
DisplayX_HEADERS += Display/displayx.h Display/drawx.h 
libVWX_a_SOURCES += Display/displayx.cpp Display/drawx.cpp

display.h: $(DisplayX_HEADERS)
	$(BUILDRULE)






