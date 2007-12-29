!include <..\make.inc>

#######################################################################
#
#  Sample Windows NT makefile using Digital Fortran
#
#  This is the makefile to create a library for LAPACK.
#  The files are organized as follows:
#       ALLAUX -- Auxiliary routines called from all precisions
#       SCLAUX -- Auxiliary routines called from both REAL and COMPLEX
#       DZLAUX -- Auxiliary routines called from both DOUBLE PRECISION
#                 and COMPLEX*16
#       SLASRC -- Single precision real LAPACK routines
#       CLASRC -- Single precision complex LAPACK routines
#       DLASRC -- Double precision real LAPACK routines
#       ZLASRC -- Double precision complex LAPACK routines
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
#       lapack.a
#  and is created at the next higher directory level.
#
#  To remove the object files after the library is created, enter
#       make clean
#
#  ***Note***
#  The functions lsame, second, dsecnd, slamch, and dlamch may have
#  to be installed before compiling the library.  Refer to the
#  installation guide, LAPACK Working Note 41, for instructions.
#
#######################################################################

.SUFFIXES: .f .obj

all: single complex double complex16
 
ALLAUX = ilaenv.obj ieeeck.obj lsame.obj lsamen.obj xerbla.obj

SCLAUX = \
   sbdsdc.obj \
   sbdsqr.obj sdisna.obj slabad.obj slacpy.obj sladiv.obj slae2.obj  slaebz.obj \
   slaed0.obj slaed1.obj slaed2.obj slaed3.obj slaed4.obj slaed5.obj slaed6.obj \
   slaed7.obj slaed8.obj slaed9.obj slaeda.obj slaev2.obj slagtf.obj \
   slagts.obj slamch.obj slamrg.obj slanst.obj \
   slapy2.obj slapy3.obj slarnv.obj \
   slarrb.obj slarre.obj slarrf.obj \
   slartg.obj slaruv.obj slas2.obj  slascl.obj \
   slasd0.obj slasd1.obj slasd2.obj slasd3.obj slasd4.obj slasd5.obj slasd6.obj \
   slasd7.obj slasd8.obj slasd9.obj slasda.obj slasdq.obj slasdt.obj \
   slaset.obj slasq1.obj slasq2.obj slasq3.obj slasq4.obj slasq5.obj slasq6.obj \
   slasr.obj  slasrt.obj slassq.obj slasv2.obj spttrf.obj sstebz.obj sstedc.obj \
   ssteqr.obj ssterf.obj second.obj

DZLAUX = \
   dbdsdc.obj \
   dbdsqr.obj ddisna.obj dlabad.obj dlacpy.obj dladiv.obj dlae2.obj  dlaebz.obj \
   dlaed0.obj dlaed1.obj dlaed2.obj dlaed3.obj dlaed4.obj dlaed5.obj dlaed6.obj \
   dlaed7.obj dlaed8.obj dlaed9.obj dlaeda.obj dlaev2.obj dlagtf.obj \
   dlagts.obj dlamch.obj dlamrg.obj dlanst.obj \
   dlapy2.obj dlapy3.obj dlarnv.obj \
   dlarrb.obj dlarre.obj dlarrf.obj \
   dlartg.obj dlaruv.obj dlas2.obj  dlascl.obj \
   dlasd0.obj dlasd1.obj dlasd2.obj dlasd3.obj dlasd4.obj dlasd5.obj dlasd6.obj \
   dlasd7.obj dlasd8.obj dlasd9.obj dlasda.obj dlasdq.obj dlasdt.obj \
   dlaset.obj dlasq1.obj dlasq2.obj dlasq3.obj dlasq4.obj dlasq5.obj dlasq6.obj \
   dlasr.obj  dlasrt.obj dlassq.obj dlasv2.obj dpttrf.obj dstebz.obj dstedc.obj \
   dsteqr.obj dsterf.obj dsecnd.obj

