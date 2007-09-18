!include <..\..\..\make.inc>

#######################################################################
#  This is the makefile to create a library of the least squares
#  routines from LAPACK that have been instrumented to time specific
#  parts of the code and count operations. 
#  The files are organized as follows:
#
#     SLINSRC -- Single precision real instrumented LAPACK routines
#     CLINSRC -- Single precision complex instrumented LAPACK routines
#     DLINSRC -- Double precision real instrumented LAPACK routines
#     ZLINSRC -- Double precision complex instrumented LAPACK routines
#
#  The library can be set up to include routines for any combination
#  of the four precisions.  To create or add to the library, enter make
#  followed by one or more of the precisions desired.  Some examples:
#       make single
#       make single complex
#       make single double complex complex16
#  Alternatively, the command
#       make
#  without any arguments creates a library of all four precisions.
#  The library is called
#       linsrc.a
#  and is created at the next higher directory level.
#
#  To remove the object files after the library is created, enter
#       make clean
#
#######################################################################

.SUFFIXES: .f .obj

SCAUX = sopla.obj sopla2.obj

DZAUX = dopla.obj dopla2.obj
 
SLINSRC = \
   sgels.obj  sgelsd.obj sgelss.obj sgelsx.obj sgelsy.obj slaic1.obj slals0.obj \
   slalsa.obj slalsd.obj

CLINSRC = \
   cgels.obj  cgelsd.obj cgelss.obj cgelsx.obj cgelsy.obj claic1.obj clals0.obj \
   clalsa.obj clalsd.obj

DLINSRC = \
   dgels.obj  dgelsd.obj dgelss.obj dgelsx.obj dgelsy.obj dlaic1.obj dlals0.obj \
   dlalsa.obj dlalsd.obj

ZLINSRC = \
   zgels.obj  zgelsd.obj zgelss.obj zgelsx.obj zgelsy.obj zlaic1.obj zlals0.obj \
   zlalsa.obj zlalsd.obj

all: single complex double complex16

single: $(SLINSRC) $(SCAUX)
	$(ARCH) $(ARCHFLAGS)..\$(LINSRCLIB) $(SLINSRC) $(SCAUX)

complex: $(CLINSRC) $(SCAUX)
	$(ARCH) $(ARCHFLAGS)..\$(LINSRCLIB) $(CLINSRC) $(SCAUX)

double: $(DLINSRC) $(DZAUX)
	$(ARCH) $(ARCHFLAGS)..\$(LINSRCLIB) $(DLINSRC) $(DZAUX)

complex16: $(ZLINSRC) $(DZAUX)
	$(ARCH) $(ARCHFLAGS)..\$(LINSRCLIB) *.obj

clean:
	del *.obj

.f.obj: 
	$(FORTRAN) $(OPTS) -c $<
