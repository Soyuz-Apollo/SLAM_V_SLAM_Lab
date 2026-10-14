## Image processing and computer vision algorithms
##
## include this file in parent directory's Makefile.am

Improcdir = $(includedir)/VW/Improc

improc.h: $(Improc_HEADERS)
	$(BUILDRULE)

##---files--

## General
libVW_a_SOURCES += 	Improc/chamferdistance.cpp \
			Improc/classifieruv.cpp \
			Improc/convolve.cpp \
			Improc/correlate.cpp \
			Improc/distortimage.cpp \
			Improc/histogramequalisation.cpp \
			Improc/imagediff.cpp \
			Improc/imagepatch.cpp \
			Improc/imagepatchyuv.cpp \
			Improc/improc_instantiate.cpp \
			Improc/improc_misc.cpp \
			Improc/integralimage.cpp \
			Improc/integralimagesquared.cpp \
			Improc/moments.cpp \
			Improc/normaliseintensity.cpp \
			Improc/parametricflow.cpp \
			Improc/pyramid.cpp \
			Improc/pyramidcorrelate.cpp \
			Improc/scaleimage.cpp \
		  	Improc/smooth.cpp \
			Improc/subsample.cpp \
			Improc/threshold.cpp \
			Improc/warp.cpp 

Improc_HEADERS =	Improc/chamferdistance.h \
	  		Improc/classifieruv.h \
		  	Improc/convolve.h \
			Improc/correlate.h \
	  		Improc/distortimage.h \
			Improc/histogramequalisation.h \
			Improc/imagediff.h \
			Improc/imagepatch.h \
			Improc/imagepatchyuv.h \
			Improc/improc_misc.h \
	  		Improc/integralimage.h \
			Improc/integralimagesquared.h \
			Improc/interpolationmethod.h \
			Improc/moments.h \
			Improc/normaliseintensity.h \
			Improc/parametricflow.h \
	  		Improc/pyramid.h \
			Improc/pyramidcorrelate.h \
			Improc/subsample.h \
	 		Improc/scaleimage.h \
			Improc/smooth.h \
	 		Improc/threshold.h \
			Improc/warp.h 


# Feature detection / matching
#   Detectors
libVW_a_SOURCES += 	Improc/cornerfeature.cpp \
			Improc/plesseydetector.cpp \
			Improc/dogdetector.cpp \
			Improc/shitomasidetector.cpp
Improc_HEADERS += 	Improc/cornerfeature.h \
			Improc/plesseydetector.h \
			Improc/dogdetector.h \
			Improc/shitomasidetector.h
#   Basic matcher
libVW_a_SOURCES += 	Improc/matchdata.cpp Improc/cornermatcherbase.cpp
Improc_HEADERS += 	Improc/matchdata.h Improc/cornermatcherbase.h
#   Old Plessey corners
#libVW_a_SOURCES += Improc/imagepatch.cpp Improc/imagepatchyuv.cpp \
#		   Improc/plesseycorner.cpp Improc/plesseycorneryuv.cpp \
#		   Improc/getplesseycorner.cpp Improc/getplesseycorneryuv.cpp \
#		   Improc/plesseycornermatcher.cpp
#Improc_HEADERS +=  Improc/imagepatch.h Improc/imagepatchyuv.h \
#		   Improc/plesseycorner.h Improc/plesseycorneryuv.h \
#		   Improc/getplesseycorner.h Improc/getplesseycorneryuv.h  \
#		   Improc/plesseycornermatcher.h
#   Patch features
libVW_a_SOURCES += Improc/patchfeature.cpp \
		   Improc/getpatchdescriptor.cpp \
		   Improc/patchfeaturematcher.cpp
Improc_HEADERS +=  Improc/patchfeature.h \
		   Improc/getpatchdescriptor.h \
		   Improc/patchfeaturematcher.h
#   SIFT features
libVW_a_SOURCES += Improc/siftfeature.cpp \
		   Improc/getsiftdescriptor.cpp \
		   Improc/siftfeaturematcher.cpp
Improc_HEADERS +=  Improc/siftfeature.h \
		   Improc/getsiftdescriptor.h \
		   Improc/siftfeaturematcher.h


# Line detection / matching

libVW_a_SOURCES += Improc/edgel.cpp Improc/linesegfitted.cpp \
		Improc/canny.cpp Improc/linefit.cpp 
Improc_HEADERS += Improc/edgel.h Improc/linesegfitted.h \
		Improc/canny.h Improc/linefit.h 
