!include <..\..\make.inc>

#######################################################################
#
#  Windows NT Digital Fortran Makefile
#
#  This makefile creates the test programs for the linear equation
#  routines in LAPACK.  The test files are grouped as follows:
#
#       ALINTST -- Auxiliary test routines
#       SLINTST -- Single precision real test routines
#       CLINTST -- Single precision complex test routines
#       SCLNTST -- Single and Complex routines in common
#       DLINTST -- Double precision real test routines
#       ZLINTST -- Double precision complex test routines
#       DZLNTST -- Double and Double Complex routines in common
#
#  Test programs can be generated for all or some of the four different
#  precisions.  Enter make followed by one or more of the data types
#  desired.  Some examples:
#       make single
#       make single complex
#       make single double complex complex16
#  Alternatively, the command
#       make
#  without any arguments creates all four test programs.
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

ALINTST = \
   aladhd.obj alaerh.obj alaesm.obj alahd.obj alareq.obj \
   alasum.obj alasvm.obj chkxer.obj icopy.obj ilaenv.obj xlaenv.obj xerbla.obj

SCLNTST= slaord.obj

DZLNTST= dlaord.obj 

SLINTST = schkaa.obj \
   schkeq.obj schkgb.obj schkge.obj schkgt.obj \
   schklq.obj schkpb.obj schkpo.obj schkpp.obj \
   schkpt.obj schkq3.obj schkql.obj schkqp.obj schkqr.obj schkrq.obj \
   schksp.obj schksy.obj schktb.obj schktp.obj schktr.obj \
   schktz.obj \
   sdrvgb.obj sdrvge.obj sdrvgt.obj sdrvls.obj sdrvpb.obj \
   sdrvpo.obj sdrvpp.obj sdrvpt.obj sdrvsp.obj sdrvsy.obj \
   serrge.obj serrgt.obj serrlq.obj serrls.obj \
   serrpo.obj serrql.obj serrqp.obj serrqr.obj \
   serrrq.obj serrsy.obj serrtr.obj serrtz.obj serrvx.obj \
   sgbt01.obj sgbt02.obj sgbt05.obj sgelqs.obj sgeqls.obj sgeqrs.obj \
   sgerqs.obj sget01.obj sget02.obj \
   sget03.obj sget04.obj sget06.obj sget07.obj sgtt01.obj sgtt02.obj \
   sgtt05.obj slaptm.obj slarhs.obj slatb4.obj slattb.obj slattp.obj \
   slattr.obj slavsp.obj slavsy.obj slqt01.obj slqt02.obj \
   slqt03.obj spbt01.obj spbt02.obj spbt05.obj spot01.obj \
   spot02.obj spot03.obj spot05.obj sppt01.obj \
   sppt02.obj sppt03.obj sppt05.obj sptt01.obj sptt02.obj \
   sptt05.obj sqlt01.obj sqlt02.obj sqlt03.obj sqpt01.obj \
   sqrt01.obj sqrt02.obj sqrt03.obj sqrt11.obj sqrt12.obj \
   sqrt13.obj sqrt14.obj sqrt15.obj sqrt16.obj sqrt17.obj \
   srqt01.obj srqt02.obj srqt03.obj srzt01.obj srzt02.obj \
   sspt01.obj ssyt01.obj \
   stbt02.obj stbt03.obj stbt05.obj stbt06.obj stpt01.obj \
   stpt02.obj stpt03.obj stpt05.obj stpt06.obj strt01.obj \
   strt02.obj strt03.obj strt05.obj strt06.obj \
   stzt01.obj stzt02.obj

