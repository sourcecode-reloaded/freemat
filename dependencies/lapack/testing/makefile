#######################################################################
#
#  Sample Windows NT makefile (Watcom Fortran 77)
#
#  This makefile runs the test programs for the linear equation routines
#  and the eigenvalue routines in LAPACK.  The test output files
#  are grouped as follows:
#
#       SLINTST,SEIGTST  -- Single precision real test routines
#       CLINTST,CEIGTST  -- Single precision complex test routines
#       DLINTST,DEIGTST  -- Double precision real test routines
#       ZLINTST,ZEIGTST  -- Double precision complex test routines
#
#  Test programs can be executed for all or some of the four different
#  precisions.  First, modify the RUN definition to refer to the
#  execute and desired execute options for your machine.
#  Then enter 'make' followed by one or more of the data types desired.
#  Some examples:
#       make single
#       make single complex
#       make single double complex complex16
#  Alternatively, the command
#       make
#  without any arguments runs all eight test programs.
#  The executable files are called:
#       xlintsts.exe, xlintstd.exe, xlintstc.exe, and xlintstz.exe for LIN
#       xeigtsts.exe, xeigtstd.exe, xeigtstc.exe, and xeigtstz.exe for EIG
#  and exist in the current directory level.
#
#  To remove the output files after the tests have been run, enter
#       make clean
#
#  To re-run specific tests after a make, enter (for example):
#       'rm ssvd.out; make'  or:
#       'make ssvd.out' or:
#       'touch svd.in; make' (to re-run the single precision SVD tests.)
#
#       'rm *svd.out; make'  (to re-run all the SVD tests.)
#
#----------------------------------------------------------------------
#
#  Susan Blackford, August, 1999
#
#######################################################################

!include <..\make.inc>

.SUFFIXES: .f .obj .exe. .lib

all:     single complex double complex16

SEIGTST= snep.out \
         ssep.out \
         ssvd.out \
         sec.out \
         sed.out \
         sgg.out \
         sgd.out \
         ssb.out \
	 ssg.out \
         sbal.out \
         sbak.out \
         sgbal.out \
         sgbak.out \
         sbb.out \
         sglm.out \
         sgqr.out \
         sgsv.out \
         slse.out

CEIGTST= cnep.out \
         csep.out \
         csvd.out \
         cec.out \
         ced.out \
         cgg.out \
         cgd.out \
         csb.out \
	 csg.out \
         cbal.out \
         cbak.out \
         cgbal.out \
         cgbak.out \
         cbb.out \
         cglm.out \
         cgqr.out \
         cgsv.out \
         clse.out

DEIGTST= dnep.out \
         dsep.out \
         dsvd.out \
         dec.out \
         ded.out \
         dgg.out \
         dgd.out \
         dsb.out \
	 dsg.out \
         dbal.out \
         dbak.out \
         dgbal.out \
         dgbak.out \
         dbb.out \
         dglm.out \
         dgqr.out \
         dgsv.out \
         dlse.out

ZEIGTST= znep.out \
         zsep.out \
         zsvd.out \
         zec.out \
         zed.out \
         zgg.out \
         zgd.out \
         zsb.out \
	 zsg.out \
         zbal.out \
         zbak.out \
         zgbal.out \
         zgbak.out \
         zbb.out \
         zglm.out \
         zgqr.out \
         zgsv.out \
         zlse.out


SLINTST= stest.out

CLINTST= ctest.out

DLINTST= dtest.out

ZLINTST= ztest.out

single:         $(SLINTST) $(SEIGTST)
complex:        $(CLINTST) $(CEIGTST)
double:         $(DLINTST) $(DEIGTST)
complex16:      $(ZLINTST) $(ZEIGTST)

#
# ======== SINGLE LIN TESTS ===========================

stest.out: stest.in xlintsts.exe
	@echo Data file for testing REAL LAPACK linear equation routines
	cd $(HOME)\testing
	xlintsts.exe < stest.in > $@ 2>&1
#
# ======== COMPLEX LIN TESTS ==========================