SLASRC = \
   sgbbrd.obj sgbcon.obj sgbequ.obj sgbrfs.obj sgbsv.obj  \
   sgbsvx.obj sgbtf2.obj sgbtrf.obj sgbtrs.obj sgebak.obj sgebal.obj sgebd2.obj \
   sgebrd.obj sgecon.obj sgeequ.obj sgees.obj  sgeesx.obj sgeev.obj  sgeevx.obj \
   sgegs.obj  sgegv.obj  sgehd2.obj sgehrd.obj sgelq2.obj sgelqf.obj \
   sgels.obj  sgelsd.obj sgelss.obj sgelsx.obj sgelsy.obj sgeql2.obj sgeqlf.obj \
   sgeqp3.obj sgeqpf.obj sgeqr2.obj sgeqrf.obj sgerfs.obj sgerq2.obj sgerqf.obj \
   sgesc2.obj sgesdd.obj sgesv.obj  sgesvd.obj sgesvx.obj sgetc2.obj sgetf2.obj \
   sgetrf.obj sgetri.obj \
   sgetrs.obj sggbak.obj sggbal.obj sgges.obj  sggesx.obj sggev.obj  sggevx.obj \
   sggglm.obj sgghrd.obj sgglse.obj sggqrf.obj \
   sggrqf.obj sggsvd.obj sggsvp.obj sgtcon.obj sgtrfs.obj sgtsv.obj  \
   sgtsvx.obj sgttrf.obj sgttrs.obj sgtts2.obj shgeqz.obj \
   shsein.obj shseqr.obj slabrd.obj slacon.obj \
   slaein.obj slaexc.obj slag2.obj  slags2.obj slagtm.obj slagv2.obj slahqr.obj \
   slahrd.obj slaic1.obj slaln2.obj slals0.obj slalsa.obj slalsd.obj \
   slangb.obj slange.obj slangt.obj slanhs.obj slansb.obj slansp.obj \
   slansy.obj slantb.obj slantp.obj slantr.obj slanv2.obj \
   slapll.obj slapmt.obj \
   slaqgb.obj slaqge.obj slaqp2.obj slaqps.obj slaqsb.obj slaqsp.obj slaqsy.obj \
   slaqtr.obj slar1v.obj slar2v.obj \
   slarf.obj  slarfb.obj slarfg.obj slarft.obj slarfx.obj slargv.obj \
   slarrv.obj slartv.obj \
   slarz.obj  slarzb.obj slarzt.obj slaswp.obj slasy2.obj slasyf.obj \
   slatbs.obj slatdf.obj slatps.obj slatrd.obj slatrs.obj slatrz.obj slatzm.obj \
   slauu2.obj slauum.obj sopgtr.obj sopmtr.obj sorg2l.obj sorg2r.obj \
   sorgbr.obj sorghr.obj sorgl2.obj sorglq.obj sorgql.obj sorgqr.obj sorgr2.obj \
   sorgrq.obj sorgtr.obj sorm2l.obj sorm2r.obj \
   sormbr.obj sormhr.obj sorml2.obj sormlq.obj sormql.obj sormqr.obj sormr2.obj \
   sormr3.obj sormrq.obj sormrz.obj sormtr.obj spbcon.obj spbequ.obj spbrfs.obj \
   spbstf.obj spbsv.obj  spbsvx.obj \
   spbtf2.obj spbtrf.obj spbtrs.obj spocon.obj spoequ.obj sporfs.obj sposv.obj  \
   sposvx.obj spotf2.obj spotrf.obj spotri.obj spotrs.obj sppcon.obj sppequ.obj \
   spprfs.obj sppsv.obj  sppsvx.obj spptrf.obj spptri.obj spptrs.obj sptcon.obj \
   spteqr.obj sptrfs.obj sptsv.obj  sptsvx.obj spttrs.obj sptts2.obj srscl.obj  \
   ssbev.obj  ssbevd.obj ssbevx.obj ssbgst.obj ssbgv.obj  ssbgvd.obj ssbgvx.obj \
   ssbtrd.obj sspcon.obj sspev.obj  sspevd.obj sspevx.obj sspgst.obj \
   sspgv.obj  sspgvd.obj sspgvx.obj ssprfs.obj sspsv.obj  sspsvx.obj ssptrd.obj \
   ssptrf.obj ssptri.obj ssptrs.obj sstegr.obj sstein.obj sstev.obj  sstevd.obj sstevr.obj \
   sstevx.obj ssycon.obj ssyev.obj  ssyevd.obj ssyevr.obj ssyevx.obj ssygs2.obj \
   ssygst.obj ssygv.obj  ssygvd.obj ssygvx.obj ssyrfs.obj ssysv.obj  ssysvx.obj \
   ssytd2.obj ssytf2.obj ssytrd.obj ssytrf.obj ssytri.obj ssytrs.obj stbcon.obj \
   stbrfs.obj stbtrs.obj stgevc.obj stgex2.obj stgexc.obj stgsen.obj \
   stgsja.obj stgsna.obj stgsy2.obj stgsyl.obj stpcon.obj stprfs.obj stptri.obj \
   stptrs.obj \
   strcon.obj strevc.obj strexc.obj strrfs.obj strsen.obj strsna.obj strsyl.obj \
   strti2.obj strtri.obj strtrs.obj stzrqf.obj stzrzf.obj