CLINTST = cchkaa.obj \
   cchkeq.obj cchkgb.obj cchkge.obj cchkgt.obj \
   cchkhe.obj cchkhp.obj cchklq.obj cchkpb.obj \
   cchkpo.obj cchkpp.obj cchkpt.obj cchkq3.obj cchkql.obj cchkqp.obj \
   cchkqr.obj cchkrq.obj cchksp.obj cchksy.obj cchktb.obj \
   cchktp.obj cchktr.obj cchktz.obj \
   cdrvgb.obj cdrvge.obj cdrvgt.obj cdrvhe.obj cdrvhp.obj \
   cdrvls.obj cdrvpb.obj cdrvpo.obj cdrvpp.obj cdrvpt.obj \
   cdrvsp.obj cdrvsy.obj \
   cerrge.obj cerrgt.obj cerrhe.obj cerrlq.obj \
   cerrls.obj cerrpo.obj cerrql.obj cerrqp.obj \
   cerrqr.obj cerrrq.obj cerrsy.obj cerrtr.obj cerrtz.obj \
   cerrvx.obj \
   cgbt01.obj cgbt02.obj cgbt05.obj cgelqs.obj cgeqls.obj cgeqrs.obj \
   cgerqs.obj cget01.obj cget02.obj \
   cget03.obj cget04.obj cget07.obj cgtt01.obj cgtt02.obj \
   cgtt05.obj chet01.obj chpt01.obj claipd.obj claptm.obj clarhs.obj clatb4.obj \
   clatsp.obj clatsy.obj clattb.obj clattp.obj clattr.obj \
   clavhe.obj clavhp.obj clavsp.obj clavsy.obj clqt01.obj \
   clqt02.obj clqt03.obj cpbt01.obj cpbt02.obj cpbt05.obj \
   cpot01.obj cpot02.obj cpot03.obj cpot05.obj \
   cppt01.obj cppt02.obj cppt03.obj cppt05.obj cptt01.obj \
   cptt02.obj cptt05.obj cqlt01.obj cqlt02.obj cqlt03.obj \
   cqpt01.obj cqrt01.obj cqrt02.obj cqrt03.obj cqrt11.obj \
   cqrt12.obj cqrt13.obj cqrt14.obj cqrt15.obj cqrt16.obj \
   cqrt17.obj crqt01.obj crqt02.obj crqt03.obj crzt01.obj crzt02.obj \
   csbmv.obj  cspt01.obj \
   cspt02.obj cspt03.obj csyt01.obj csyt02.obj csyt03.obj \
   ctbt02.obj ctbt03.obj ctbt05.obj ctbt06.obj ctpt01.obj \
   ctpt02.obj ctpt03.obj ctpt05.obj ctpt06.obj ctrt01.obj \
   ctrt02.obj ctrt03.obj ctrt05.obj ctrt06.obj \
   ctzt01.obj ctzt02.obj sget06.obj

DLINTST = dchkaa.obj \
   dchkeq.obj dchkgb.obj dchkge.obj dchkgt.obj \
   dchklq.obj dchkpb.obj dchkpo.obj dchkpp.obj \
   dchkpt.obj dchkq3.obj dchkql.obj dchkqp.obj dchkqr.obj dchkrq.obj \
   dchksp.obj dchksy.obj dchktb.obj dchktp.obj dchktr.obj \
   dchktz.obj \
   ddrvgb.obj ddrvge.obj ddrvgt.obj ddrvls.obj ddrvpb.obj \
   ddrvpo.obj ddrvpp.obj ddrvpt.obj ddrvsp.obj ddrvsy.obj \
   derrge.obj derrgt.obj derrlq.obj derrls.obj \
   derrpo.obj derrql.obj derrqp.obj derrqr.obj \
   derrrq.obj derrsy.obj derrtr.obj derrtz.obj derrvx.obj \
   dgbt01.obj dgbt02.obj dgbt05.obj dgelqs.obj dgeqls.obj dgeqrs.obj \
   dgerqs.obj dget01.obj dget02.obj \
   dget03.obj dget04.obj dget06.obj dget07.obj dgtt01.obj dgtt02.obj \
   dgtt05.obj dlaptm.obj dlarhs.obj dlatb4.obj dlattb.obj dlattp.obj \
   dlattr.obj dlavsp.obj dlavsy.obj dlqt01.obj dlqt02.obj \
   dlqt03.obj dpbt01.obj dpbt02.obj dpbt05.obj dpot01.obj \
   dpot02.obj dpot03.obj dpot05.obj dppt01.obj \
   dppt02.obj dppt03.obj dppt05.obj dptt01.obj dptt02.obj \
   dptt05.obj dqlt01.obj dqlt02.obj dqlt03.obj dqpt01.obj \
   dqrt01.obj dqrt02.obj dqrt03.obj dqrt11.obj dqrt12.obj \
   dqrt13.obj dqrt14.obj dqrt15.obj dqrt16.obj dqrt17.obj \
   drqt01.obj drqt02.obj drqt03.obj drzt01.obj drzt02.obj \
   dspt01.obj dsyt01.obj \
   dtbt02.obj dtbt03.obj dtbt05.obj dtbt06.obj dtpt01.obj \
   dtpt02.obj dtpt03.obj dtpt05.obj dtpt06.obj dtrt01.obj \
   dtrt02.obj dtrt03.obj dtrt05.obj dtrt06.obj \
   dtzt01.obj dtzt02.obj

