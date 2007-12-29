!include <../../make.inc>

#######################################################################
#  This makefile creates the timing programs for the linear equation
#  routines in LAPACK.  The timing files are grouped as follows:
#       ALINTIM -- Timing routines for all data types
#       SCINTIM -- Single precision real and complex timing routines
#       DZINTIM -- Double precision real and complex timing routines
#
#       SLINTIM -- Single precision real timing routines
#       CLINTIM -- Single precision complex timing routines
#       DLINTIM -- Double precision real timing routines
#       ZLINTIM -- Double precision complex timing routines
#
#  Timing programs can be generated for all or some of the four
#  data types.  Enter make followed by one or more of the data types
#  desired.  Some examples:
#       make single
#       make single complex
#       make single double complex complex16
#  Alternatively, the command
#       make
#  without any arguments creates all four timing programs.
#  The executable files are called
#       xlintims, xlintimd, xlintimc, and xlintimz
#  and are created in the next higher directory level.
#
#  To remove the object files after the executable files have been
#  created, enter
#       make clean
#
#######################################################################

.SUFFIXES: .f .obj .exe .lib

all:  single double complex complex16
 
ALINTIM = atimin.obj atimck.obj icopy.obj ilaenv.obj xlaenv.obj
 
SCINTIM = slaord.obj sprtbl.obj sprtb2.obj sprtb3.obj sprtb4.obj sprtb5.obj \
        sprtls.obj smflop.obj sopaux.obj sopbl2.obj sopbl3.obj sopgb.obj sopla.obj
 
DZINTIM = dlaord.obj dprtbl.obj dprtb2.obj dprtb3.obj dprtb4.obj dprtb5.obj \
        dprtls.obj dmflop.obj dopaux.obj dopbl2.obj dopbl3.obj dopgb.obj dopla.obj
 
SLINTIM = stimaa.obj stimmg.obj \
        stimmv.obj stimmm.obj stimb2.obj stimb3.obj stimge.obj stimgb.obj stimgt.obj \
        stimls.obj stimpo.obj stimpp.obj stimpb.obj stimpt.obj stimsy.obj stimsp.obj \
        stimtr.obj stimtp.obj stimtb.obj stimhr.obj stimtd.obj stimbr.obj \
        stimq3.obj stimqr.obj stimlq.obj stimql.obj stimrq.obj stimqp.obj \
        sqrt13.obj sqrt15.obj slinpk.obj seispk.obj
 
CLINTIM = ctimaa.obj ctimmg.obj \
        ctimmv.obj ctimmm.obj ctimb2.obj ctimb3.obj ctimge.obj ctimgb.obj ctimgt.obj \
        ctimls.obj ctimpo.obj ctimpp.obj ctimpb.obj ctimpt.obj \
        ctimsy.obj ctimsp.obj ctimhe.obj ctimhp.obj \
        ctimtr.obj ctimtp.obj ctimtb.obj ctimhr.obj ctimtd.obj ctimbr.obj \
        ctimq3.obj ctimqr.obj ctimlq.obj ctimql.obj ctimrq.obj ctimqp.obj \
        cqrt13.obj cqrt15.obj clinpk.obj
 
DLINTIM = dtimaa.obj dtimmg.obj \
        dtimmv.obj dtimmm.obj dtimb2.obj dtimb3.obj dtimge.obj dtimgb.obj dtimgt.obj \
        dtimls.obj dtimpo.obj dtimpp.obj dtimpb.obj dtimpt.obj dtimsy.obj dtimsp.obj \
        dtimtr.obj dtimtp.obj dtimtb.obj dtimhr.obj dtimtd.obj dtimbr.obj \
        dtimq3.obj dtimqr.obj dtimlq.obj dtimql.obj dtimrq.obj dtimqp.obj \
        dqrt13.obj dqrt15.obj dlinpk.obj deispk.obj
 
ZLINTIM = ztimaa.obj ztimmg.obj \
        ztimmv.obj ztimmm.obj ztimb2.obj ztimb3.obj ztimge.obj ztimgb.obj ztimgt.obj \
        ztimls.obj ztimpo.obj ztimpp.obj ztimpb.obj ztimpt.obj \
        ztimsy.obj ztimsp.obj ztimhe.obj ztimhp.obj \
        ztimtr.obj ztimtp.obj ztimtb.obj ztimhr.obj ztimtd.obj ztimbr.obj \
        ztimq3.obj ztimqr.obj ztimlq.obj ztimql.obj ztimrq.obj ztimqp.obj \
        zqrt13.obj zqrt15.obj zlinpk.obj
 
single: ..\xlintims.exe
double: ..\xlintimd.exe
complex: ..\xlintimc.exe
complex16: ..\xlintimz.exe

..\xlintims.exe: $(ALINTIM) $(SCINTIM) $(SLINTIM)
	$(LOADER) $(LOADOPTS) -exe:$@ $(ALINTIM) $(SCINTIM) $(SLINTIM) \
        ..\..\$(TMGLIB) $(LINSRCLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

..\xlintimd.exe: $(ALINTIM) $(DZINTIM) $(DLINTIM)
	$(LOADER) $(LOADOPTS) -exe:$@ $(ALINTIM) $(DZINTIM) $(DLINTIM) \
        ..\..\$(TMGLIB) $(LINSRCLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

..\xlintimc.exe: $(ALINTIM) $(SCINTIM) $(CLINTIM)
	$(LOADER) $(LOADOPTS) -exe:$@ $(ALINTIM) $(SCINTIM) $(CLINTIM) \
        ..\..\$(TMGLIB) $(LINSRCLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

..\xlintimz.exe: $(ALINTIM) $(DZINTIM) $(ZLINTIM)
	$(LOADER) $(LOADOPTS) -exe:$@ $(ALINTIM) $(DZINTIM) $(ZLINTIM) \
        ..\..\$(TMGLIB) $(LINSRCLIB) ..\..\$(LAPACKLIB) $(BLASLIB)
 
clean:
	del *.obj

stimaa.o: stimaa.f
	$(FORTRAN) $(DRVOPTS) -c stimaa.f
dtimaa.o: dtimaa.f
	$(FORTRAN) $(DRVOPTS) -c dtimaa.f
ctimaa.o: ctimaa.f
	$(FORTRAN) $(DRVOPTS) -c ctimaa.f
ztimaa.o: ztimaa.f
	$(FORTRAN) $(DRVOPTS) -c ztimaa.f
 
.f.obj:
	$(FORTRAN) $(OPTS) -c $*.f