CLASRC = \
   cbdsqr.obj cgbbrd.obj cgbcon.obj cgbequ.obj cgbrfs.obj cgbsv.obj  cgbsvx.obj \
   cgbtf2.obj cgbtrf.obj cgbtrs.obj cgebak.obj cgebal.obj cgebd2.obj cgebrd.obj \
   cgecon.obj cgeequ.obj cgees.obj  cgeesx.obj cgeev.obj  cgeevx.obj \
   cgegs.obj  cgegv.obj  cgehd2.obj cgehrd.obj cgelq2.obj cgelqf.obj \
   cgels.obj  cgelsd.obj cgelss.obj cgelsx.obj cgelsy.obj cgeql2.obj cgeqlf.obj cgeqp3.obj \
   cgeqpf.obj cgeqr2.obj cgeqrf.obj cgerfs.obj cgerq2.obj cgerqf.obj \
   cgesc2.obj cgesdd.obj cgesv.obj  cgesvd.obj cgesvx.obj cgetc2.obj cgetf2.obj cgetrf.obj \
   cgetri.obj cgetrs.obj \
   cggbak.obj cggbal.obj cgges.obj  cggesx.obj cggev.obj  cggevx.obj cggglm.obj \
   cgghrd.obj cgglse.obj cggqrf.obj cggrqf.obj \
   cggsvd.obj cggsvp.obj \
   cgtcon.obj cgtrfs.obj cgtsv.obj  cgtsvx.obj cgttrf.obj cgttrs.obj cgtts2.obj chbev.obj  \
   chbevd.obj chbevx.obj chbgst.obj chbgv.obj  chbgvd.obj chbgvx.obj chbtrd.obj \
   checon.obj cheev.obj  cheevd.obj cheevr.obj cheevx.obj chegs2.obj chegst.obj \
   chegv.obj  chegvd.obj chegvx.obj cherfs.obj chesv.obj  chesvx.obj chetd2.obj \
   chetf2.obj chetrd.obj \
   chetrf.obj chetri.obj chetrs.obj chgeqz.obj chpcon.obj chpev.obj  chpevd.obj \
   chpevx.obj chpgst.obj chpgv.obj  chpgvd.obj chpgvx.obj chprfs.obj chpsv.obj  \
   chpsvx.obj \
   chptrd.obj chptrf.obj chptri.obj chptrs.obj chsein.obj chseqr.obj clabrd.obj \
   clacgv.obj clacon.obj clacp2.obj clacpy.obj clacrm.obj clacrt.obj cladiv.obj \
   claed0.obj claed7.obj claed8.obj \
   claein.obj claesy.obj claev2.obj clags2.obj clagtm.obj \
   clahef.obj clahqr.obj \
   clahrd.obj claic1.obj clals0.obj clalsa.obj clalsd.obj clangb.obj clange.obj clangt.obj \
   clanhb.obj clanhe.obj \
   clanhp.obj clanhs.obj clanht.obj clansb.obj clansp.obj clansy.obj clantb.obj \
   clantp.obj clantr.obj clapll.obj clapmt.obj clarcm.obj claqgb.obj claqge.obj \
   claqhb.obj claqhe.obj claqhp.obj claqp2.obj claqps.obj claqsb.obj \
   claqsp.obj claqsy.obj clar1v.obj clar2v.obj clarf.obj  clarfb.obj clarfg.obj clarft.obj \
   clarfx.obj clargv.obj clarnv.obj clarrv.obj clartg.obj clartv.obj \
   clarz.obj  clarzb.obj clarzt.obj clascl.obj claset.obj clasr.obj  classq.obj \
   claswp.obj clasyf.obj clatbs.obj clatdf.obj clatps.obj clatrd.obj clatrs.obj clatrz.obj \
   clatzm.obj clauu2.obj clauum.obj cpbcon.obj cpbequ.obj cpbrfs.obj cpbstf.obj cpbsv.obj  \
   cpbsvx.obj cpbtf2.obj cpbtrf.obj cpbtrs.obj cpocon.obj cpoequ.obj cporfs.obj \
   cposv.obj  cposvx.obj cpotf2.obj cpotrf.obj cpotri.obj cpotrs.obj cppcon.obj \
   cppequ.obj cpprfs.obj cppsv.obj  cppsvx.obj cpptrf.obj cpptri.obj cpptrs.obj \
   cptcon.obj cpteqr.obj cptrfs.obj cptsv.obj  cptsvx.obj cpttrf.obj cpttrs.obj cptts2.obj \
   crot.obj   cspcon.obj cspmv.obj  cspr.obj   csprfs.obj cspsv.obj  \
   cspsvx.obj csptrf.obj csptri.obj csptrs.obj csrot.obj  csrscl.obj cstedc.obj \
   cstegr.obj cstein.obj csteqr.obj csycon.obj csymv.obj  \
   csyr.obj   csyrfs.obj csysv.obj  csysvx.obj csytf2.obj csytrf.obj csytri.obj \
   csytrs.obj ctbcon.obj ctbrfs.obj ctbtrs.obj ctgevc.obj ctgex2.obj \
   ctgexc.obj ctgsen.obj ctgsja.obj ctgsna.obj ctgsy2.obj ctgsyl.obj ctpcon.obj \
   ctprfs.obj ctptri.obj \
   ctptrs.obj ctrcon.obj ctrevc.obj ctrexc.obj ctrrfs.obj ctrsen.obj ctrsna.obj \
   ctrsyl.obj ctrti2.obj ctrtri.obj ctrtrs.obj ctzrqf.obj ctzrzf.obj cung2l.obj cung2r.obj \
   cungbr.obj cunghr.obj cungl2.obj cunglq.obj cungql.obj cungqr.obj cungr2.obj \
   cungrq.obj cungtr.obj cunm2l.obj cunm2r.obj cunmbr.obj cunmhr.obj cunml2.obj \
   cunmlq.obj cunmql.obj cunmqr.obj cunmr2.obj cunmr3.obj cunmrq.obj cunmrz.obj \
   cunmtr.obj cupgtr.obj cupmtr.obj icmax1.obj scsum1.obj

