#######################################################################
#  This makefile runs the timing programs for the linear equation routines
#  and the eigenvalue routines in LAPACK.  The timing output files
#  are grouped as follows:
#
#       SLINTIM,SEIGTIM  -- Single precision real time routines
#       CLINTIM,CEIGTIM  -- Single precision complex time routines
#       DLINTIM,DEIGTIM  -- Double precision real time routines
#       ZLINTIM,ZEIGTIM  -- Double precision complex time routines
#
#  Timing programs can be executed for all or some of the four different
#  precisions.  Enter 'make' followed by one or more of the data types
#  desired.
#
#  By default, 'make' alone executes the timings with SMALL data sets.
#
#  Some examples:
#       make single
#       make single complex
#       make single double complex complex16
#  Alternatively, the command
#       make
#  without any arguments runs all eight test programs with the SMALL
#  timing data files.
#
#  The executable files are called:
#       xlintims, xlintimd, xlintimc, and xlintimz for LIN
#       xeigtims, xeigtimd, xeigtimc, and xeigtimz for EIG
#  and exist in the current directory level.
#
#  To remove the output files after the tests have been run, enter
#       make clean
#
#  To re-run specific tests after a make, enter (for example):
#       'rm stime.out; make'  or:
#       'make stime.out' or:
#       'touch stime.in; make' (to re-run the timings.)
#
#       'rm *time.out; make'  (to re-run all the timings.)
#
#######################################################################

!include <..\make.inc>

all:     single complex double complex16

SEIGTIM= sgeptim.out \
         sneptim.out \
         sseptim.out \
         ssvdtim.out

CEIGTIM= cgeptim.out \
         cneptim.out \
         cseptim.out \
         csvdtim.out

DEIGTIM= dgeptim.out \
         dneptim.out \
         dseptim.out \
         dsvdtim.out

ZEIGTIM= zgeptim.out \
         zneptim.out \
         zseptim.out \
         zsvdtim.out

SLINTIM= stime.out \
	 sband.out \
	 stime2.out

CLINTIM= ctime.out \
	 cband.out \
	 ctime2.out

DLINTIM= dtime.out \
	 dband.out \
	 dtime2.out

ZLINTIM= ztime.out \
	 zband.out \
	 ztime2.out

single:         $(SLINTIM) $(SEIGTIM)
complex:        $(CLINTIM) $(CEIGTIM)
double:         $(DLINTIM) $(DEIGTIM)
complex16:      $(ZLINTIM) $(ZEIGTIM)

#
# ======== SINGLE LIN TIMINGS ===========================

stime.out: stime.in xlintims.exe
	@echo Timing square REAL LAPACK linear equation routines
	cd $(HOME)\timing
	xlintims.exe < stime.in > $@ 2>&1

sband.out: sband.in xlintims.exe
	@echo Timing banded REAL LAPACK linear equation routines
	cd $(HOME)\timing
	xlintims.exe < sband.in > $@ 2>&1

stime2.out: stime2.in xlintims.exe
	@echo Timing rectangular REAL LAPACK linear equation routines
	cd $(HOME)\timing
	xlintims.exe < stime2.in > $@ 2>&1

#
# ======== COMPLEX LIN TIMINGS ==========================

ctime.out: ctime.in xlintimc.exe
	@echo Timing square COMPLEX LAPACK linear equation routines
	cd $(HOME)\timing
	xlintimc.exe < ctime.in > $@ 2>&1

cband.out: cband.in xlintimc.exe
	@echo Timing banded COMPLEX LAPACK linear equation routines
	cd $(HOME)\timing
	xlintimc.exe < cband.in > $@ 2>&1

ctime2.out: ctime2.in xlintimc.exe
	@echo Timing rectangular COMPLEX LAPACK linear equation routines
	cd $(HOME)\timing
	xlintimc.exe < ctime2.in > $@ 2>&1

#
# ======== DOUBLE LIN TIMINGS ===========================

dtime.out: dtime.in xlintimd.exe
	@echo Timing square DOUBLE PRECISION LAPACK linear equation routines
	cd $(HOME)\timing
	xlintimd.exe < dtime.in > $@ 2>&1

dband.out: dband.in xlintimd.exe
	@echo Timing banded DOUBLE PRECISION LAPACK linear equation routines
	cd $(HOME)\timing
	xlintimd.exe < dband.in > $@ 2>&1

dtime2.out: dtime2.in xlintimd.exe
	@echo Timing rectangular DOUBLE PRECISION LAPACK linear equation routines
	cd $(HOME)\timing
	xlintimd.exe < dtime2.in > $@ 2>&1

#
# ======== COMPLEX16 LIN TIMINGS ========================

ztime.out: ztime.in xlintimz.exe
	@echo Timing square COMPLEX16 LAPACK linear equation routines
	cd $(HOME)\timing
	xlintimz.exe < ztime.in > $@ 2>&1

zband.out: zband.in xlintimz.exe
	@echo Timing banded COMPLEX16 LAPACK linear equation routines
	cd $(HOME)\timing
	xlintimz.exe < zband.in > $@ 2>&1

