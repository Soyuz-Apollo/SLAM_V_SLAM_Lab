## VW Image definitions
##
## include this file in parent directory's Makefile.am

Imagedir = $(includedir)/VW/Image

image.h: $(Image_HEADERS)
	$(BUILDRULE)

##---Pixel classes---
Image_HEADERS =	Image/pixelmono.h   Image/pixelhsv.h   Image/pixelrgb.h   Image/pixelyuv.h
libVW_a_SOURCES += Image/pixelmono.cpp Image/pixelhsv.cpp Image/pixelrgb.cpp Image/pixelyuv.cpp

#---Image classes---
Image_HEADERS += Image/roi.h Image/imagebase.h Image/pixptr.h \
		Image/imagemono.h Image/imagehsv.h Image/imagergb.h \
		Image/imageyuv.h Image/imageinterleave.h Image/imagergbplanes.h
libVW_a_SOURCES += Image/imagemono.cpp Image/imagehsv.cpp Image/imagergb.cpp \
		Image/imageyuv.cpp Image/imageinterleave.cpp Image/imagergbplanes.cpp

##---Image copying---
Image_HEADERS += 	Image/pixelconversions.h \
			Image/imageconversions.h \
			Image/imagecopy.h Image/imagecopy.tpp
libVW_a_SOURCES += 	Image/pixelconversions.cpp \
			Image/imageconversions.cpp \
			Image/imagecopy.cpp

##---Image IO---
Image_HEADERS += 	Image/imageio.h \
			Image/imageio_ppm.h Image/imageio_ppm.tpp
libVW_a_SOURCES += 	Image/imageio.cpp \
			Image/imageio_ppm.cpp