DLASRC = \
   dgbbrd.obj dgbcon.obj dgbequ.obj dgbrfs.obj dgbsv.obj  \
   dgbsvx.obj dgbtf2.obj dgbtrf.obj dgbtrs.obj dgebak.obj dgebal.obj dgebd2.obj \
   dgebrd.obj dgecon.obj dgeequ.obj dgees.obj  dgeesx.obj dgeev.obj  dgeevx.obj \
   dgegs.obj  dgegv.obj  dgehd2.obj dgehrd.obj dgelq2.obj dgelqf.obj \
   dgels.obj  dgelsd.obj dgelss.obj dgelsx.obj dgelsy.obj dgeql2.obj dgeqlf.obj \
   dgeqp3.obj dgeqpf.obj dgeqr2.obj dgeqrf.obj dgerfs.obj dgerq2.obj dgerqf.obj \
   dgesc2.obj dgesdd.obj dgesv.obj  dgesvd.obj dgesvx.obj dgetc2.obj dgetf2.obj \
   dgetrf.obj dgetri.obj \
   dgetrs.obj dggbak.obj dggbal.obj dgges.obj  dggesx.obj dggev.obj  dggevx.obj \
   dggglm.obj dgghrd.obj dgglse.obj dggqrf.obj \
   dggrqf.obj dggsvd.obj dggsvp.obj dgtcon.obj dgtrfs.obj dgtsv.obj  \
   dgtsvx.obj dgttrf.obj dgttrs.obj dgtts2.obj dhgeqz.obj \
   dhsein.obj dhseqr.obj dlabrd.obj dlacon.obj \
   dlaein.obj dlaexc.obj dlag2.obj  dlags2.obj dlagtm.obj dlagv2.obj dlahqr.obj \
   dlahrd.obj dlaic1.obj dlaln2.obj dlals0.obj dlalsa.obj dlalsd.obj \
   dlangb.obj dlange.obj dlangt.obj dlanhs.obj dlansb.obj dlansp.obj \
   dlansy.obj dlantb.obj dlantp.obj dlantr.obj dlanv2.obj \
   dlapll.obj dlapmt.obj \
   dlaqgb.obj dlaqge.obj dlaqp2.obj dlaqps.obj dlaqsb.obj dlaqsp.obj dlaqsy.obj \
   dlaqtr.obj dlar1v.obj dlar2v.obj \
   dlarf.obj  dlarfb.obj dlarfg.obj dlarft.obj dlarfx.obj dlargv.obj\
   dlarrv.obj dlartv.obj \
   dlarz.obj  dlarzb.obj dlarzt.obj dlaswp.obj dlasy2.obj dlasyf.obj \
   dlatbs.obj dlatdf.obj dlatps.obj dlatrd.obj dlatrs.obj dlatrz.obj dlatzm.obj dlauu2.obj \
   dlauum.obj dopgtr.obj dopmtr.obj dorg2l.obj dorg2r.obj \
   dorgbr.obj dorghr.obj dorgl2.obj dorglq.obj dorgql.obj dorgqr.obj dorgr2.obj \
   dorgrq.obj dorgtr.obj dorm2l.obj dorm2r.obj \
   dormbr.obj dormhr.obj dorml2.obj dormlq.obj dormql.obj dormqr.obj dormr2.obj \
   dormr3.obj dormrq.obj dormrz.obj dormtr.obj dpbcon.obj dpbequ.obj dpbrfs.obj \
   dpbstf.obj dpbsv.obj  dpbsvx.obj \
   dpbtf2.obj dpbtrf.obj dpbtrs.obj dpocon.obj dpoequ.obj dporfs.obj dposv.obj  \
   dposvx.obj dpotf2.obj dpotrf.obj dpotri.obj dpotrs.obj dppcon.obj dppequ.obj \
   dpprfs.obj dppsv.obj  dppsvx.obj dpptrf.obj dpptri.obj dpptrs.obj dptcon.obj \
   dpteqr.obj dptrfs.obj dptsv.obj  dptsvx.obj dpttrs.obj dptts2.obj drscl.obj  \
   dsbev.obj  dsbevd.obj dsbevx.obj dsbgst.obj dsbgv.obj  dsbgvd.obj dsbgvx.obj \
   dsbtrd.obj  dspcon.obj dspev.obj  dspevd.obj dspevx.obj dspgst.obj \
   dspgv.obj  dspgvd.obj dspgvx.obj dsprfs.obj dspsv.obj  dspsvx.obj dsptrd.obj \
   dsptrf.obj dsptri.obj dsptrs.obj dstegr.obj dstein.obj dstev.obj  dstevd.obj dstevr.obj \
   dstevx.obj dsycon.obj dsyev.obj  dsyevd.obj dsyevr.obj \
   dsyevx.obj dsygs2.obj dsygst.obj dsygv.obj  dsygvd.obj dsygvx.obj dsyrfs.obj \
   dsysv.obj  dsysvx.obj \
   dsytd2.obj dsytf2.obj dsytrd.obj dsytrf.obj dsytri.obj dsytrs.obj dtbcon.obj \
   dtbrfs.obj dtbtrs.obj dtgevc.obj dtgex2.obj dtgexc.obj dtgsen.obj \
   dtgsja.obj dtgsna.obj dtgsy2.obj dtgsyl.obj dtpcon.obj dtprfs.obj dtptri.obj \
   dtptrs.obj \
   dtrcon.obj dtrevc.obj dtrexc.obj dtrrfs.obj dtrsen.obj dtrsna.obj dtrsyl.obj \
   dtrti2.obj dtrtri.obj dtrtrs.obj dtzrqf.obj dtzrzf.obj

