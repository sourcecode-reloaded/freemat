!include <../../../make.inc>

#######################################################################
#
#  Windows NT Digital Fortran
#
#  This is the makefile to create a library of the eigensystem routines
#  from LAPACK that have been instrumented to count operations. 
#  The files are organized as follows:
#
#     SCIGSRC -- Single precision instrumented LAPACK routines used in
#                REAL and COMPLEX
#     DZIGSRC -- Double precision instrumented LAPACK routines used in
#                DOUBLE PRECISION and COMPLEX*16
#
#     SEIGSRC -- Single precision real instrumented LAPACK routines
#     CEIGSRC -- Single precision complex instrumented LAPACK routines
#     DEIGSRC -- Double precision real instrumented LAPACK routines
#     ZEIGSRC -- Double precision complex instrumented LAPACK routines
#
#  The library can be set up to include routines for any combination
#  of the four precisions.  First, modify the FORTRAN and OPTS 
#  definitions to match your compiler and the options to be used.
#  Then to create or add to the library, enter make followed by one or
#  more of the precisions desired.  Some examples:
#       make single
#       make single complex
#       make single double complex complex16
#  Alternatively, the command
#       make
#  without any arguments creates a library of all four precisions.
#  The library is called
#       eigsrc.lib
#  and is created at the next higher directory level.
#
#  To remove the object files after the library is created, enter
#       make clean
#
#######################################################################

.SUFFIXES: .f .obj .lib

SCIGSRC = sbdsdc.obj sbdsqr.obj slaebz.obj slaed0.obj slaed1.obj slaed2.obj slaed3.obj \
	  slaed4.obj slaed5.obj slaed6.obj slaed7.obj slaed8.obj \
	  slaed9.obj slaeda.obj slasd0.obj slasd1.obj slasd2.obj slasd3.obj \
	  slasd4.obj slasd5.obj slasdq.obj slasq1.obj slasq2.obj slasq3.obj slasq4.obj \
	  slasq5.obj slasq6.obj sstebz.obj sstedc.obj ssteqr.obj ssterf.obj

DZIGSRC = dbdsdc.obj dbdsqr.obj dlaebz.obj dlaed0.obj dlaed1.obj dlaed2.obj dlaed3.obj \
	  dlaed4.obj dlaed5.obj dlaed6.obj dlaed7.obj dlaed8.obj \
	  dlaed9.obj dlaeda.obj dlasd0.obj dlasd1.obj dlasd2.obj dlasd3.obj \
	  dlasd4.obj dlasd5.obj dlasdq.obj dlasq1.obj dlasq2.obj dlasq3.obj dlasq4.obj \
	  dlasq5.obj dlasq6.obj dstebz.obj dstedc.obj dsteqr.obj dsterf.obj
 
SEIGSRC = \
   sgesdd.obj sgghrd.obj shgeqz.obj shsein.obj shseqr.obj \
   slaein.obj slahqr.obj slar1v.obj slarrb.obj slarre.obj slarrf.obj slarrv.obj \
   spteqr.obj sstegr.obj sstein.obj stgevc.obj strevc.obj

CEIGSRC = \
   cbdsqr.obj cgesdd.obj cgghrd.obj chgeqz.obj chsein.obj chseqr.obj \
   claed0.obj claed7.obj claed8.obj claein.obj clahqr.obj clar1v.obj clarrv.obj \
   cpteqr.obj cstedc.obj cstegr.obj cstein.obj csteqr.obj ctgevc.obj ctrevc.obj

DEIGSRC = \
   dgesdd.obj dgghrd.obj dhgeqz.obj dhsein.obj dhseqr.obj \
   dlaein.obj dlahqr.obj dlar1v.obj dlarrb.obj dlarre.obj dlarrf.obj dlarrv.obj \
   dpteqr.obj dstegr.obj dstein.obj dtgevc.obj dtrevc.obj

ZEIGSRC = \
   zbdsqr.obj zgesdd.obj zgghrd.obj zhgeqz.obj zhsein.obj zhseqr.obj \
   zlaed0.obj zlaed7.obj zlaed8.obj zlaein.obj zlahqr.obj zlar1v.obj zlarrv.obj \
   zpteqr.obj zstedc.obj zstegr.obj zstein.obj zsteqr.obj ztgevc.obj ztrevc.obj

all: single complex double complex16

single: $(SEIGSRC) $(SCIGSRC)
	$(ARCH) $(ARCHFLAGS)..\$(EIGSRCLIB) $(SEIGSRC) $(SCIGSRC)

complex: $(CEIGSRC) $(SCIGSRC)
	$(ARCH) $(ARCHFLAGS)..\$(EIGSRCLIB) $(CEIGSRC) $(SCIGSRC)

double: $(DEIGSRC) $(DZIGSRC)
	$(ARCH) $(ARCHFLAGS)..\$(EIGSRCLIB) $(DEIGSRC) $(DZIGSRC)

complex16: $(ZEIGSRC) $(DZIGSRC)
	$(ARCH) $(ARCHFLAGS)..\$(EIGSRCLIB) *.obj

clean:
	del *.obj

.f.obj: 
	$(FORTRAN) $(OPTS) -c $<
