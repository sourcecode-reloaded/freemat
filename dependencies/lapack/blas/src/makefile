!include <..\..\make.inc>

#######################################################################
#
#  Sample Windows NT makefile (Digital Fortran)
#
#  This is the makefile to create a library for the BLAS.
#  The files are grouped as follows:
#
#       SBLAS1 -- Single precision real BLAS routines
#       CBLAS1 -- Single precision complex BLAS routines
#       DBLAS1 -- Double precision real BLAS routines
#       ZBLAS1 -- Double precision complex BLAS routines
#
#       CB1AUX -- Real BLAS routines called by complex routines
#       ZB1AUX -- D.P. real BLAS routines called by d.p. complex
#                 routines
#
#      ALLBLAS -- Auxiliary routines for Level 2 and 3 BLAS
#
#       SBLAS2 -- Single precision real BLAS2 routines
#       CBLAS2 -- Single precision complex BLAS2 routines
#       DBLAS2 -- Double precision real BLAS2 routines
#       ZBLAS2 -- Double precision complex BLAS2 routines
#
#       SBLAS3 -- Single precision real BLAS3 routines
#       CBLAS3 -- Single precision complex BLAS3 routines
#       DBLAS3 -- Double precision real BLAS3 routines
#       ZBLAS3 -- Double precision complex BLAS3 routines
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
#       blas.lib
#
#  To remove the object files after the library is created, enter
#       make clean
#
#---------------------------------------------------------------------
#
#  Susan Blackford, August, 1999
#
#######################################################################

.SUFFIXES: .f .obj

all: single double complex complex16
 
#---------------------------------------------------------
#  Comment out the next 6 definitions if you already have
#  the Level 1 BLAS.
#---------------------------------------------------------
SBLAS1 = isamax.obj sasum.obj saxpy.obj scopy.obj sdot.obj snrm2.obj \
	srot.obj srotg.obj sscal.obj sswap.obj

CBLAS1 = scasum.obj scnrm2.obj icamax.obj caxpy.obj ccopy.obj \
	cdotc.obj cdotu.obj csscal.obj crotg.obj cscal.obj cswap.obj

DBLAS1 = idamax.obj dasum.obj daxpy.obj dcopy.obj ddot.obj dnrm2.obj \
	drot.obj drotg.obj dscal.obj dswap.obj

ZBLAS1 = dcabs1.obj dzasum.obj dznrm2.obj izamax.obj zaxpy.obj zcopy.obj \
	zdotc.obj zdotu.obj zdscal.obj zrotg.obj zscal.obj zswap.obj

CB1AUX = isamax.obj sasum.obj saxpy.obj scopy.obj snrm2.obj sscal.obj

ZB1AUX = idamax.obj dasum.obj daxpy.obj dcopy.obj dnrm2.obj dscal.obj

#---------------------------------------------------------------------
#  The following line defines auxiliary routines needed by both the
#  Level 2 and Level 3 BLAS.  Comment it out only if you already have
#  both the Level 2 and 3 BLAS.
#---------------------------------------------------------------------
ALLBLAS  = lsame.obj xerbla.obj

#---------------------------------------------------------
#  Comment out the next 4 definitions if you already have
#  the Level 2 BLAS.
#---------------------------------------------------------
SBLAS2 = sgemv.obj sgbmv.obj ssymv.obj ssbmv.obj sspmv.obj \
	strmv.obj stbmv.obj stpmv.obj strsv.obj stbsv.obj stpsv.obj \
	sger.obj ssyr.obj sspr.obj ssyr2.obj sspr2.obj

CBLAS2 = cgemv.obj cgbmv.obj chemv.obj chbmv.obj chpmv.obj \
	ctrmv.obj ctbmv.obj ctpmv.obj ctrsv.obj ctbsv.obj ctpsv.obj \
	cgerc.obj cgeru.obj cher.obj chpr.obj cher2.obj chpr2.obj

DBLAS2 = dgemv.obj dgbmv.obj dsymv.obj dsbmv.obj dspmv.obj \
	dtrmv.obj dtbmv.obj dtpmv.obj dtrsv.obj dtbsv.obj dtpsv.obj \
	dger.obj dsyr.obj dspr.obj dsyr2.obj dspr2.obj

ZBLAS2 = zgemv.obj zgbmv.obj zhemv.obj zhbmv.obj zhpmv.obj \
	ztrmv.obj ztbmv.obj ztpmv.obj ztrsv.obj ztbsv.obj ztpsv.obj \
	zgerc.obj zgeru.obj zher.obj zhpr.obj zher2.obj zhpr2.obj

#---------------------------------------------------------
#  Comment out the next 4 definitions if you already have
#  the Level 3 BLAS.
#---------------------------------------------------------
SBLAS3 = sgemm.obj ssymm.obj ssyrk.obj ssyr2k.obj strmm.obj strsm.obj 

CBLAS3 = cgemm.obj csymm.obj csyrk.obj csyr2k.obj ctrmm.obj ctrsm.obj \
	chemm.obj cherk.obj cher2k.obj

DBLAS3 = dgemm.obj dsymm.obj dsyrk.obj dsyr2k.obj dtrmm.obj dtrsm.obj

ZBLAS3 = zgemm.obj zsymm.obj zsyrk.obj zsyr2k.obj ztrmm.obj ztrsm.obj \
	zhemm.obj zherk.obj zher2k.obj


single: $(SBLAS1) $(ALLBLAS) $(SBLAS2) $(SBLAS3)
	$(ARCH) $(ARCHFLAGS)$(BLASLIB) $(SBLAS1) $(ALLBLAS) \
	$(SBLAS2) $(SBLAS3)

double: $(DBLAS1) $(ALLBLAS) $(DBLAS2) $(DBLAS3)
	$(ARCH) $(ARCHFLAGS)$(BLASLIB) $(DBLAS1) $(ALLBLAS) \
	$(DBLAS2) $(DBLAS3)

complex: $(CBLAS1) $(CB1AUX) $(ALLBLAS) $(CBLAS2) $(CBLAS3)
	$(ARCH) $(ARCHFLAGS)$(BLASLIB) $(CBLAS1) $(CB1AUX) $(ALLBLAS) \
	$(CBLAS2) $(CBLAS3)

complex16: $(ZBLAS1) $(ZB1AUX) $(ALLBLAS) $(ZBLAS2) $(ZBLAS3)
	$(ARCH) $(ARCHFLAGS)$(BLASLIB) *.obj

clean:
	del *.obj

.f.obj: 
	$(FORTRAN) $(OPTS) -c $*.f
