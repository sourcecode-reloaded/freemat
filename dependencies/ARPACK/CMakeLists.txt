
FILE( GLOB f_src SRC/*.f UTIL/*.f )
IF( MSVC )
	CONVERT_F2C( arpack c_src ${f_src} )
	ADD_LIBRARY( arpack ${c_src} )
ELSE( MSVC )
	ADD_LIBRARY( arpack ${f_src} )
ENDIF( MSVC )
#WRITE_OUT_ALL_VARIABLES()