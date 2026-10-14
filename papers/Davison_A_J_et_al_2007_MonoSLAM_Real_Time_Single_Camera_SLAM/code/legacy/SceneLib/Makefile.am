# Process this file with automake to create Makefile.in
SUBDIRS = Scene SceneImproc MonoSLAM

EXTRA_DIST = Doxyfile

# There is no default target for doxygen, so write our own
doxygen-run: Doxyfile 
	doxygen Doxyfile
	
docs: doxygen-run
