####################################################################
#                                                                  #
#  Sample Windows NT makefile (Digital Fortran)                    #
#                                                                  #
#  LAPACK make include file.                                       #
#  LAPACK, Version 3.0                                             #
#  June 30, 1999                                                   #
####################################################################
#
HOME = d:\digital\lapack
#
#  The machine (platform) identifier to append to the library names
#
PLAT = _win32
#  
#  Modify the FORTRAN and OPTS definitions to refer to the
#  compiler and desired compiler options for your machine.  NOOPT
#  refers to the compiler options desired when NO OPTIMIZATION is
#  selected.  Define LOADER and LOADOPTS to refer to the loader and 
#  desired load options for your machine.
#
FORTRAN  = df
OPTS     = -optimize:2
DRVOPTS  = $(OPTS)
NOOPT    = -optimize:0
LOADER   = $(FORTRAN)
LOADOPTS =
#
#  The archiver and the flag(s) to use when building archive (library)
#  If you system has no ranlib, set RANLIB = echo.
#
ARCH     = lib
ARCHFLAGS= -out:
RANLIB   = echo
#
#  The location of the libraries to which you will link.  (The 
#  machine-specific, optimized BLAS library should be used whenever
#  possible.)
#
BLASLIB      = ..\..\blas$(PLAT).lib
#BLASLIB      = c:\plsuite\lib\mkl_w.lib
#BLASLIB     = -lblas
LAPACKLIB    = lapack$(PLAT).lib
TMGLIB       = tmglib$(PLAT).lib
EIGSRCLIB    = eigsrc$(PLAT).lib
LINSRCLIB    = linsrc$(PLAT).lib