ctest.out: ctest.in xlintstc.exe
	@echo Data file for testing COMPLEX LAPACK linear equation routines
	cd $(HOME)\testing
	xlintstc.exe < ctest.in > $@ 2>&1
#
# ======== DOUBLE LIN TESTS ===========================

dtest.out: dtest.in xlintstd.exe
	@echo Data file for testing DOUBLE PRECISION LAPACK linear equation routines
	cd $(HOME)\testing
	xlintstd.exe < dtest.in > $@ 2>&1
#
# ======== COMPLEX16 LIN TESTS ========================

ztest.out: ztest.in xlintstz.exe
	@echo Data file for testing COMPLEX16 LAPACK linear equation routines
	cd $(HOME)\testing
	xlintstz.exe < ztest.in > $@ 2>&1
#
#
# ======== SINGLE EIG TESTS ===========================
#

snep.out: nep.in xeigtsts.exe
	@echo NEP: Testing Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtsts.exe < nep.in > $@ 2>&1

ssep.out: sep.in xeigtsts.exe
	@echo SEP: Testing Symmetric Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtsts.exe < sep.in > $@ 2>&1

ssvd.out: svd.in xeigtsts.exe
	@echo SVD: Testing Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtsts.exe < svd.in > $@ 2>&1

sec.out: sec.in xeigtsts.exe
	@echo SEC: Testing REAL Eigen Condition Routines
	cd $(HOME)\testing
	xeigtsts.exe < sec.in > $@ 2>&1

sed.out: sed.in xeigtsts.exe
	@echo SEV: Testing REAL Nonsymmetric Eigenvalue Driver
	cd $(HOME)\testing
	xeigtsts.exe < sed.in > $@ 2>&1

sgg.out: sgg.in xeigtsts.exe
	@echo SGG: Testing REAL Nonsymmetric Generalized Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtsts.exe < sgg.in > $@ 2>&1

sgd.out: sgd.in xeigtsts.exe
	@echo SGD: Testing REAL Nonsymmetric Generalized Eigenvalue Problem driver routines
	cd $(HOME)\testing
	xeigtsts.exe < sgd.in > $@ 2>&1

ssb.out: ssb.in xeigtsts.exe
	@echo SSB: Testing REAL Symmetric Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtsts.exe < ssb.in > $@ 2>&1

ssg.out: ssg.in xeigtsts.exe
	@echo SSG: Testing REAL Symmetric Generalized Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtsts.exe < ssg.in > $@ 2>&1

sbal.out: sbal.in xeigtsts.exe
	@echo SGEBAL: Testing the balancing of a REAL general matrix
	cd $(HOME)\testing
	xeigtsts.exe < sbal.in > $@ 2>&1

sbak.out: sbak.in xeigtsts.exe
	@echo SGEBAK: Testing the back transformation of a REAL balanced matrix
	cd $(HOME)\testing
	xeigtsts.exe < sbak.in > $@ 2>&1

sgbal.out: sgbal.in xeigtsts.exe
	@echo SGGBAL: Testing the balancing of a pair of REAL general matrices
	cd $(HOME)\testing
	xeigtsts.exe < sgbal.in > $@ 2>&1

sgbak.out: sgbak.in xeigtsts.exe
	@echo SGGBAK: Testing the back transformation of a pair of REAL balanced matrices
	cd $(HOME)\testing
	xeigtsts.exe < sgbak.in > $@ 2>&1

sbb.out: sbb.in xeigtsts.exe
	@echo SBB:  Testing banded Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtsts.exe < sbb.in > $@ 2>&1

sglm.out: glm.in xeigtsts.exe
	@echo GLM: Testing Generalized Linear Regression Model routines
	cd $(HOME)\testing
	xeigtsts.exe < glm.in > $@ 2>&1

sgqr.out: gqr.in xeigtsts.exe
	@echo GQR: Testing Generalized QR and RQ factorization routines
	cd $(HOME)\testing
	xeigtsts.exe < gqr.in > $@ 2>&1

