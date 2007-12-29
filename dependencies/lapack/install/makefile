#
#  Sample Windows NT makefile (nmake) and Digital Fortran
#

!include <../make.inc>

.SUFFIXES: .f .obj .exe

all:  testlsame.exe testslamch.exe testdlamch.exe testsecond.exe testdsecnd.exe testieee.exe

testlsame.exe:  lsame.obj lsametst.obj
	$(LOADER) $(LOADOPTS) -exe:testlsame.exe lsame.obj lsametst.obj

testslamch.exe: slamch.obj lsame.obj slamchtst.obj
	$(LOADER) $(LOADOPTS) -exe:testslamch.exe slamch.obj lsame.obj slamchtst.obj

testdlamch.exe: dlamch.obj lsame.obj dlamchtst.obj
	$(LOADER) $(LOADOPTS) -exe:testdlamch.exe dlamch.obj lsame.obj dlamchtst.obj

testsecond.exe: second.obj secondtst.obj
	$(LOADER) $(LOADOPTS) -exe:testsecond.exe second.obj secondtst.obj

testdsecnd.exe: dsecnd.obj dsecndtst.obj
	$(LOADER) $(LOADOPTS) -exe:testdsecnd.exe dsecnd.obj dsecndtst.obj

testieee.exe: tstiee.obj
	$(LOADER) $(LOADOPTS) -exe:testieee.exe tstiee.obj

clean:
	del *.obj

slamch.obj: slamch.f ; $(FORTRAN) $(NOOPT) -c slamch.f
dlamch.obj: dlamch.f ; $(FORTRAN) $(NOOPT) -c dlamch.f

.f.obj: ; $(FORTRAN) $(OPTS) -c $<
