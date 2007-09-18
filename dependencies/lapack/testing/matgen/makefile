!include <..\..\make.inc>

#######################################################################
#
#  Sample Windows NT makefile (Digital Fortran)
#
#  This is the makefile to create a library of the test matrix
#  generators used in LAPACK.  The files are organized as follows:
#
#     SCATGEN  -- Auxiliary routines called from both REAL and COMPLEX
#     DZATGEN  -- Auxiliary routines called from both DOUBLE PRECISION
#                 and COMPLEX*16
#     SMATGEN  -- Single precision real matrix generation routines
#     CMATGEN  -- Single precision complex matrix generation routines
#     DMATGEN  -- Double precision real matrix generation routines
#     ZMATGEN  -- Double precision complex matrix generation routines
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
#       tmglib.lib
#  and is created at the LAPACK directory level.
#
#  To remove the object files after the library is created, enter
#       make clean
#
#######################################################################

.SUFFIXES: .f .obj

all: single double complex complex16
 
SCATGEN = slatm1.obj slaran.obj slarnd.obj

SMATGEN = slatms.obj slatme.obj slatmr.obj \
   slagge.obj slagsy.obj slakf2.obj slarge.obj slaror.obj slarot.obj slatm2.obj \
   slatm3.obj slatm5.obj slatm6.obj

CMATGEN = clatms.obj clatme.obj clatmr.obj \
   clagge.obj claghe.obj clagsy.obj clakf2.obj clarge.obj claror.obj clarot.obj \
   clatm1.obj clarnd.obj clatm2.obj clatm3.obj clatm5.obj clatm6.obj

DZATGEN = dlatm1.obj dlaran.obj dlarnd.obj

DMATGEN = dlatms.obj dlatme.obj dlatmr.obj \
   dlagge.obj dlagsy.obj dlakf2.obj dlarge.obj dlaror.obj dlarot.obj dlatm2.obj \
   dlatm3.obj dlatm5.obj dlatm6.obj

ZMATGEN = zlatms.obj zlatme.obj zlatmr.obj \
   zlagge.obj zlaghe.obj zlagsy.obj zlakf2.obj zlarge.obj zlaror.obj zlarot.obj \
   zlatm1.obj zlarnd.obj zlatm2.obj zlatm3.obj zlatm5.obj zlatm6.obj


single: $(SMATGEN) $(SCATGEN)
	$(ARCH) $(ARCHFLAGS)..\..\$(TMGLIB) $(SMATGEN) $(SCATGEN)

complex: $(CMATGEN) $(SCATGEN)
	$(ARCH) $(ARCHFLAGS)..\..\$(TMGLIB) $(CMATGEN) $(SCATGEN)

double: $(DMATGEN) $(DZATGEN)
	$(ARCH) $(ARCHFLAGS)..\..\$(TMGLIB) $(DMATGEN) $(DZATGEN)

complex16: $(ZMATGEN) $(DZATGEN)
	$(ARCH) $(ARCHFLAGS)..\..\$(TMGLIB) *.obj

clean:
	del *.obj

.f.obj:
	$(FORTRAN) $(OPTS) -c $*.f
