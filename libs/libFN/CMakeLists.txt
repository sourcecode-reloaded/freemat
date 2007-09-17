IF( !WIN32 )
PROJECT( XP Fortran )
ENDIF( !WIN32 )

INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

SET(F_FILES algama.f 
calerf.f 
daw.f 
dcalerf.f 
ddaw.f 
dei.f 
derf.f 
dgamma.f 
di0.f 
di1.f 
dj0y0.f 
dj1y1.f 
dk0.f 
dk1.f 
dlgama.f 
dpmpar.f 
dpsi.f 
ei.f 
enorm.f 
erf.f 
fdjac2.f 
gamma.f 
i0.f 
i1.f 
j0y0.f 
j1y1.f 
k0.f 
k1.f 
lmdif1.f 
lmdif.f 
lmpar.f 
psi.f 
qrfac.f 
qrsolv.f 
ribesl.f 
rjbesl.f 
rkbesl.f 
rybesl.f )

IF( WIN32 AND F2C_PROG )
	FOREACH( nm ${F_FILES} )
		GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
	
		ADD_CUSTOM_COMMAND(OUTPUT ${outfile}.c 
		  COMMAND ${F2C_PROG} ARGS ${F2C_FLAGS} ${nm}
		  MAIN_DEPENDENCY FN
		  DEPENDS ${nm}
		  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		) 
		MACRO_ADD_FILE_DEPENDENCIES(${nm} ${outfile}.c )
		SET( C_FILES ${C_FILES} ${outfile}.c )
	ENDFOREACH( nm ${F_FILES} )
	SET( F_FILES ${C_FILES} )
ENDIF( WIN32 AND F2C_PROG )



ADD_LIBRARY( FN
FNFun.cpp 
Interp1D.cpp 
LoadFN.cpp 
OptFun.cpp 
${F_FILES}  
)
#SET_SOURCE_FILES_PROPERTIES( ${F_FILES} PROPERTIES HEADER_FILE_ONLY TRUE )
SET_TARGET_PROPERTIES( FN PROPERTIES LINKER_LANGUAGE CXX )
