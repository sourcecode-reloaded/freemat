#
#  Top Level Makefile for LAPACK
#  Version 3.0
#  June 30, 1999
#

!include <make.inc>

all: install lib

lib: blaslib lapacklib tmglib

clean: cleanlib cleantesting cleantiming

install:
	cd $(HOME)\install
	$(MAKE)
	testlsame.exe
	testslamch.exe
	testdlamch.exe
	testsecond.exe
	testdsecnd.exe
	testieee.exe
	copy lsame.f $(HOME)\blas\src\
	copy lsame.f $(HOME)\src\
	copy slamch.f $(HOME)\src\
	copy dlamch.f $(HOME)\src\
	copy second.f $(HOME)\src\
	copy dsecnd.f $(HOME)\src\

blaslib:
	cd $(HOME)\blas\src
	$(MAKE)

lapacklib:
	cd $(HOME)\src
	$(MAKE)

tmglib:
	cd $(HOME)\testing\matgen
	$(MAKE)

testing:
	cd $(HOME)\testing
	$(MAKE)

blas_testing:
	cd $(HOME)\blas\testing
	$(MAKE) -f makeblat1
	cd $(HOME)\blas
	xblat1s > sblat1.out
	xblat1d > dblat1.out
	xblat1c > cblat1.out
	xblat1z > zblat1.out

	cd $(HOME)\blas\testing
	$(MAKE) -f makeblat2
	cd $(HOME)\blas
	xblat2s < sblat2.in
	xblat2d < dblat2.in
	xblat2c < cblat2.in
	xblat2z < zblat2.in

	cd $(HOME)\blas\testing
	$(MAKE) -f makeblat3
	cd $(HOME)\blas
	xblat3s < sblat3.in
	xblat3d < dblat3.in
	xblat3c < cblat3.in
	xblat3z < zblat3.in

timing:
	cd $(HOME)\timing
	$(MAKE)

blas_timing:
	cd $(HOME)\timing\lin
	$(MAKE)
	cd $(HOME)\timing
	xlintims < sblasa.in > sblasa.out
	xlintims < sblasb.in > sblasb.out
	xlintims < sblasc.in > sblasc.out
	cd $(HOME)\timing
	xlintimd < dblasa.in > dblasa.out
	xlintimd < dblasb.in > dblasb.out
	xlintimd < dblasc.in > dblasc.out
	cd $(HOME)\timing
	xlintimc < cblasa.in > cblasa.out
	xlintimc < cblasb.in > cblasb.out
	xlintimc < cblasc.in > cblasc.out
	cd $(HOME)\timing
	xlintimz < zblasa.in > zblasa.out
	xlintimz < zblasb.in > zblasb.out
	xlintimz < zblasc.in > zblasc.out

cleanlib:
	cd $(HOME)\install
	$(MAKE) clean
	cd $(HOME)\blas\src
	$(MAKE) clean
	cd $(HOME)\src
	$(MAKE) clean
	cd $(HOME)\testing\matgen
	$(MAKE) clean

cleantesting:
	cd $(HOME)\testing\lin
	$(MAKE) clean
	cd $(HOME)\testing\eig
	$(MAKE) clean
	cd $(HOME)\testing
	del xlin*.exe xeig*.exe

cleantiming:
	cd $(HOME)\timing\lin
	$(MAKE) clean
	cd $(HOME)\timing\lin\linsrc
	$(MAKE) clean
	cd $(HOME)\timing\eig
	$(MAKE) clean
	cd $(HOME)\timing\eig\eigsrc
	$(MAKE) clean
	cd $(HOME)\timing
	del xlin*.exe xeig*.exe
