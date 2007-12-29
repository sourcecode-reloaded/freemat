!include <..\..\make.inc>

########################################################################
#
#  Windows NT Digital Fortran makefile
#
#  This is the makefile for the eigenvalue test program from LAPACK.
#  The test files are organized as follows:
#
#     AEIGTST -- Auxiliary test routines used in all precisions
#     SCIGTST -- Auxiliary test routines used in REAL and COMPLEX
#     DZIGTST -- Auxiliary test routines used in DOUBLE PRECISION and
#                COMPLEX*16
#     SEIGTST -- Single precision real test routines
#     CEIGTST -- Single precision complex test routines
#     DEIGTST -- Double precision real test routines
#     ZEIGTST -- Double precision complex test routines
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
#       xeigtsts, xeigtstd, xeigtstc, and xeigtstz
#  and are created in the next higher directory level.
#
#  To remove the object files after the executable files have been
#  created, enter
#       make clean
#
########################################################################

.SUFFIXES: .f .obj .exe .lib

AEIGTST = \
   alahdg.obj \
   alasum.obj \
   alasvm.obj \
   alareq.obj \
   ilaenv.obj \
   xerbla.obj \
   xlaenv.obj \
   chkxer.obj

SCIGTST = slafts.obj slahd2.obj slasum.obj slatb9.obj sstech.obj sstect.obj \
   ssvdch.obj ssvdct.obj ssxt1.obj

SEIGTST = schkee.obj \
   sbdt01.obj sbdt02.obj sbdt03.obj \
   schkbb.obj schkbd.obj schkbk.obj schkbl.obj schkec.obj \
   schkgg.obj schkgk.obj schkgl.obj schkhs.obj schksb.obj schkst.obj \
   sckglm.obj sckgqr.obj sckgsv.obj scklse.obj \
   sdrges.obj sdrgev.obj sdrgsx.obj sdrgvx.obj \
   sdrvbd.obj sdrves.obj sdrvev.obj sdrvgg.obj sdrvsg.obj \
   sdrvst.obj sdrvsx.obj sdrvvx.obj \
   serrbd.obj serrec.obj serred.obj serrgg.obj serrhs.obj serrst.obj \
   sget02.obj sget10.obj sget22.obj sget23.obj sget24.obj sget31.obj \
   sget32.obj sget33.obj sget34.obj sget35.obj sget36.obj \
   sget37.obj sget38.obj sget39.obj sget51.obj sget52.obj sget53.obj \
   sget54.obj sglmts.obj sgqrts.obj sgrqts.obj sgsvts.obj \
   shst01.obj slarfy.obj slarhs.obj slatm4.obj slctes.obj slctsx.obj slsets.obj sort01.obj \
   sort03.obj ssbt21.obj ssgt01.obj sslect.obj sspt21.obj sstt21.obj \
   sstt22.obj ssyt21.obj ssyt22.obj

CEIGTST = cchkee.obj \
   cbdt01.obj cbdt02.obj cbdt03.obj \
   cchkbb.obj cchkbd.obj cchkbk.obj cchkbl.obj cchkec.obj \
   cchkgg.obj cchkgk.obj cchkgl.obj cchkhb.obj cchkhs.obj cchkst.obj \
   cckglm.obj cckgqr.obj cckgsv.obj ccklse.obj \
   cdrges.obj cdrgev.obj cdrgsx.obj cdrgvx.obj \
   cdrvbd.obj cdrves.obj cdrvev.obj cdrvgg.obj cdrvsg.obj \
   cdrvst.obj cdrvsx.obj cdrvvx.obj \
   cerrbd.obj cerrec.obj cerred.obj cerrgg.obj cerrhs.obj cerrst.obj \
   cget02.obj cget10.obj cget22.obj cget23.obj cget24.obj \
   cget35.obj cget36.obj cget37.obj cget38.obj cget51.obj cget52.obj \
   cget54.obj cglmts.obj cgqrts.obj cgrqts.obj cgsvts.obj \
   chbt21.obj chet21.obj chet22.obj chpt21.obj chst01.obj \
   clarfy.obj clarhs.obj clatm4.obj clctes.obj clctsx.obj clsets.obj csbmv.obj \
   csgt01.obj cslect.obj \
   cstt21.obj cstt22.obj cunt01.obj cunt03.obj

DZIGTST = dlafts.obj dlahd2.obj dlasum.obj dlatb9.obj dstech.obj dstect.obj \
   dsvdch.obj dsvdct.obj dsxt1.obj

