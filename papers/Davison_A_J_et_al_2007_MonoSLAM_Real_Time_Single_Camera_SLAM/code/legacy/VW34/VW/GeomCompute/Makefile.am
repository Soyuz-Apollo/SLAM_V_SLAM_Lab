## VW Geometry computations
##
## include this file in parent directory's Makefile.am

GeomComputedir = $(includedir)/VW/GeomCompute

geomcompute.h: $(GeomCompute_HEADERS)
	$(BUILDRULE)

##---files--

##   Misc
GeomCompute_HEADERS =	GeomCompute/computefuncs.h \
			GeomCompute/convexhull2d.h \
			GeomCompute/delaunay.h \
			GeomCompute/mlesacdefs.h \
			GeomCompute/mlesacfuncs.h \
			GeomCompute/sampling.h 
libVW_a_SOURCES += 	GeomCompute/convexhull2d.cpp \
			GeomCompute/delaunay.cpp \
			GeomCompute/sampling.cpp

##   Radial Distortion
GeomCompute_HEADERS += 	GeomCompute/radialdistortion.h \
			GeomCompute/radialdistortioncompute.h
libVW_a_SOURCES += 	GeomCompute/radialdistortion.cpp \
			GeomCompute/radialdistortioncompute.cpp

##   Line2D
GeomCompute_HEADERS += 	GeomCompute/line2dcompute.h \
			GeomCompute/line2dcomputelinear.h \
			GeomCompute/line2dcomputemlesac.h \
			GeomCompute/line2dcomputeguisac.h
libVW_a_SOURCES += 	GeomCompute/line2dcomputelinear.cpp \
			GeomCompute/line2dcomputemlesac.cpp \
			GeomCompute/line2dcomputeguisac.cpp

##   Homography
GeomCompute_HEADERS += GeomCompute/homography2dcomputelinear.h \
	GeomCompute/homography2dcomputenonlinear.h \
	GeomCompute/homography2dcomputemlesac.h \
	GeomCompute/homography2dcomputeguisac.h \
	GeomCompute/homography2dcomputegunsac.h
libVW_a_SOURCES += GeomCompute/homography2dcomputelinear.cpp \
	GeomCompute/homography2dcomputenonlinear.cpp \
	GeomCompute/homography2dcomputemlesac.cpp \
	GeomCompute/homography2dcomputeguisac.cpp \
	GeomCompute/homography2dcomputegunsac.cpp

##   F matrix
GeomCompute_HEADERS += GeomCompute/fmatrixcompute.h \
	GeomCompute/fmatrixcompute7point.h \
	GeomCompute/fmatrixcomputelinear.h \
	GeomCompute/fmatrixcomputenonlinear.h \
	GeomCompute/fmplanarcomputenonlinear.h \
	GeomCompute/fmatrixcomputemlesac.h \
	GeomCompute/fmatrixcomputegunsac.h \
	GeomCompute/fmaffinecomputeguisac.h \
	GeomCompute/fmaffinecomputelinear.h \
	GeomCompute/fmaffinecomputepreemptive.h \
	GeomCompute/fmaffinecompute.h \
	GeomCompute/fmaffinecomputemlesac.h

libVW_a_SOURCES += GeomCompute/fmatrixcompute7point.cpp \
	GeomCompute/fmatrixcomputelinear.cpp \
	GeomCompute/fmatrixcomputenonlinear.cpp \
	GeomCompute/fmplanarcomputenonlinear.cpp \
	GeomCompute/fmatrixcomputemlesac.cpp \
	GeomCompute/fmatrixcomputegunsac.cpp \
	GeomCompute/fmaffinecomputeguisac.cpp \
	GeomCompute/fmaffinecomputemlesac.cpp \
	GeomCompute/fmaffinecomputelinear.cpp \
	GeomCompute/fmaffinecomputepreemptive.cpp

##   ProjectionMatrix
GeomCompute_HEADERS += GeomCompute/pmatrixcomputelinear.h \
	GeomCompute/pmatrixcomputenonlinear.h
libVW_a_SOURCES += GeomCompute/pmatrixcomputelinear.cpp \
	GeomCompute/pmatrixcomputenonlinear.cpp

##   Pose computation
GeomCompute_HEADERS += GeomCompute/threepointpose.h
libVW_a_SOURCES += GeomCompute/threepointpose.cpp

##   Essential matrix
GeomCompute_HEADERS += GeomCompute/essentialmatrixcompute5point.h \
			GeomCompute/essentialmatrixcomputemlesac.h \
			GeomCompute/essentialmatrixcompute.h \
			GeomCompute/essentialmatrixcomputepreemptive.h \
			GeomCompute/essentialmatrixcomputelinear.h
libVW_a_SOURCES += GeomCompute/essentialmatrixcompute5point.cpp \
			GeomCompute/essentialmatrixcomputemlesac.cpp \
			GeomCompute/essentialmatrixcomputelinear.cpp \
			GeomCompute/essentialmatrixcomputepreemptive.cpp

##GeomCompute_HEADERS += GeomCompute/computemlesac.h GeomCompute/computelinear.h GeomCompute/computenonlinear.h \
##	 GeomCompute/homography2d_computemlesac.h GeomCompute/homography2d_computelinear.h \
##	GeomCompute/h2d_computenonlinear.h \
##	GeomCompute/fmatrix_computelinear.h \
##	GeomCompute/fmatrix_computemlesac.h \
##	GeomCompute/fmatrix_computenonlinear.h \
##	GeomCompute/fmplanar_computenonlinear.h \
##	GeomCompute/fmatrix_compute7point.h
##libVW_a_SOURCES += GeomCompute/computemlesac.cpp \
##	GeomCompute/computelinear.cpp \
##	GeomCompute/computenonlinear.cpp \
##	GeomCompute/homography2d_computemlesac.cpp \
##	GeomCompute/homography2d_computelinear.cpp \
##	GeomCompute/h2d_computenonlinear.cpp \
##	GeomCompute/fmatrix_computelinear.cpp \
##	GeomCompute/fmatrix_computemlesac.cpp \
##	GeomCompute/fmatrix_computenonlinear.cpp \
##	GeomCompute/fmplanar_computenonlinear.cpp \
##	GeomComnpute/fmatrix_compute7point.cpp