sgsv.out: gsv.in xeigtsts.exe
	@echo GSV: Testing Generalized Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtsts.exe < gsv.in > $@ 2>&1

slse.out: lse.in xeigtsts.exe
	@echo LSE: Testing Constrained Linear Least Squares routines
	cd $(HOME)\testing
	xeigtsts.exe < lse.in > $@ 2>&1
#
# ======== COMPLEX EIG TESTS ===========================

cnep.out: nep.in xeigtstc.exe
	@echo NEP: Testing Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstc.exe < nep.in > $@ 2>&1

csep.out: sep.in xeigtstc.exe
	@echo SEP: Testing Symmetric Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstc.exe < sep.in > $@ 2>&1

csvd.out: svd.in xeigtstc.exe
	@echo SVD: Testing Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtstc.exe < svd.in > $@ 2>&1

cec.out: cec.in xeigtstc.exe
	@echo CEC: Testing COMPLEX Eigen Condition Routines
	cd $(HOME)\testing
	xeigtstc.exe < cec.in > $@ 2>&1

ced.out: ced.in xeigtstc.exe
	@echo CES: Testing COMPLEX Nonsymmetric Schur Form Driver
	cd $(HOME)\testing
	xeigtstc.exe < ced.in > $@ 2>&1

cgg.out: cgg.in xeigtstc.exe
	@echo CGG: Testing COMPLEX Nonsymmetric Generalized Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstc.exe < cgg.in > $@ 2>&1

cgd.out: cgd.in xeigtstc.exe
	@echo CGD: Testing COMPLEX Nonsymmetric Generalized Eigenvalue Problem driver routines
	cd $(HOME)\testing
	xeigtstc.exe < cgd.in > $@ 2>&1

csb.out: csb.in xeigtstc.exe
	@echo CHB: Testing Hermitian Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstc.exe < csb.in > $@ 2>&1

csg.out: csg.in xeigtstc.exe
	@echo CSG: Testing Symmetric Generalized Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstc.exe < csg.in > $@ 2>&1

cbal.out: cbal.in xeigtstc.exe
	@echo CGEBAL: Testing the balancing of a COMPLEX general matrix
	cd $(HOME)\testing
	xeigtstc.exe < cbal.in > $@ 2>&1

cbak.out: cbak.in xeigtstc.exe
	@echo CGEBAK: Testing the back transformation of a COMPLEX balanced matrix
	cd $(HOME)\testing
	xeigtstc.exe < cbak.in > $@ 2>&1

cgbal.out: cgbal.in xeigtstc.exe
	@echo CGGBAL: Testing the balancing of a pair of COMPLEX general matrices
	cd $(HOME)\testing
	xeigtstc.exe < cgbal.in > $@ 2>&1

cgbak.out: cgbak.in xeigtstc.exe
	@echo CGGBAK: Testing the back transformation of a pair of COMPLEX balanced matrices
	cd $(HOME)\testing
	xeigtstc.exe < cgbak.in > $@ 2>&1

cbb.out: cbb.in xeigtstc.exe
	@echo CBB:  Testing banded Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtstc.exe < cbb.in > $@ 2>&1

cglm.out: glm.in xeigtstc.exe
	@echo GLM: Testing Generalized Linear Regression Model routines
	cd $(HOME)\testing
	xeigtstc.exe < glm.in > $@ 2>&1

cgqr.out: gqr.in xeigtstc.exe
	@echo GQR: Testing Generalized QR and RQ factorization routines
	cd $(HOME)\testing
	xeigtstc.exe < gqr.in > $@ 2>&1

cgsv.out: gsv.in xeigtstc.exe
	@echo GSV: Testing Generalized Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtstc.exe < gsv.in > $@ 2>&1

clse.out: lse.in xeigtstc.exe
	@echo LSE: Testing Constrained Linear Least Squares routines
	cd $(HOME)\testing
	xeigtstc.exe < lse.in > $@ 2>&1
#
# ======== DOUBLE EIG TESTS ===========================

dnep.out: nep.in xeigtstd.exe
	@echo NEP: Testing Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstd.exe < nep.in > $@ 2>&1

