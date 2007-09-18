!include <..\..\make.inc>

#######################################################################
#
#  Windows NT Digital Fortran makefile
#
#  This makefile creates the timing programs for the eigensystem
#  routines in LAPACK.  The timing files are grouped as follows:
#
#       AEIGTIM -- General auxiliary timing routines
#       SCIGTIM -- Single precision auxiliary timing routines
#       DZIGTIM -- Double precision auxiliary timing routines
#
#       SEIGTIM -- Single precision real timing routines
#       CEIGTIM -- Single precision complex timing routines
#       DEIGTIM -- Double precision real timing routines
#       ZEIGTIM -- Double precision complex timing routines
#
#  Timing programs can be generated for all or some of the four
#  data types.  To enter make followed by one or more of the data
#  types desired.  Some examples:
#       make single
#       make single complex
#       make single double complex complex16
#  Alternatively, the command
#       make
#  without any arguments creates all four timing programs.
#  The executable files are called
#       xeigtims.exe, xeigtimd.exe, xeigtimc.exe, and xeigtimz.exe
#  and are created in the next higher directory level.
#
#  To remove the object files after the executable files have been
#  created, enter
#       make clean
#
#######################################################################

.SUFFIXES: .f .obj .exe .lib

all:  single double complex complex16

AEIGTIM = atimin.obj ilaenv.obj xlaenv.obj

SCIGTIM = \
   smflop.obj sopla.obj sopla2.obj sopbl3.obj \
   sprtbe.obj sprtbg.obj sprtbr.obj sprtbs.obj sprtbv.obj

DZIGTIM = \
   dmflop.obj dopla.obj dopla2.obj dopbl3.obj \
   dprtbe.obj dprtbg.obj dprtbr.obj dprtbs.obj dprtbv.obj

SEIGTIM = stimee.obj \
   slaqzh.obj slatm4.obj stim21.obj stim22.obj stim26.obj stim51.obj seispack.obj

CEIGTIM = ctimee.obj \
   claqzh.obj clatm4.obj ctim21.obj ctim22.obj ctim26.obj ctim51.obj ceispack.obj

DEIGTIM = dtimee.obj \
   dlaqzh.obj dlatm4.obj dtim21.obj dtim22.obj dtim26.obj dtim51.obj deispack.obj

ZEIGTIM = ztimee.obj \
   zlaqzh.obj zlatm4.obj ztim21.obj ztim22.obj ztim26.obj ztim51.obj zeispack.obj

single: ..\xeigtims.exe
double: ..\xeigtimd.exe
complex: ..\xeigtimc.exe
complex16: ..\xeigtimz.exe

..\xeigtims.exe:$(AEIGTIM) $(SCIGTIM) $(SEIGTIM)
	$(LOADER) $(LOADOPTS) -exe:$@ $(AEIGTIM) $(SCIGTIM) $(SEIGTIM) \
        ..\..\$(TMGLIB) $(EIGSRCLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

..\xeigtimd.exe:$(AEIGTIM) $(DZIGTIM) $(DEIGTIM)
	$(LOADER) $(LOADOPTS) -exe:$@ $(AEIGTIM) $(DZIGTIM) $(DEIGTIM) \
        ..\..\$(TMGLIB) $(EIGSRCLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

..\xeigtimc.exe:$(AEIGTIM) $(SCIGTIM) $(CEIGTIM)
	$(LOADER) $(LOADOPTS) -exe:$@ $(AEIGTIM) $(SCIGTIM) $(CEIGTIM) \
        ..\..\$(TMGLIB) $(EIGSRCLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

..\xeigtimz.exe:$(AEIGTIM) $(DZIGTIM) $(ZEIGTIM)
	$(LOADER) $(LOADOPTS) -exe:$@ $(AEIGTIM) $(DZIGTIM) $(ZEIGTIM) \
        ..\..\$(TMGLIB) $(EIGSRCLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

clean:
	del *.obj

stimee.obj: stimee.f
	$(FORTRAN) $(DRVOPTS) -c stimee.f
dtimee.obj: dtimee.f
	$(FORTRAN) $(DRVOPTS) -c dtimee.f
ctimee.obj: ctimee.f
	$(FORTRAN) $(DRVOPTS) -c ctimee.f
ztimee.obj: ztimee.f
	$(FORTRAN) $(DRVOPTS) -c ztimee.f
 
.f.obj:
	$(FORTRAN) $(OPTS) -c $<
