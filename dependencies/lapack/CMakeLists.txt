FILE( GLOB f_src src/*.f )
IF( MSVC )
	CONVERT_F2C( lapack c_src ${f_src} )
	ADD_LIBRARY( lapack ${c_src} src/dsecnd.c src/second.c )
ELSE( MSVC )
	ADD_LIBRARY( lapack ${f_src} )
ENDIF( MSVC )
GET_TARGET_PROPERTY(LAPACK_LIBRARY lapack LOCATION)
WRITE_OUT_ALL_VARIABLES()