ZLASRC = \
   zbdsqr.obj zgbbrd.obj zgbcon.obj zgbequ.obj zgbrfs.obj zgbsv.obj  zgbsvx.obj \
   zgbtf2.obj zgbtrf.obj zgbtrs.obj zgebak.obj zgebal.obj zgebd2.obj zgebrd.obj \
   zgecon.obj zgeequ.obj zgees.obj  zgeesx.obj zgeev.obj  zgeevx.obj \
   zgegs.obj  zgegv.obj  zgehd2.obj zgehrd.obj zgelq2.obj zgelqf.obj \
   zgels.obj  zgelsd.obj zgelss.obj zgelsx.obj zgelsy.obj zgeql2.obj zgeqlf.obj zgeqp3.obj \
   zgeqpf.obj zgeqr2.obj zgeqrf.obj zgerfs.obj zgerq2.obj zgerqf.obj \
   zgesc2.obj zgesdd.obj zgesv.obj  zgesvd.obj zgesvx.obj zgetc2.obj zgetf2.obj zgetrf.obj \
   zgetri.obj zgetrs.obj \
   zggbak.obj zggbal.obj zgges.obj  zggesx.obj zggev.obj  zggevx.obj zggglm.obj \
   zgghrd.obj zgglse.obj zggqrf.obj zggrqf.obj \
   zggsvd.obj zggsvp.obj \
   zgtcon.obj zgtrfs.obj zgtsv.obj  zgtsvx.obj zgttrf.obj zgttrs.obj zgtts2.obj zhbev.obj  \
   zhbevd.obj zhbevx.obj zhbgst.obj zhbgv.obj  zhbgvd.obj zhbgvx.obj zhbtrd.obj \
   zhecon.obj zheev.obj  zheevd.obj zheevr.obj zheevx.obj zhegs2.obj zhegst.obj \
   zhegv.obj  zhegvd.obj zhegvx.obj zherfs.obj zhesv.obj  zhesvx.obj zhetd2.obj \
   zhetf2.obj zhetrd.obj \
   zhetrf.obj zhetri.obj zhetrs.obj zhgeqz.obj zhpcon.obj zhpev.obj  zhpevd.obj \
   zhpevx.obj zhpgst.obj zhpgv.obj  zhpgvd.obj zhpgvx.obj zhprfs.obj zhpsv.obj  \
   zhpsvx.obj \
   zhptrd.obj zhptrf.obj zhptri.obj zhptrs.obj zhsein.obj zhseqr.obj zlabrd.obj \
   zlacgv.obj zlacon.obj zlacp2.obj zlacpy.obj zlacrm.obj zlacrt.obj zladiv.obj \
   zlaed0.obj zlaed7.obj zlaed8.obj \
   zlaein.obj zlaesy.obj zlaev2.obj zlags2.obj zlagtm.obj \
   zlahef.obj zlahqr.obj \
   zlahrd.obj zlaic1.obj zlals0.obj zlalsa.obj zlalsd.obj zlangb.obj zlange.obj \
   zlangt.obj zlanhb.obj \
   zlanhe.obj \
   zlanhp.obj zlanhs.obj zlanht.obj zlansb.obj zlansp.obj zlansy.obj zlantb.obj \
   zlantp.obj zlantr.obj zlapll.obj zlapmt.obj zlaqgb.obj zlaqge.obj \
   zlaqhb.obj zlaqhe.obj zlaqhp.obj zlaqp2.obj zlaqps.obj zlaqsb.obj \
   zlaqsp.obj zlaqsy.obj zlar1v.obj zlar2v.obj zlarcm.obj zlarf.obj  zlarfb.obj \
   zlarfg.obj zlarft.obj \
   zlarfx.obj zlargv.obj zlarnv.obj zlarrv.obj zlartg.obj zlartv.obj \
   zlarz.obj  zlarzb.obj zlarzt.obj zlascl.obj zlaset.obj zlasr.obj  \
   zlassq.obj zlaswp.obj zlasyf.obj \
   zlatbs.obj zlatdf.obj zlatps.obj zlatrd.obj zlatrs.obj zlatrz.obj zlatzm.obj zlauu2.obj \
   zlauum.obj zpbcon.obj zpbequ.obj zpbrfs.obj zpbstf.obj zpbsv.obj  \
   zpbsvx.obj zpbtf2.obj zpbtrf.obj zpbtrs.obj zpocon.obj zpoequ.obj zporfs.obj \
   zposv.obj  zposvx.obj zpotf2.obj zpotrf.obj zpotri.obj zpotrs.obj zppcon.obj \
   zppequ.obj zpprfs.obj zppsv.obj  zppsvx.obj zpptrf.obj zpptri.obj zpptrs.obj \
   zptcon.obj zpteqr.obj zptrfs.obj zptsv.obj  zptsvx.obj zpttrf.obj zpttrs.obj zptts2.obj \
   zrot.obj   zspcon.obj zspmv.obj  zspr.obj   zsprfs.obj zspsv.obj  \
   zspsvx.obj zsptrf.obj zsptri.obj zsptrs.obj zdrot.obj  zdrscl.obj zstedc.obj \
   zstegr.obj zstein.obj zsteqr.obj zsycon.obj zsymv.obj  \
   zsyr.obj   zsyrfs.obj zsysv.obj  zsysvx.obj zsytf2.obj zsytrf.obj zsytri.obj \
   zsytrs.obj ztbcon.obj ztbrfs.obj ztbtrs.obj ztgevc.obj ztgex2.obj \
   ztgexc.obj ztgsen.obj ztgsja.obj ztgsna.obj ztgsy2.obj ztgsyl.obj ztpcon.obj \
   ztprfs.obj ztptri.obj \
   ztptrs.obj ztrcon.obj ztrevc.obj ztrexc.obj ztrrfs.obj ztrsen.obj ztrsna.obj \
   ztrsyl.obj ztrti2.obj ztrtri.obj ztrtrs.obj ztzrqf.obj ztzrzf.obj zung2l.obj \
   zung2r.obj zungbr.obj zunghr.obj zungl2.obj zunglq.obj zungql.obj zungqr.obj zungr2.obj \
   zungrq.obj zungtr.obj zunm2l.obj zunm2r.obj zunmbr.obj zunmhr.obj zunml2.obj \
   zunmlq.obj zunmql.obj zunmqr.obj zunmr2.obj zunmr3.obj zunmrq.obj zunmrz.obj \
   zunmtr.obj zupgtr.obj \
   zupmtr.obj izmax1.obj dzsum1.obj

single: $(SLASRC) $(ALLAUX) $(SCLAUX) 
	$(ARCH) $(ARCHFLAGS)..\$(LAPACKLIB) $(SLASRC) $(ALLAUX) $(SCLAUX)

complex: $(CLASRC) $(ALLAUX) $(SCLAUX)
	$(ARCH) $(ARCHFLAGS)..\$(LAPACKLIB) $(CLASRC) $(ALLAUX) $(SCLAUX)

double: $(DLASRC) $(ALLAUX) $(DZLAUX)
	$(ARCH) $(ARCHFLAGS)..\$(LAPACKLIB) $(DLASRC) $(ALLAUX) $(DZLAUX)

complex16: $(ZLASRC) $(ALLAUX) $(DZLAUX)
	$(ARCH) $(ARCHFLAGS)..\$(LAPACKLIB) *.obj

slamch.obj: slamch.f
	$(FORTRAN) $(NOOPT) -c slamch.f
dlamch.obj: dlamch.f
	$(FORTRAN) $(NOOPT) -c dlamch.f

.f.obj: 
	$(FORTRAN) $(OPTS) -c $<

clean:
	del *.obj

