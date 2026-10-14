## VW top level 
##
## process this file with automake to generate all Makefile.in files

# First the core libraries
SUBDIRS = VNL \
			VW


# Now the optional GUIs
if X
  SUBDIRS +=	VWX
endif

if XFORMS
  SUBDIRS +=	VWXForms 
endif

if GL
  SUBDIRS +=	VWGL
endif

if GTKMM
  SUBDIRS +=	VWGTK
endif

if WX
  SUBDIRS +=	VWWx
endif

if GLOW
  SUBDIRS +=	VWGLOW
endif

if QT
  SUBDIRS +=	VWQt
endif




# Now the optional hardware
SUBDIRS +=	VWMeteor \
		VWYorick

if FIREWIRE
  SUBDIRS +=	VWFirewire 
endif