DEIGTST = dchkee.obj \
   dbdt01.obj dbdt02.obj dbdt03.obj \
   dchkbb.obj dchkbd.obj dchkbk.obj dchkbl.obj dchkec.obj \
   dchkgg.obj dchkgk.obj dchkgl.obj dchkhs.obj dchksb.obj dchkst.obj \
   dckglm.obj dckgqr.obj dckgsv.obj dcklse.obj \
   ddrges.obj ddrgev.obj ddrgsx.obj ddrgvx.obj \
   ddrvbd.obj ddrves.obj ddrvev.obj ddrvgg.obj ddrvsg.obj \
   ddrvst.obj ddrvsx.obj ddrvvx.obj \
   derrbd.obj derrec.obj derred.obj derrgg.obj derrhs.obj derrst.obj \
   dget02.obj dget10.obj dget22.obj dget23.obj dget24.obj dget31.obj \
   dget32.obj dget33.obj dget34.obj dget35.obj dget36.obj \
   dget37.obj dget38.obj dget39.obj dget51.obj dget52.obj dget53.obj \
   dget54.obj dglmts.obj dgqrts.obj dgrqts.obj dgsvts.obj \
   dhst01.obj dlarfy.obj dlarhs.obj dlatm4.obj dlctes.obj dlctsx.obj dlsets.obj dort01.obj \
   dort03.obj dsbt21.obj dsgt01.obj dslect.obj dspt21.obj dstt21.obj \
   dstt22.obj dsyt21.obj dsyt22.obj

ZEIGTST = zchkee.obj \
   zbdt01.obj zbdt02.obj zbdt03.obj \
   zchkbb.obj zchkbd.obj zchkbk.obj zchkbl.obj zchkec.obj \
   zchkgg.obj zchkgk.obj zchkgl.obj zchkhb.obj zchkhs.obj zchkst.obj \
   zckglm.obj zckgqr.obj zckgsv.obj zcklse.obj \
   zdrges.obj zdrgev.obj zdrgsx.obj zdrgvx.obj \
   zdrvbd.obj zdrves.obj zdrvev.obj zdrvgg.obj zdrvsg.obj \
   zdrvst.obj zdrvsx.obj zdrvvx.obj \
   zerrbd.obj zerrec.obj zerred.obj zerrgg.obj zerrhs.obj zerrst.obj \
   zget02.obj zget10.obj zget22.obj zget23.obj zget24.obj \
   zget35.obj zget36.obj zget37.obj zget38.obj zget51.obj zget52.obj \
   zget54.obj zglmts.obj zgqrts.obj zgrqts.obj zgsvts.obj \
   zhbt21.obj zhet21.obj zhet22.obj zhpt21.obj zhst01.obj \
   zlarfy.obj zlarhs.obj zlatm4.obj zlctes.obj zlctsx.obj zlsets.obj zsbmv.obj \
   zsgt01.obj zslect.obj \
   zstt21.obj zstt22.obj zunt01.obj zunt03.obj

all: single complex double complex16

single: ..\xeigtsts.exe
complex: ..\xeigtstc.exe
double: ..\xeigtstd.exe
complex16: ..\xeigtstz.exe

..\xeigtsts.exe: $(SEIGTST) $(SCIGTST) $(AEIGTST)
	$(LOADER) $(LOADOPTS) -exe:$@ $(SEIGTST) $(SCIGTST) $(AEIGTST) \
	..\..\$(TMGLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

..\xeigtstc.exe: $(CEIGTST) $(SCIGTST) $(AEIGTST)
	$(LOADER) $(LOADOPTS) -exe:$@ $(CEIGTST) $(SCIGTST) $(AEIGTST) \
	..\..\$(TMGLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

..\xeigtstd.exe: $(DEIGTST) $(DZIGTST) $(AEIGTST)
	$(LOADER) $(LOADOPTS) -exe:$@ $(DEIGTST) $(DZIGTST) $(AEIGTST) \
	..\..\$(TMGLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

..\xeigtstz.exe: $(ZEIGTST) $(DZIGTST) $(AEIGTST)
	$(LOADER) $(LOADOPTS) -exe:$@ $(ZEIGTST) $(DZIGTST) $(AEIGTST) \
	..\..\$(TMGLIB) ..\..\$(LAPACKLIB) $(BLASLIB)

clean:
	del *.obj

schkee.o: schkee.f
	$(FORTRAN) $(DRVOPTS) -c schkee.f
dchkee.o: dchkee.f
	$(FORTRAN) $(DRVOPTS) -c dchkee.f
cchkee.o: cchkee.f
	$(FORTRAN) $(DRVOPTS) -c cchkee.f
zchkee.o: zchkee.f
	$(FORTRAN) $(DRVOPTS) -c zchkee.f

.f.obj:
	$(FORTRAN) $(OPTS) -c $<
