
FILE( GLOB f_src SRC/*.f UTIL/*.f )
FILE( GLOB c_src f2c/*.c )
IF( NOFORTRAN )
	ADD_LIBRARY( arpack ${c_src} )
ELSE( NOFORTRAN )
	ADD_LIBRARY( arpack ${f_src} )
ENDIF( NOFORTRAN )
#WRITE_OUT_ALL_VARIABLES()
