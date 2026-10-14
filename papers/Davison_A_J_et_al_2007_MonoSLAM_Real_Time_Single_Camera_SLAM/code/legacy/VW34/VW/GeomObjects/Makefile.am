## VW Geometry objects
##
## include this file in parent directory's Makefile.am

GeomObjectsdir = $(includedir)/VW/GeomObjects

geomobjects.h: $(GeomObjects_HEADERS)
	$(BUILDRULE)

##---files--

##   Entities
GeomObjects_HEADERS = 	GeomObjects/quaternion.h GeomObjects/angleaxis.h GeomObjects/rotationmatrix.h \
		GeomObjects/point3d.h GeomObjects/point2d.h \
		GeomObjects/line2d.h GeomObjects/lineseg2d.h \
		GeomObjects/lineseg3d.h \
		GeomObjects/trajectorymap.h GeomObjects/calibrationmatrix.h \
		GeomObjects/calibrationtable.h \
		GeomObjects/essentialmatrix.h
libVW_a_SOURCES += GeomObjects/quaternion.cpp \
	GeomObjects/angleaxis.cpp \
	GeomObjects/rotationmatrix.cpp \
	GeomObjects/point3d.cpp GeomObjects/point2d.cpp \
	GeomObjects/line2d.cpp GeomObjects/lineseg2d.cpp GeomObjects/lineseg3d.cpp \
	GeomObjects/calibrationmatrix.cpp GeomObjects/calibrationtable.cpp \
	GeomObjects/essentialmatrix.cpp


##   Miscellaneous functions
GeomObjects_HEADERS += GeomObjects/operations.h GeomObjects/generatedata.h
libVW_a_SOURCES += GeomObjects/operations.cpp GeomObjects/generatedata.cpp

##   Transforms
GeomObjects_HEADERS += GeomObjects/transform.h GeomObjects/homography2d.h GeomObjects/fmatrix.h GeomObjects/fmatrixaffine.h GeomObjects/fmatrixplanar.h GeomObjects/projectionmatrix.h
libVW_a_SOURCES +=  GeomObjects/homography2d.cpp GeomObjects/fmatrix.cpp GeomObjects/fmatrixaffine.cpp GeomObjects/fmatrixplanar.cpp GeomObjects/projectionmatrix.cpp

##   Computations
GeomObjects_HEADERS += GeomObjects/normalisation.h GeomObjects/normalisation3d.h
libVW_a_SOURCES += GeomObjects/normalisation.cpp GeomObjects/normalisation3d.cpp