ZLINTST = zchkaa.obj \
   zchkeq.obj zchkgb.obj zchkge.obj zchkgt.obj \
   zchkhe.obj zchkhp.obj zchklq.obj zchkpb.obj \
   zchkpo.obj zchkpp.obj zchkpt.obj zchkq3.obj zchkql.obj zchkqp.obj \
   zchkqr.obj zchkrq.obj zchksp.obj zchksy.obj zchktb.obj \
   zchktp.obj zchktr.obj zchktz.obj \
   zdrvgb.obj zdrvge.obj zdrvgt.obj zdrvhe.obj zdrvhp.obj \
   zdrvls.obj zdrvpb.obj zdrvpo.obj zdrvpp.obj zdrvpt.obj \
   zdrvsp.obj zdrvsy.obj \
   zerrge.obj zerrgt.obj zerrhe.obj zerrlq.obj \
   zerrls.obj zerrpo.obj zerrql.obj zerrqp.obj \
   zerrqr.obj zerrrq.obj zerrsy.obj zerrtr.obj zerrtz.obj \
   zerrvx.obj \
   zgbt01.obj zgbt02.obj zgbt05.obj zgelqs.obj zgeqls.obj zgeqrs.obj \
   zgerqs.obj zget01.obj zget02.obj \
   zget03.obj zget04.obj zget07.obj zgtt01.obj zgtt02.obj \
   zgtt05.obj zhet01.obj zhpt01.obj zlaipd.obj zlaptm.obj zlarhs.obj zlatb4.obj \
   zlatsp.obj zlatsy.obj zlattb.obj zlattp.obj zlattr.obj \
   zlavhe.obj zlavhp.obj zlavsp.obj zlavsy.obj zlqt01.obj \
   zlqt02.obj zlqt03.obj zpbt01.obj zpbt02.obj zpbt05.obj \
   zpot01.obj zpot02.obj zpot03.obj zpot05.obj \
   zppt01.obj zppt02.obj zppt03.obj zppt05.obj zptt01.obj \
   zptt02.obj zptt05.obj zqlt01.obj zqlt02.obj zqlt03.obj \
   zqpt01.obj zqrt01.obj zqrt02.obj zqrt03.obj zqrt11.obj \
   zqrt12.obj zqrt13.obj zqrt14.obj zqrt15.obj zqrt16.obj \
   zqrt17.obj zrqt01.obj zrqt02.obj zrqt03.obj zrzt01.obj zrzt02.obj \
   zsbmv.obj  zspt01.obj \
   zspt02.obj zspt03.obj zsyt01.obj zsyt02.obj zsyt03.obj \
   ztbt02.obj ztbt03.obj ztbt05.obj ztbt06.obj ztpt01.obj \
   ztpt02.obj ztpt03.obj ztpt05.obj ztpt06.obj ztrt01.obj \
   ztrt02.obj ztrt03.obj ztrt05.obj ztrt06.obj \
   ztzt01.obj ztzt02.obj dget06.obj

all:  single double complex complex16

single: ..\xlintsts.exe
double: ..\xlintstd.exe
complex: ..\xlintstc.exe
complex16: ..\xlintstz.exe

..\xlintsts.exe : $(ALINTST) $(SLINTST) $(SCLNTST)
	$(LOADER) $(LOADOPTS) -exe:$@  $(ALINTST) $(SCLNTST) $(SLINTST) \
	..\..\$(TMGLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

..\xlintstc.exe : $(ALINTST) $(CLINTST) $(SCLNTST)
	$(LOADER) $(LOADOPTS) -exe:$@ $(ALINTST) $(SCLNTST) $(CLINTST) \
	..\..\$(TMGLIB) ..\..\$(LAPACKLIB) $(BLASLIB)
 
..\xlintstd.exe : $(ALINTST) $(DLINTST) $(DZLNTST)
	$(LOADER) $(LOADOPTS) -exe:$@ $(ALINTST) $(DZLNTST) $(DLINTST) \
	..\..\$(TMGLIB) ..\..\$(LAPACKLIB) $(BLASLIB)
 
..\xlintstz.exe : $(ALINTST) $(ZLINTST) $(DZLNTST)
	$(LOADER) $(LOADOPTS) -exe:$@ $(ALINTST) $(DZLNTST) $(ZLINTST) \
	..\..\$(TMGLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

clean:
	del *.obj

schkaa.obj: schkaa.f
	$(FORTRAN) $(DRVOPTS) -c schkaa.f
dchkaa.obj: dchkaa.f
	$(FORTRAN) $(DRVOPTS) -c dchkaa.f
cchkaa.obj: cchkaa.f
	$(FORTRAN) $(DRVOPTS) -c cchkaa.f
zchkaa.obj: zchkaa.f
	$(FORTRAN) $(DRVOPTS) -c zchkaa.f
 
.f.obj: 
	$(FORTRAN) $(OPTS) -c $<
