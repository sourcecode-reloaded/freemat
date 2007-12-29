# IF( NOT MSVC )
#   PROJECT( XP Fortran )
# ENDIF( NOT MSVC )

INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

SET(F_FILES 
#algama.f 
#calerf.f 
#daw.f 
#dcalerf.f 
#ddaw.f 
#dei.f 
#derf.f 
#dgamma.f 
#di0.f 
#di1.f 
#dj0y0.f 
#dj1y1.f 
#dk0.f 
#dk1.f 
#dlgama.f 
dpmpar.f 
#dpsi.f 
#ei.f 
enorm.f 
#erf.f 
fdjac2.f 
#gamma.f 
#i0.f 
#i1.f 
#j0y0.f 
#j1y1.f 
#k0.f 
#k1.f 
lmdif1.f 
lmdif.f 
lmpar.f 
#psi.f 
qrfac.f 
qrsolv.f 
#ribesl.f 
#rjbesl.f 
#rkbesl.f 
#rybesl.f 
)

IF( MSVC )
  CONVERT_F2C( FN c_src ${F_FILES} )
  SET(F_FILES ${c_src} )
  SET(LIBM_FILES 
	 mathfunc.cpp
	 mathfunc2.cpp
	 mathfunc3.cpp
	 mathfunc4.cpp
  )
ENDIF( MSVC )

ADD_LIBRARY( FN
FNFun.cpp 
Interp1D.cpp 
LoadFN.cpp 
OptFun.cpp 
${F_FILES}
${LIBM_FILES}
)
#SET_SOURCE_FILES_PROPERTIES( ${F_FILES} PROPERTIES HEADER_FILE_ONLY TRUE )
SET_TARGET_PROPERTIES( FN PROPERTIES LINKER_LANGUAGE CXX )
WRITE_OUT_ALL_VARIABLES()