dsep.out: sep.in xeigtstd.exe
	@echo SEP: Testing Symmetric Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstd.exe < sep.in > $@ 2>&1

dsvd.out: svd.in xeigtstd.exe
	@echo SVD: Testing Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtstd.exe < svd.in > $@ 2>&1

dec.out: dec.in xeigtstd.exe
	@echo DEC: Testing DOUBLE PRECISION Eigen Condition Routines
	cd $(HOME)\testing
	xeigtstd.exe < dec.in > $@ 2>&1

ded.out: ded.in xeigtstd.exe
	@echo DEV: Testing DOUBLE PRECISION Nonsymmetric Eigenvalue Driver routines
	cd $(HOME)\testing
	xeigtstd.exe < ded.in > $@ 2>&1

dgg.out: dgg.in xeigtstd.exe
	@echo DGG: Testing DOUBLE PRECISION Nonsymmetric Generalized Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstd.exe < dgg.in > $@ 2>&1

dgd.out: dgd.in xeigtstd.exe
	@echo DGD: Testing DOUBLE PRECISION Nonsymmetric Generalized Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstd.exe < dgd.in > $@ 2>&1

dsb.out: dsb.in xeigtstd.exe
	@echo DSB: Testing DOUBLE PRECISION Symmetric Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstd.exe < dsb.in > $@ 2>&1

dsg.out: dsg.in xeigtstd.exe
	@echo DSG: Testing DOUBLE PRECISION Symmetric Generalized Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstd.exe < dsg.in > $@ 2>&1

dbal.out: dbal.in xeigtstd.exe
	@echo DGEBAL: Testing the balancing of a DOUBLE PRECISION general matrix
	cd $(HOME)\testing
	xeigtstd.exe < dbal.in > $@ 2>&1

dbak.out: dbak.in xeigtstd.exe
	@echo DGEBAK:  Testing the back transformation of a DOUBLE PRECISION balanced matrix
	cd $(HOME)\testing
	xeigtstd.exe < dbak.in > $@ 2>&1

dgbal.out: dgbal.in xeigtstd.exe
	@echo DGGBAL: Testing the balancing of a pair of DOUBLE PRECISION general matrices
	cd $(HOME)\testing
	xeigtstd.exe < dgbal.in > $@ 2>&1

dgbak.out: dgbak.in xeigtstd.exe
	@echo DGGBAK: Testing the back transformation of a pair of DOUBLE PRECISION balanced matrices
	cd $(HOME)\testing
	xeigtstd.exe < dgbak.in > $@ 2>&1

dbb.out: dbb.in xeigtstd.exe
	@echo DBB:  Testing banded Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtstd.exe < dbb.in > $@ 2>&1

dglm.out: glm.in xeigtstd.exe
	@echo GLM: Testing Generalized Linear Regression Model routines
	cd $(HOME)\testing
	xeigtstd.exe < glm.in > $@ 2>&1

dgqr.out: gqr.in xeigtstd.exe
	@echo GQR: Testing Generalized QR and RQ factorization routines
	cd $(HOME)\testing
	xeigtstd.exe < gqr.in > $@ 2>&1

dgsv.out: gsv.in xeigtstd.exe
	@echo GSV: Testing Generalized Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtstd.exe < gsv.in > $@ 2>&1

dlse.out: lse.in xeigtstd.exe
	@echo LSE: Testing Constrained Linear Least Squares routines
	cd $(HOME)\testing
	xeigtstd.exe < lse.in > $@ 2>&1
#
# ======== COMPLEX16 EIG TESTS ===========================

znep.out: nep.in xeigtstz.exe
	@echo NEP: Testing Nonsymmetric Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstz.exe < nep.in > $@ 2>&1

zsep.out: sep.in xeigtstz.exe
	@echo SEP: Testing Symmetric Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstz.exe < sep.in > $@ 2>&1

zsvd.out: svd.in xeigtstz.exe
	@echo SVD: Testing Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtstz.exe < svd.in > $@ 2>&1