ztime2.out: ztime2.in xlintimz.exe
	@echo Timing rectangular COMPLEX16 LAPACK linear equation routines
	cd $(HOME)\timing
	xlintimz.exe < ztime2.in > $@ 2>&1

#
#
# ======== SINGLE EIG TIMINGS ===========================
#
sgeptim.out: sgeptim.in xeigtims.exe
	@echo GEP: Timing REAL Generalized Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtims.exe < sgeptim.in > $@ 2>&1

sneptim.out: sneptim.in xeigtims.exe
	@echo NEP: Timing REAL Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtims.exe < sneptim.in > $@ 2>&1

sseptim.out: sseptim.in xeigtims.exe
	@echo SEP: Timing REAL Symmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtims.exe < sseptim.in > $@ 2>&1

ssvdtim.out: ssvdtim.in xeigtims.exe
	@echo SVD: Timing REAL Singular Value Decomposition routines
	cd $(HOME)\timing
	xeigtims.exe < ssvdtim.in > $@ 2>&1

#
# ======== COMPLEX EIG TIMINGS ===========================
#
cgeptim.out: cgeptim.in xeigtimc.exe
	@echo GEP: Timing COMPLEX Generalized Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtimc.exe < cgeptim.in > $@ 2>&1

cneptim.out: cneptim.in xeigtimc.exe
	@echo NEP: Timing COMPLEX Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtimc.exe < cneptim.in > $@ 2>&1

cseptim.out: cseptim.in xeigtimc.exe
	@echo SEP: Timing COMPLEX Symmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtimc.exe < cseptim.in > $@ 2>&1

csvdtim.out: csvdtim.in xeigtimc.exe
	@echo SVD: Timing COMPLEX Singular Value Decomposition routines
	cd $(HOME)\timing
	xeigtimc.exe < csvdtim.in > $@ 2>&1

#
# ======== DOUBLE EIG TIMINGS ===========================
#
dgeptim.out: dgeptim.in xeigtimd.exe
	@echo GEP: Timing DOUBLE PRECISION Generalized Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtimd.exe < dgeptim.in > $@ 2>&1

dneptim.out: dneptim.in xeigtimd.exe
	@echo NEP: Timing DOUBLE PRECISION Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtimd.exe < dneptim.in > $@ 2>&1

dseptim.out: dseptim.in xeigtimd.exe
	@echo SEP: Timing DOUBLE PRECISION Symmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtimd.exe < dseptim.in > $@ 2>&1

dsvdtim.out: dsvdtim.in xeigtimd.exe
	@echo SVD: Timing DOUBLE PRECISION Singular Value Decomposition routines
	cd $(HOME)\timing
	xeigtimd.exe < dsvdtim.in > $@ 2>&1

#
# ======== COMPLEX16 EIG TIMINGS ===========================
#
zgeptim.out: zgeptim.in xeigtimz.exe
	@echo GEP: Timing COMPLEX16 Generalized Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtimz.exe < zgeptim.in > $@ 2>&1

zneptim.out: zneptim.in xeigtimz.exe
	@echo NEP: Timing COMPLEX16 Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtimz.exe < zneptim.in > $@ 2>&1

zseptim.out: zseptim.in xeigtimz.exe
	@echo SEP: Timing COMPLEX16 Symmetric Eigenvalue Problem routines
	cd $(HOME)\timing
	xeigtimz.exe < zseptim.in > $@ 2>&1

zsvdtim.out: zsvdtim.in xeigtimz.exe
	@echo SVD: Timing COMPLEX16 Singular Value Decomposition routines
	cd $(HOME)\timing
	xeigtimz.exe < zsvdtim.in > $@ 2>&1

# ==============================================================================

xlintims.exe:
	cd $(HOME)\timing\lin\linsrc
	$(MAKE) single
	cd $(HOME)\timing\lin
	$(MAKE) single

xlintimc.exe:
	cd $(HOME)\timing\lin\linsrc
	$(MAKE) complex
	cd $(HOME)\timing\lin
	$(MAKE) complex

xlintimd.exe:
	cd $(HOME)\timing\lin\linsrc
	$(MAKE) double
	cd $(HOME)\timing\lin
	$(MAKE) double

xlintimz.exe:
	cd $(HOME)\timing\lin\linsrc
	$(MAKE) complex16
	cd $(HOME)\timing\lin
	$(MAKE) complex16

xeigtims.exe:
	cd $(HOME)\timing\eig\eigsrc
	$(MAKE) single
	cd $(HOME)\timing\eig
	$(MAKE) single

xeigtimc.exe:
	cd $(HOME)\timing\eig\eigsrc
	$(MAKE) complex
	cd $(HOME)\timing\eig
	$(MAKE) complex

xeigtimd.exe:
	cd $(HOME)\timing\eig\eigsrc
	$(MAKE) double
	cd $(HOME)\timing\eig
	$(MAKE) double

xeigtimz.exe:
	cd $(HOME)\timing\eig\eigsrc
	$(MAKE) complex16
	cd $(HOME)\timing\eig
	$(MAKE) complex16

clean:
	del *.out core

cleanup:
	del x* *.out core
