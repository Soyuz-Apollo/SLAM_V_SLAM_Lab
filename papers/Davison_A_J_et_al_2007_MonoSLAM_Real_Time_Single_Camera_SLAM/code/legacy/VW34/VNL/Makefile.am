## libVNL.a headers and sources 
##
## process this file with automake to generate Makefile.in
##
## note includes at end of file drawing source and headers from 
## ./Templates ./Algo and ./Algo/Templates

lib_LIBRARIES =	libVNL.a

VNLdir = $(includedir)/VNL

INCLUDES = -I..

## Basics
nobase_VNL_HEADERS =	alloc.h bignum.h copy.h tag.h error.h T_n.h \
		config.h VC6WarningsFix.h VC6StdFix.h
libVNL_a_SOURCES =	alloc.cpp bignum.cpp copy.cpp error.cpp

## Number stuff

nobase_VNL_HEADERS +=	numerictraits.h numericlimits.h rational.h vnlmath.h
libVNL_a_SOURCES +=	numerictraits.cpp numericlimits.cpp rational.cpp vnlmath.cpp


## Complex

nobase_VNL_HEADERS +=	complextraits.h complexify.h complex.h \
		real.h imag.h
nobase_VNL_HEADERS += 	complexops.tpp 
libVNL_a_SOURCES +=	complextraits.cpp complex.cpp

## Vector classes

nobase_VNL_HEADERS +=	cvector.h vector.h vectorref.h vectorfixedref.h vectorfixed.h \
		filevector.h
nobase_VNL_HEADERS += 	cvector.tpp vector.tpp vectorref.tpp \
		vectorfixedref.tpp vectorfixed.tpp \
		filevector.tpp

## Matrix classes

nobase_VNL_HEADERS +=	matrixref.h matrix.h matrixfixedref.h matrixfixed.h \
		sparsematrix.h filematrix.h crossproductmatrix.h \
		identity.h diagmatrix.h matops.h symmatrix.h
nobase_VNL_HEADERS += 	matrix.tpp \
		matrixfixedref.tpp matrixfixed.tpp \
		sparsematrix.tpp filematrix.tpp diagmatrix.tpp symmatrix.tpp
libVNL_a_SOURCES +=	crossproductmatrix.cpp matops.cpp



## Basic matrix ops
nobase_VNL_HEADERS +=	trace.h transpose.h \
		det.h matrixexp.h fortrancopy.h scalarjoiniterator.h
nobase_VNL_HEADERS += 	matrixexp.tpp det.tpp fortrancopy.tpp \
		scalarjoiniterator.tpp
libVNL_a_SOURCES +=	trace.cpp transpose.cpp



## Algorithmic
nobase_VNL_HEADERS +=	unaryfunction.h linearoperators3.h linearsystem.h \
		costfunction.h nonlinearminimizer.h \
		realnpolynomial.h realpolynomial.h \
		leastsquarescostfunction.h leastsquaresfunction.h \
		fastops.h sparsematrixlinearsystem.h sample.h
libVNL_a_SOURCES +=	unaryfunction.cpp linearoperators3.cpp linearsystem.cpp \
		costfunction.cpp nonlinearminimizer.cpp \
		realnpolynomial.cpp realpolynomial.cpp \
		leastsquarescostfunction.cpp leastsquaresfunction.cpp \
		fastops.cpp sparsematrixlinearsystem.cpp sample.cpp

## Matlab compat
nobase_VNL_HEADERS +=	matlabfilewrite.h matlabheader.h matlabprint2.h \
		matlabprint.h matlabprintformat.h matlabprintscalar.h \
		matlabread.h matlabwrite.h
nobase_VNL_HEADERS += 	matlabprint.tpp 
libVNL_a_SOURCES +=	matlabfilewrite.cpp matlabheader.cpp matlabprint2.cpp \
		matlabprint.cpp matlabprintformat.cpp matlabprintscalar.cpp \
		matlabread.cpp matlabwrite.cpp



include Templates/Makefile.am

include NetLib/Makefile.am
include Algo/Makefile.am

include Algo/Templates/Makefile.am