zec.out: zec.in xeigtstz.exe
	@echo ZEC: Testing COMPLEX16 Eigen Condition Routines
	cd $(HOME)\testing
	xeigtstz.exe < zec.in > $@ 2>&1

zed.out: zed.in xeigtstz.exe
	@echo ZES: Testing COMPLEX16 Nonsymmetric Schur Form Driver
	cd $(HOME)\testing
	xeigtstz.exe < zed.in > $@ 2>&1

zgg.out: zgg.in xeigtstz.exe
	@echo ZGG: Testing COMPLEX16 Nonsymmetric Generalized Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstz.exe < zgg.in > $@ 2>&1

zgd.out: zgd.in xeigtstz.exe
	@echo ZGD: Testing COMPLEX16 Nonsymmetric Generalized Eigenvalue Problem driver routines
	cd $(HOME)\testing
	xeigtstz.exe < zgd.in > $@ 2>&1

zsb.out: zsb.in xeigtstz.exe
	@echo ZHB: Testing Hermitian Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstz.exe < zsb.in > $@ 2>&1

zsg.out: zsg.in xeigtstz.exe
	@echo ZSG: Testing Symmetric Generalized Eigenvalue Problem routines
	cd $(HOME)\testing
	xeigtstz.exe < zsg.in > $@ 2>&1

zbal.out: zbal.in xeigtstz.exe
	@echo ZGEBAL: Testing the balancing of a COMPLEX16 general matrix
	cd $(HOME)\testing
	xeigtstz.exe < zbal.in > $@ 2>&1

zbak.out: zbak.in xeigtstz.exe
	@echo ZGEBAK: Testing the back transformation of a COMPLEX16 balanced matrix
	cd $(HOME)\testing
	xeigtstz.exe < zbak.in > $@ 2>&1

zgbal.out: zgbal.in xeigtstz.exe
	@echo ZGGBAL: Testing the balancing of a pair of COMPLEX general matrices
	cd $(HOME)\testing
	xeigtstz.exe < zgbal.in > $@ 2>&1

zgbak.out: zgbak.in xeigtstz.exe
	@echo ZGGBAK: Testing the back transformation of a pair of COMPLEX16 balanced matrices
	cd $(HOME)\testing
	xeigtstz.exe < zgbak.in > $@ 2>&1

zbb.out: zbb.in xeigtstz.exe
	@echo ZBB:  Testing banded Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtstz.exe < zbb.in > $@ 2>&1

zglm.out: glm.in xeigtstz.exe
	@echo GLM: Testing Generalized Linear Regression Model routines
	cd $(HOME)\testing
	xeigtstz.exe < glm.in > $@ 2>&1

zgqr.out: gqr.in xeigtstz.exe
	@echo GQR: Testing Generalized QR and RQ factorization routines
	cd $(HOME)\testing
	xeigtstz.exe < gqr.in > $@ 2>&1

zgsv.out: gsv.in xeigtstz.exe
	@echo GSV: Testing Generalized Singular Value Decomposition routines
	cd $(HOME)\testing
	xeigtstz.exe < gsv.in > $@ 2>&1

zlse.out: lse.in xeigtstz.exe
	@echo LSE: Testing Constrained Linear Least Squares routines
	cd $(HOME)\testing
	xeigtstz.exe < lse.in > $@ 2>&1
# ==============================================================================

xlintsts.exe:
	cd $(HOME)\testing\lin
	$(MAKE) single

xlintstc.exe:
	cd $(HOME)\testing\lin
	$(MAKE) complex

xlintstd.exe:
	cd $(HOME)\testing\lin
	$(MAKE) double

xlintstz.exe:
	cd $(HOME)\testing\lin
	$(MAKE) complex16

xeigtsts.exe:
	cd $(HOME)\testing\eig
	$(MAKE) single

xeigtstc.exe:
	cd $(HOME)\testing\eig
	$(MAKE) complex

xeigtstd.exe:
	cd $(HOME)\testing\eig
	$(MAKE) double

xeigtstz.exe:
	cd $(HOME)\testing\eig
	$(MAKE) complex16

clean:
	del *.out core

cleanup:
	del x* *.out core
