## VW maths stuff 
##
## include this file in parent directory's Makefile.am

MathBitsdir = $(includedir)/VW/MathBits

mathbits.h: $(nobase_Math_HEADERS)
	$(BUILDRULE)

##---files---

MathBits_HEADERS = 	MathBits/atan2.h \
			MathBits/exp.h \
			MathBits/nan.h \
			MathBits/chebyshevpolynomial.h \
			MathBits/kalmanfilter.h \
			MathBits/random.h
MathBits_HEADERS += 	MathBits/kalmanfilter.tpp \
			MathBits/kalmanfilterwithinput.tpp	
libVW_a_SOURCES += 	MathBits/atan2.cpp \
			MathBits/nan.cpp \
			MathBits/chebyshevpolynomial.cpp \
			MathBits/kalmanfilter.cpp \
			MathBits/kalmanfilterwithinput.cpp


#---Histograms---
MathBits_HEADERS += 	MathBits/histogram.h \
			MathBits/histogram2d.h

libVW_a_SOURCES +=	MathBits/histogram.cpp \
			MathBits/histogram2d.cpp


#---Covariances---
MathBits_HEADERS += 	MathBits/covariance2d.h \
			MathBits/covariance3d.h

libVW_a_SOURCES +=	MathBits/covariance2d.cpp \
			MathBits/covariance3d.cpp


#---PDF's---
MathBits_HEADERS += 	MathBits/pdfbase1d.h \
			MathBits/pdfbase2d.h \
			MathBits/pdfbase2diso.h \
	 		MathBits/pdfuniform1d.h \
			MathBits/pdfexponential.h \
			MathBits/pdfrayleigh.h \
	 		MathBits/pdfgaussian1d.h \
			MathBits/pdfgaussian2d.h \
			MathBits/pdfgaussian2diso.h

libVW_a_SOURCES += 	MathBits/pdfbase1d.cpp \
			MathBits/pdfbase2d.cpp \
			MathBits/pdfbase2diso.cpp \
	 		MathBits/pdfuniform1d.cpp \
			MathBits/pdfexponential.cpp \
			MathBits/pdfrayleigh.cpp \
	 		MathBits/pdfgaussian1d.cpp \
			MathBits/pdfgaussian2d.cpp \
			MathBits/pdfgaussian2diso.cpp
