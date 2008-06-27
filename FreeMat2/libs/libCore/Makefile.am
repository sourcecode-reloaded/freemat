noinst_LIBRARIES = libCore.a

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libMex -I@top_srcdir@/libs/libXP -I@top_builddir@/libs/libXP -I@top_srcdir@/libs/libMatC

libCore_a_MOC = helpwidget.moc.cpp XML.moc.cpp 

BUILT_SOURCES = $(libCore_a_MOC)

libCore_a_HFILES = Core.hpp \
helpwidget.hpp \
LoadCore.hpp \
MPIWrap.hpp \
RanLib.hpp \
Utils.hpp \
MatIO.hpp \
XML.hpp \
Printf.hpp \
RPC.hpp 

libCore_a_SOURCES = Abs.cpp \
Binary.cpp \
Ceil.cpp \
Conj.cpp \
CumProd.cpp \
CumSum.cpp \
Floor.cpp \
Max.cpp \
Min.cpp \
Prod.cpp \
RCond.cpp \
Round.cpp \
Sum.cpp \
Time.cpp \
Variance.cpp \
RealImag.cpp \
Sort.cpp \
Unique.cpp \
XNorm.cpp \
Analyze.cpp \
Cast.cpp \
Constants.cpp \
Reshape.cpp \
Zeros.cpp \
Ones.cpp \
Cell.cpp \
Struct.cpp \
Directory.cpp \
FFT.cpp \
helpwidget.cpp \
Help.cpp \
Hex.cpp \
Clear.cpp \
Exist.cpp \
Find.cpp \
Is.cpp \
Size.cpp \
Path.cpp \
Inspect.cpp \
Audio.cpp \
Random.cpp \
RanLib.cpp \
Image.cpp \
File.cpp \
Printf.cpp \
IO.cpp \
Conv.cpp \
Eval.cpp \
Debug.cpp \
LU.cpp \
QR.cpp \
Eigen.cpp \
SVD.cpp \
Array.cpp \
Version.cpp \
Error.cpp \
Misc.cpp \
Mean.cpp \
MPIWrap.cpp \
StringOps.cpp \
Transcendental.cpp \
Trigonometric.cpp \
Utils.cpp \
MatIO.cpp \
XML.cpp \
RPC.cpp \
Cstruct.cpp \
LoadCore.cpp \
Control.cpp \
Sparse.cpp \
$(libCore_a_HFILES)

nodist_libCore_a_SOURCES = $(libCore_a_MOC) 

AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(libCore_a_MOC)

resourcedir=$(datadir)/$(PACKAGE)-$(VERSION)

AM_CPPFLAGS = $(QT_CFLAGS) -DRESOURCEDIR=\"$(resourcedir)\"

XML.moc.cpp: XML.hpp
	$(QT_MOC) -o $@ $<

helpwidget.moc.cpp: helpwidget.hpp
	$(QT_MOC) -o $@ $<
